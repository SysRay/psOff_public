#pragma once
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

} // namespace vulkan

class IEventsGraphics {
  public:
  virtual void eventDoFlip(int handle, int index, int64_t flipArg, VkSemaphore waitSema, size_t waitValue) = 0;

  virtual vulkan::DeviceInfo* getDeviceInfo() = 0;

  virtual std::pair<VkQueue, uint32_t> getQueue(vulkan::QueueType type) = 0;
};

namespace Kernel::EventQueue {
struct KernelEqueueEvent;
class KernelEqueue;
} // namespace Kernel::EventQueue

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
  virtual int addEvent(Kernel::EventQueue::KernelEqueueEvent& event, Kernel::EventQueue::KernelEqueue* eq) = 0;

  /**
   * @brief Removes a GRAPHICS_EVENTS event
   *
   * @param eq
   * @param ident
   */
  virtual void removeEvent(Kernel::EventQueue::KernelEqueue* eq, int const ident) = 0;

  /**
   * @brief Currently used to preprocess the command buffer.
   * todo: will be hooked in the future. and replaced
   *
   * @param curEndAddr end address of the command buffer
   */
  virtual void updateCmdBuffer(uint64_t curEndAddr) = 0;

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
};

std::unique_ptr<IGraphics> createGraphics(IEventsGraphics& listener);