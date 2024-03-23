#pragma once

#include "../cconfig.h"
#include "../icontroller.h"

#include <memory>

std::unique_ptr<IController> createController_xinput(ControllerConfig*, uint32_t);
