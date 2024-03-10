#pragma once

#include "utility/utility.h"
#include "vk_mem_alloc.h"

enum class ImageType : uint8_t { ColorAttachment, DepthStencilTarget, Image, Buffer, NUM_ITEMS };

struct HWContext;

class IGpuMemoryObject {
  CLASS_NO_COPY(IGpuMemoryObject);
  CLASS_NO_MOVE(IGpuMemoryObject);

  uint64_t m_lastSubmitId = 0;

  void notifyNewSubmit() {
    m_srcAccessMask = VK_ACCESS_NONE;
    m_srcStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    m_checkUpdate   = true;
  }

  protected:
  uint64_t m_vaddr = 0;
  uint64_t m_size  = 0;

  uint32_t m_srcOffset = 0;       /// vaddr - HostAddr
  VkBuffer m_srcBuf    = nullptr; /// Host allocated

  VkAccessFlagBits        m_srcAccessMask = VK_ACCESS_NONE;
  VkPipelineStageFlagBits m_srcStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

  bool         m_checkUpdate          = false;
  virtual void update(HWContext& ctx) = 0;

  public:
  VmaAllocation     m_allocation;
  VmaAllocationInfo m_allocInfo;

  ImageType const type = ImageType::NUM_ITEMS;

  virtual bool create(VmaAllocator& allocator, HWContext* ctx)  = 0;
  virtual void dispose(VkDevice device, VmaAllocator allocator) = 0;

  virtual void writeBack(VkCommandBuffer writeBackBuffer) = 0;

  explicit IGpuMemoryObject(ImageType type, VkBuffer srcBuffer, uint32_t srcOffset): type(type), m_srcBuf(srcBuffer), m_srcOffset(srcOffset) {}

  inline auto getBaseAddr() const { return m_vaddr; }

  inline auto getBaseOffset() const { return m_srcOffset; }

  inline auto getSize() const { return m_size; }

  auto getSrcBuffer() const { return m_srcBuf; }

  auto getCurStage() const { return m_srcStage; }

  bool getCheckUpdate() const { return m_checkUpdate; }

  void setSubmitId(uint64_t submitId) {
    if (m_lastSubmitId != submitId) notifyNewSubmit();
    m_lastSubmitId = submitId;
  }

  inline auto getSubmitId() const { return m_lastSubmitId; }

  void doUpdate(HWContext& ctx) {
    if (!m_checkUpdate) return;
    m_checkUpdate = false;
    update(ctx);
  }

  virtual ~IGpuMemoryObject() = default;
};

struct RenderTarget;

namespace sb2spirv {
struct TextureResource;
}

class IGpuImageObject: public IGpuMemoryObject {
  protected:
  VkImage       m_vkImage     = nullptr;
  VkFormat      m_format      = VK_FORMAT_UNDEFINED;
  VkImageLayout m_imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

  VkImageSubresourceRange m_subresource;
  VkImageAspectFlags      m_aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;

  VkExtent3D m_extent;

  VkBuffer     m_bufferAlias     = nullptr;
  VkBufferView m_bufferViewAlias = nullptr;

  bool m_needsWriteBack = false;

  public:
  explicit IGpuImageObject(ImageType type, VkBuffer srcBuffer, uint32_t srcOffset): IGpuMemoryObject(type, srcBuffer, srcOffset) {}

  virtual bool equals(RenderTarget const& renderTarget) const         = 0;
  virtual bool equals(sb2spirv::TextureResource const& texture) const = 0;

  inline auto getFormat() const { return m_format; }

  inline auto getImage() const { return m_vkImage; }

  inline auto getAspect() const { return m_aspectFlag; }

  inline auto getImageLayout() const { return m_imageLayout; }

  inline auto getExtent() const { return m_extent; }

  void setWriteBack(bool writeBack) { m_needsWriteBack = writeBack; }

  bool getWriteBack() const { return m_needsWriteBack; }

  inline auto getSubresource() const { return m_subresource; }

  VkImageView createImageView(VkDevice device, VkFormat format, VkComponentMapping swizzle);

  std::pair<VkBuffer, VkBufferView> createAlias(VkDevice device);

  virtual ~IGpuImageObject() = default;

  VkImageMemoryBarrier readBarrier(VkAccessFlagBits dstAccessMask, VkPipelineStageFlagBits dstStage, VkImageLayout layout);

  void setWrite(VkAccessFlagBits accessMask, VkPipelineStageFlagBits stage) {
    m_srcAccessMask  = accessMask;
    m_srcStage       = stage;
    m_needsWriteBack = false;
  }

  void setImageLayout(VkImageLayout layout) { m_imageLayout = layout; }
};