#pragma once

#include "../ifile.h"
#include "core/kernel/filesystem.h"

#include <filesystem>
#include <memory>

std::unique_ptr<IFile> createType_file(std::filesystem::path path, filesystem::SceOpen mode);