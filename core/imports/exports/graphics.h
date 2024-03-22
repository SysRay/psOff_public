#pragma once
#include "core/kernel/eventqueue_types.h"
#include "utility/utility.h"
#include "vulkan/vulkan_core.h"

#include <memory>

enum class GRAPHICS_EVENTS : int {
  Compute0RelMem = 0,
  Compute1RelMem,
  Compute2RelMem,
  Compute3RelMem,
  Compute4RelMem,
  Compute5RelMem,
  Compute6RelMem,
  EOP = 0x40,
};

namespace vulkan {
struct DeviceInfo;

enum class QueueType : uint8_t;

struct QueueInfo;
} // namespace vulkan

class IEventsGraphics {
  public:
  virtual void eventDoFlip(int handle, int index, int64_t flipArg, VkSemaphore waitSema, size_t waitValue) = 0;

  virtual vulkan::DeviceInfo* getDeviceInfo() = 0;

  virtual vulkan::QueueInfo* getQueue(vulkan::QueueType type) = 0;
};

class IGraphics {
  CLASS_NO_COPY(IGraphics);
  CLASS_NO_MOVE(IGraphics);

  protected:
  IGraphics() = default;

  public:
  virtual ~IGraphics() = default;

  /**
   * @brief Adds a GRAPHICS_EVENTS event
   *
   * @param event
   * @param eq
   * @return int result
   */
  virtual int addEvent(Kernel::EventQueue::KernelEqueueEvent& event, Kernel::EventQueue::IKernelEqueue_t eq) = 0;

  /**
   * @brief Removes a GRAPHICS_EVENTS event
   *
   * @param eq
   * @param ident
   */
  virtual void removeEvent(Kernel::EventQueue::IKernelEqueue_t eq, int const ident) = 0;

  /**
   * @brief Add commandbuffer for preparsing.
   * unregistered on submit
   *
   * @param cmdBufferClass
   */
  virtual void registerCommandBuffer(void* cmdBufferClass) = 0;

  /**
   * @brief submits the command buffer to the command processor
   *
   * @param count
   * @param drawBuffers
   * @param numDrawDw
   * @param constBuffers
   * @param numConstDw
   * @param handle
   * @param index
   * @param flipMode
   * @param flipArg
   * @param flip
   */
  virtual void submitCmdBuffer(uint32_t count, uint32_t const* drawBuffers[], uint32_t const numDrawDw[], uint32_t const* constBuffers[],
                               uint32_t const numConstDw[], int const handle, int const index, int const flipMode, int64_t const flipArg, bool flip) = 0;

  /**
   * @brief Wait for all submits (and currently flips todo: check if needed)
   *
   */
  virtual void waitSubmitDone() = 0;

  /**
   * @brief Increase the counter in waitSubmitDone(). Adds a wait condition.
   *
   */
  virtual void submited() = 0;

  /**
   * @brief Notify waitSubmitDone(). Decreases the counter
   *
   */
  virtual void submitDone() = 0;

  /**
   * @brief Check if command processor is running (handling submits)
   *
   * @return true
   * @return false
   */
  virtual bool isRunning() const = 0;

  /**
   * @brief Pre deletion call
   *
   */
  virtual void deinit() = 0;

  /**
   * @brief Register a display buffer
   *
   * @param vaddr
   * @param extent
   * @param pitch
   * @param format
   * @return true
   * @return false
   */
  virtual bool registerDisplayBuffer(uint64_t vaddr, VkExtent2D extent, uint32_t pitch, VkFormat format) = 0;

  /**
   * @brief Notify a gpu visible memory range
   *
   * @return true: Memory has been allocated successfully
   */
  virtual bool notify_allocHeap(uint64_t vaddr, uint64_t size, int memoryProtection) = 0;

  /**
   * @brief Checks if the vaddr is gpu memory (prev notify_allocHeap)
   *
   * @param vaddr
   * @return true is gpu local memory
   * @return false
   */
  virtual bool isGPULocal(uint64_t vaddr) = 0;

  /**
   * @brief Records the copy commands for copying the displayBuffer to the dstImage
   *
   * @param vaddr
   * @param cmdBuffer
   * @param dstImage
   * @param dstExtent
   * @return true success
   */
  virtual bool copyDisplayBuffer(uint64_t vaddr, VkCommandBuffer cmdBuffer, VkImage dstImage, VkExtent2D dstExtent) = 0;
};