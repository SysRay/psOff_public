#pragma once

#include "core/kernel/eventqueue_types.h"

#include <mutex>
#include <utility/utility.h>

namespace vulkan {
struct SwapchainData;
} // namespace vulkan

constexpr int VIDEO_OUT_EVENT_FLIP   = 0;
constexpr int VIDEO_OUT_EVENT_VBLANK = 1;

class IGraphics;
union SDL_Event;
struct SDL_Window;

typedef void (*SDLEventFunc)(SDL_Event*, void*);

class IVideoOut {
  CLASS_NO_COPY(IVideoOut);
  CLASS_NO_MOVE(IVideoOut);

  protected:
  IVideoOut() = default;

  public:
  virtual ~IVideoOut() = default;

  /**
   * @brief Open a window
   *
   * @param userId
   * @return int internal handle of the window
   */
  virtual int open(int userId) = 0;

  /**
   * @brief Closes the window
   *
   * @param handle
   */
  virtual void close(int handle) = 0;

  /**
   * @brief Set the fps to use
   *
   * @param handle
   * @param rate
   */
  virtual void setFliprate(int handle, int rate) = 0;

  /**
   * @brief Returns the current display's safe area
   *
   * @param area
   */
  virtual void getSafeAreaRatio(float* area) = 0;

  /**
   * @brief Add a VIDEO_OUT_EVENT for the window
   *
   * @param handle
   * @param event
   * @param eq
   * @return int
   */
  virtual int addEvent(int handle, Kernel::EventQueue::KernelEqueueEvent const& event, Kernel::EventQueue::IKernelEqueue_t eq) = 0;

  /**
   * @brief Removes a VIDEO_OUT_EVENT for the window
   *
   * @param handle
   * @param eq
   * @param ident
   */
  virtual void removeEvent(int handle, Kernel::EventQueue::IKernelEqueue_t eq, int const ident) = 0;

  /**
   * @brief Submit flip video buffers to the queue
   *
   * @param index
   * @param flipArg used by the flip event
   */
  virtual void submitFlip(int handle, int index, int64_t flipArg) = 0;

  /**
   * @brief Get the Flip Status
   *
   * @param handle
   * @param status
   */
  virtual void getFlipStatus(int handle, void* status) = 0;

  /**
   * @brief Get the VBlank Status (currently faked by a timer)
   *
   * @param handle
   * @param status
   */
  virtual void getVBlankStatus(int handle, void* status) = 0;

  /**
   * @brief Get the current resolution
   *
   * @param handle
   * @param status
   */
  virtual void getResolution(int handle, void* status) = 0;

  /**
   * @brief Get the number of unhandled flip submits
   *
   * @param handle
   * @return int
   */
  virtual int getPendingFlips(int handle) = 0;

  /**
   * @brief Get the video Buffer Attributes
   *
   * @param attribute
   * @param pixel_format
   * @param tiling_mode
   * @param aspect_ratio
   * @param width
   * @param height
   * @param pitch_in_pixel
   */
  virtual void getBufferAttribute(void* attribute, uint32_t pixel_format, int32_t tiling_mode, int32_t aspect_ratio, uint32_t width, uint32_t height,
                                  uint32_t pitch_in_pixel) = 0;

  /**
   * @brief Registers a video buffer
   *
   * @param handle
   * @param startIndex
   * @param addresses
   * @param numBuffer
   * @param attribute
   * @return int
   */
  virtual int registerBuffers(int handle, int startIndex, void* const* addresses, int numBuffer, const void* attribute) = 0;

  /**
   * @brief Access the graphics interface
   *
   * @return IGraphics*
   */
  virtual IGraphics* getGraphics() = 0;

  /**
   * @brief calls SDL_InitSubSystem() on correct thread context
   *
   * @param flags
   * @return 0: success
   */
  virtual int SDLInit(uint32_t flags) = 0;

  /**
   * @brief Register SDL event listener
   *
   * @param type
   * @param eventFunc
   * @return int
   */
  virtual void SDLEventReg(uint32_t type, SDLEventFunc eventFunc, void* userData) = 0;

  /**
   * @brief Unrgister SDL event listener by type and function pointer
   *
   * @param type
   * @param eventFunc
   * @return int
   */
  virtual bool SDLEventUnreg(uint32_t type, SDLEventFunc eventFunc) = 0;

  /**
   * @brief Unrgister all the SDL event listeners by function pointer
   *
   * @param eventFunc
   * @return int
   */
  virtual bool SDLEventUnreg(SDLEventFunc eventFunc) = 0;

  /**
   * @brief Returns the main SDL Window handle
   *
   * @return SDL_Window*
   */
  virtual SDL_Window* SDLWindow() = 0;

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
};

#ifndef PSOFF_RENDER_VERSION
#define PSOFF_RENDER_VERSION "unknown"
#endif

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif
__APICALL IVideoOut& accessVideoOut();

#undef __APICALL
