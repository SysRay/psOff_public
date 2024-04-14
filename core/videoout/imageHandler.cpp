#include "imageHandler.h"

#include "core/initParams/initParams.h"
#include "logging.h"
#include "vulkan/vulkanSetup.h"

#include <boost/thread/thread.hpp>
#include <vulkan/vk_enum_string_helper.h>

LOG_DEFINE_MODULE(ImageHandler);

constexpr uint32_t NUM_DISPLAY_BUFFERS = 3;

class ImageHandler: public IImageHandler {
  uint32_t m_maxImages = 1;

  VkDevice m_device;

  // Vulkan internal
  VkSwapchainKHR m_swapchain   = nullptr;
  VkCommandPool  m_commandPool = nullptr;

  VkSwapchainCreateInfoKHR m_swapchainCreateInfo;

  std::vector<VkSemaphore>     m_semsImageReady;
  std::vector<VkSemaphore>     m_semsImageCopied;
  std::vector<VkCommandBuffer> m_commandBuffer;

  std::vector<VkFence> m_fenceSubmit;

  size_t m_nextIndex = 0;

  uint32_t m_countImages = 0;

  std::vector<VkImage> m_scImages;
  // - vulkan

  boost::mutex m_mutexInt;

  size_t m_waitId       = 0;
  size_t m_presentCount = 1;

  boost::condition_variable m_present_condVar; /// regulates number of images

  bool m_stop = false;

  public:
  ImageHandler(VkDevice vkDevice, VkExtent2D extentWindow, vulkan::QueueInfo* queue, ImageHandlerCB* callback)
      : IImageHandler(extentWindow, queue, callback), m_device(vkDevice) {};

  virtual ~ImageHandler() = default;

  void recreate();

  // ### Interace
  void init(vulkan::VulkanObj* obj, VkSurfaceKHR surface) final;
  void deinit() final;

  void stop() final {
    m_stop = true;
    m_present_condVar.notify_all();
  }

  std::optional<ImageData> getImage_blocking() final;

  void notify_done(ImageData const&) final;

  VkSwapchainKHR getSwapchain() const final { return m_swapchain; }
};

std::unique_ptr<IImageHandler> createImageHandler(VkDevice vkDevice, VkExtent2D extentWindow, vulkan::QueueInfo* queue, ImageHandlerCB* callback) {
  return std::make_unique<ImageHandler>(vkDevice, extentWindow, queue, callback);
}

void ImageHandler::recreate() {
  LOG_USE_MODULE(ImageHandler);

  vkDeviceWaitIdle(m_device);

  auto const newExtent = m_callback->getWindowSize();
  if (newExtent.width != m_extentWindow.width || newExtent.height != m_extentWindow.height) {
    m_extentWindow = newExtent;

    m_swapchainCreateInfo.imageExtent  = m_extentWindow;
    m_swapchainCreateInfo.oldSwapchain = m_swapchain;

    VkSwapchainKHR newSwapchain;
    if (auto result = vkCreateSwapchainKHR(m_device, &m_swapchainCreateInfo, nullptr, &newSwapchain); result != VK_SUCCESS) {
      LOG_CRIT(L"Couldn't recreate swapchain: %d", result);
    }

    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr); // Destroys images as well

    m_swapchain = newSwapchain;

    uint32_t numImages = 0;
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &numImages, nullptr);
    m_scImages.resize(numImages);

    vkGetSwapchainImagesKHR(m_device, m_swapchain, &numImages, m_scImages.data());
  }
}

std::optional<ImageData> ImageHandler::getImage_blocking() {
  LOG_USE_MODULE(ImageHandler);

  boost::unique_lock lock(m_mutexInt);

  // Regulate max images
  auto waitId = ++m_waitId;
  LOG_DEBUG(L"waitId:%llu presentCount:%llu", waitId, m_presentCount);
  m_present_condVar.wait(lock, [this, &waitId] { return m_stop || (waitId == m_presentCount && m_countImages < m_maxImages); });
  if (m_stop) return {};
  // -

  ImageData imageData {
      .semImageReady  = m_semsImageReady[m_nextIndex],
      .semImageCopied = m_semsImageCopied[m_nextIndex],
      .cmdBuffer      = m_commandBuffer[m_nextIndex],
      .submitFence    = m_fenceSubmit[m_nextIndex],
  };

  // Get swapchain image
  {
    int numTries = 2;

    VkResult result = VK_SUCCESS;
    for (; numTries >= 0; --numTries) {
      if (result = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, imageData.semImageReady, VK_NULL_HANDLE, &imageData.index); result != VK_SUCCESS) {
        if (result == VK_NOT_READY) {
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } else if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR) {
          recreate();
          ++numTries;
        }
      } else
        break;
    }
    if (numTries <= 0) {
      LOG_ERR(L"vkAcquireNextImageKHR %S", string_VkResult(result));
      ++m_presentCount; // fake present
      lock.unlock();
      m_present_condVar.notify_all();
      return {};
    }
  }
  // - swapchain image

  m_nextIndex = (++m_nextIndex) % m_maxImages;
  ++m_countImages;

  imageData.swapchainImage = m_scImages[imageData.index];

  imageData.extent = m_extentWindow;

  { // Begin commandbuffer| end is done by user
    VkCommandBufferBeginInfo const beginInfo {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags            = 0,
        .pInheritanceInfo = nullptr,
    };

    if (auto result = vkBeginCommandBuffer(imageData.cmdBuffer, &beginInfo); result != VK_SUCCESS) {
      LOG_CRIT(L"Error vkBeginCommandBuffer: %S", string_VkResult(result));
    }
  }

  return imageData;
}

void ImageHandler::notify_done(ImageData const& imageData) {
  LOG_USE_MODULE(ImageHandler);

  // vkQueuePresentKHR: amd waits and nvidia doesn't -> manual wait
  vkWaitForFences(m_device, 1, &imageData.submitFence, VK_TRUE, UINT64_MAX);
  vkResetFences(m_device, 1, &imageData.submitFence);

  vkResetCommandBuffer(imageData.cmdBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

  boost::unique_lock lock(m_mutexInt);

  ++m_presentCount;
  --m_countImages;
  lock.unlock();

  m_present_condVar.notify_all();
}

void ImageHandler::init(vulkan::VulkanObj* obj, VkSurfaceKHR surface) {
  LOG_USE_MODULE(ImageHandler);

  { // Create Display ready sems and fences
    VkSemaphoreCreateInfo const semCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = 0,
        .flags = 0,
    };

    VkFenceCreateInfo const fenceCreateInfo {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = 0,
        .flags = 0,
    };

    m_semsImageReady.resize(m_maxImages);
    m_semsImageCopied.resize(m_maxImages);
    m_fenceSubmit.resize(m_maxImages);

    for (size_t n = 0; n < m_maxImages; ++n) {
      vkCreateSemaphore(m_device, &semCreateInfo, nullptr, &m_semsImageReady[n]);
      vkCreateSemaphore(m_device, &semCreateInfo, nullptr, &m_semsImageCopied[n]);
      vkCreateFence(obj->deviceInfo.device, &fenceCreateInfo, nullptr, &m_fenceSubmit[n]);
    }
  }

  auto numBuffers = NUM_DISPLAY_BUFFERS;

  { // swapchain
    numBuffers = std::clamp(numBuffers, obj->surfaceCapabilities.capabilities.minImageCount, obj->surfaceCapabilities.capabilities.maxImageCount);

    m_extentWindow.width  = std::clamp(m_extentWindow.width, obj->surfaceCapabilities.capabilities.minImageExtent.width,
                                       obj->surfaceCapabilities.capabilities.maxImageExtent.width);
    m_extentWindow.height = std::clamp(m_extentWindow.height, obj->surfaceCapabilities.capabilities.minImageExtent.height,
                                       obj->surfaceCapabilities.capabilities.maxImageExtent.height);

    auto [displayFormat, displayColorSpace] = getDisplayFormat(obj);
    m_imageFormat                           = displayFormat;

    bool const useVsync = accessInitParams()->useVSYNC();

    m_swapchainCreateInfo = VkSwapchainCreateInfoKHR {
        .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext                 = nullptr,
        .flags                 = 0,
        .surface               = surface,
        .minImageCount         = numBuffers,
        .imageFormat           = m_imageFormat,
        .imageColorSpace       = displayColorSpace,
        .imageExtent           = m_extentWindow,
        .imageArrayLayers      = 1,
        .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = nullptr,
        .preTransform          = obj->surfaceCapabilities.capabilities.currentTransform,
        .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode           = useVsync ? VK_PRESENT_MODE_FIFO_RELAXED_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR,
        .clipped               = VK_TRUE,
        .oldSwapchain          = nullptr,
    };

    vkCreateSwapchainKHR(obj->deviceInfo.device, &m_swapchainCreateInfo, nullptr, &m_swapchain);
  }

  { // swapchain images
    uint32_t numImages = 0;
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &numImages, nullptr);
    m_scImages.resize(numImages);

    vkGetSwapchainImagesKHR(m_device, m_swapchain, &numImages, m_scImages.data());
  }

  { // Command buffer

    VkCommandPoolCreateInfo const poolInfo {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = m_queue->family,
    };

    if (auto result = vkCreateCommandPool(obj->deviceInfo.device, &poolInfo, nullptr, &m_commandPool); result != VK_SUCCESS) {
      LOG_CRIT(L"Couldn't create commandpool(graphics): %d", result);
    }

    m_commandBuffer.resize(m_maxImages);
    VkCommandBufferAllocateInfo const allocInfo {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool        = m_commandPool,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = m_maxImages,
    };

    if (auto result = vkAllocateCommandBuffers(obj->deviceInfo.device, &allocInfo, m_commandBuffer.data()); result != VK_SUCCESS) {
      LOG_CRIT(L"Couldn't create commandbuffers(graphics): %d", result);
    }
  }
}

void ImageHandler::deinit() {
  printf("deinit ImageHandler\n");

  for (auto& sem: m_semsImageReady) {
    vkDestroySemaphore(m_device, sem, nullptr);
  }
  for (auto& sem: m_semsImageCopied) {
    vkDestroySemaphore(m_device, sem, nullptr);
  }
  for (auto& fence: m_fenceSubmit) {
    vkDestroyFence(m_device, fence, nullptr);
  }

  if (m_commandPool != nullptr) vkDestroyCommandPool(m_device, m_commandPool, nullptr);
  if (m_swapchain != nullptr) vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

  printf("deinit ImageHandler| done\n");
}
