#pragma once

#include "../ifile.h"

#include <memory>

enum SceFileOutChannel {
  SCE_TYPEOUT_ERROR,
  SCE_TYPEOUT_DEBUG,
};

std::unique_ptr<IFile> createType_out(SceFileOutChannel ch);
