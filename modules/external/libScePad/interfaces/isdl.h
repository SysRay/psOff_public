#pragma once

#include "../icontroller.h"

#include <memory>

std::unique_ptr<IController> createController_sdl(uint32_t);
