#pragma once

#include "../ifile.h"

#include <filesystem>
#include <memory>

std::unique_ptr<IFile> createType_file(std::filesystem::path path, std::ios_base::openmode mode);