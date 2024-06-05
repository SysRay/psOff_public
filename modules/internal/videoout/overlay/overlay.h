#pragma once
#include "../imageHandler.h"
#include "../vulkan/vulkanSetup.h"
#include "../vulkan/vulkanTypes.h"
#include "utility/utility.h"

#include <SDL2/SDL.h>
#include <memory>

struct ImageData;

class IOverlayHandler {
  CLASS_NO_COPY(IOverlayHandler);

  protected:
  IOverlayHandler() = default;

  public:
  virtual ~IOverlayHandler() = default;

  virtual void stop() = 0;

  virtual void submit(ImageData const& imageData) = 0;

  virtual void processEvent(SDL_Event const* event) = 0;
};

std::unique_ptr<IOverlayHandler> createOverlay(std::shared_ptr<vulkan::DeviceInfo>& deviceInfo, std::shared_ptr<IImageHandler>& imageHandler,
                                               SDL_Window* window, vulkan::QueueInfo* queue, VkFormat displayFormat);
