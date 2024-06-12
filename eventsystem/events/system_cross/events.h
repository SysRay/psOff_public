#pragma once
#include "types.h"

namespace events::system_cross {

void initChild();

void postEventLoadExec(LoadArgs const& data);
void postEventSetArguments(SetArg const& data);
void postEventRunExec();
} // namespace events::system_cross
