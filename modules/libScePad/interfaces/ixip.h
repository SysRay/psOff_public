#pragma once

#include "../icontroller.h"
#include "../cconfig.h"

#include <memory>

std::unique_ptr<IController> createController_xinput(ControllerConfig*, uint32_t);
