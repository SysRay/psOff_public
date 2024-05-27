#pragma once

#include "../ifile.h"
#include "core/runtime/program.h"

std::unique_ptr<IFile> createType_lib(std::unique_ptr<Program>& prog);
