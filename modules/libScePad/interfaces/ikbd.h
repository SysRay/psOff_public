#pragma once

#include "../icontroller.h"

#include <memory>

std::unique_ptr<IController> createController_keyboard(uint32_t);
