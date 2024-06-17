#pragma once
#include "types.h"

namespace events::system_circle {

void initChild();

void postEventLoadExec(LoadArgs const& data);
void postEventSetArguments(SetArg const& data);
void postEventRunExec();
} // namespace events::system_circle
