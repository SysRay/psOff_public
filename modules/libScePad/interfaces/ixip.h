#pragma once

#include "../icontroller.h"

#include <memory>

std::unique_ptr<IController> createController_xinput(uint32_t);
