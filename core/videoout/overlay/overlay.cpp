#include "overlay.h"

#include "logging.h"
#include "overtrophy/overtrophy.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

LOG_DEFINE_MODULE(Overlay);

class OverlayHandler: public IOverlayHandler {
  std::shared_ptr<vulkan::DeviceInfo> m_deviceInfo;
  std::shared_ptr<IImageHandler>      m_imageHandler;

  VkDescriptorPool m_descriptorPool;

  bool m_isInit = false, m_isStop = false;

  void init(SDL_Window* window, vulkan::QueueInfo* queue, VkFormat displayFormat);
  void draw();

  public:
  OverlayHandler(std::shared_ptr<vulkan::DeviceInfo>& deviceInfo, std::shared_ptr<IImageHandler>& imagehandler, SDL_Window* window, vulkan::QueueInfo* queue,
                 VkFormat displayFormat)
      : m_deviceInfo(deviceInfo), m_imageHandler(imagehandler) {
    init(window, queue, displayFormat);
  }

  virtual ~OverlayHandler() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    vkDestroyDescriptorPool(m_deviceInfo->device, m_descriptorPool, nullptr);
  }

  // ### Interface
  void stop() final { m_isStop = true; }

  void submit(ImageData const& imageData) final;

  void processEvent(SDL_Event const* event) final {
    [[unlikely]] if (!m_isInit || m_isStop)
      return;
    ImGui_ImplSDL2_ProcessEvent(event);
  }
};

std::unique_ptr<IOverlayHandler> createOverlay(std::shared_ptr<vulkan::DeviceInfo>& deviceInfo, std::shared_ptr<IImageHandler>& imagehandler,
                                               SDL_Window* window, vulkan::QueueInfo* queue, VkFormat displayFormat) {
  return std::make_unique<OverlayHandler>(deviceInfo, imagehandler, window, queue, displayFormat);
}

void OverlayHandler::init(SDL_Window* window, vulkan::QueueInfo* queue, VkFormat displayFormat) {
  LOG_USE_MODULE(Overlay);

  // Create intern vulkan data
  VkDescriptorPoolSize poolSizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                                      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                                      {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                                      {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                                      {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                                      {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                                      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                                      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                                      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                                      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                                      {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

  VkDescriptorPoolCreateInfo poolInfo = {
      .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
      .flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
      .maxSets       = 1000,
      .poolSizeCount = std::size(poolSizes),
      .pPoolSizes    = poolSizes,
  };

  if (auto res = vkCreateDescriptorPool(m_deviceInfo->device, &poolInfo, nullptr, &m_descriptorPool); res != VK_SUCCESS) {
    LOG_ERR(L"Couldn't create descriptor pool %S", string_VkResult(res));
    return;
  }

  // -

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO& io = ImGui::GetIO();

  io.IniFilename = nullptr;
  io.LogFilename = nullptr;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  ImGui_ImplSDL2_InitForVulkan(window);

  ImGui_ImplVulkan_InitInfo initInfo = {
      .Instance            = m_deviceInfo->instance,
      .PhysicalDevice      = m_deviceInfo->physicalDevice,
      .Device              = m_deviceInfo->device,
      .QueueFamily         = queue->family,
      .Queue               = queue->queue,
      .DescriptorPool      = m_descriptorPool,
      .RenderPass          = nullptr, // Dynamic
      .MinImageCount       = 2,
      .ImageCount          = 3,
      .MSAASamples         = VK_SAMPLE_COUNT_1_BIT,
      .PipelineCache       = nullptr,
      .Subpass             = 0,
      .UseDynamicRendering = true,
      .PipelineRenderingCreateInfo =
          {
              .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
              .colorAttachmentCount    = 1,
              .pColorAttachmentFormats = &displayFormat,
          },

      .Allocator       = nullptr,
      .CheckVkResultFn = nullptr,

  };

  ImGui_ImplVulkan_Init(&initInfo);
  (void)accessTrophyOverlay(); // Should be called there to avoid initialization inside NewFrame()

  m_isInit = true;
}

void OverlayHandler::submit(ImageData const& imageData) {
  [[unlikely]] if (!m_isInit || m_isStop)
    return;

  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
  draw();
  ImGui::EndFrame();
  ImGui::Render();
  ImDrawData* drawData = ImGui::GetDrawData();

  VkRenderingAttachmentInfo colorInfo {
      .sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
      .imageView   = imageData.imageView,
      .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .resolveMode = VK_RESOLVE_MODE_NONE,
      .loadOp      = VK_ATTACHMENT_LOAD_OP_LOAD,
      .storeOp     = VK_ATTACHMENT_STORE_OP_STORE,
      .clearValue  = {},
  };

  VkRenderingInfo renderingInfo {
      .sType                = VK_STRUCTURE_TYPE_RENDERING_INFO,
      .renderArea           = {{}, imageData.extent},
      .layerCount           = 1,
      .colorAttachmentCount = 1,
      .pColorAttachments    = &colorInfo,
      .pDepthAttachment     = nullptr,
      .pStencilAttachment   = nullptr,
  };

  vkCmdBeginRendering(imageData.cmdBuffer, &renderingInfo);
  ImGui_ImplVulkan_RenderDrawData(drawData, imageData.cmdBuffer);
  vkCmdEndRendering(imageData.cmdBuffer);
}

void OverlayHandler::draw() {
  // ImGui::ShowDemoWindow();
  accessTrophyOverlay().draw(m_imageHandler->getFPS());
}
