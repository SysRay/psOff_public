#pragma once

#include "../ifile.h"

#include <memory>

enum SceFileOutChannel {
  SCE_ITYPEOUT_ERROR,
  SCE_ITYPEOUT_TRACE,
};

std::unique_ptr<IFile> createType_out(SceFileOutChannel ch);
