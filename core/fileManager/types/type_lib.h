#pragma once

#include "../ifile.h"

struct Program;
std::unique_ptr<IFile> createType_lib(Program* prog);
