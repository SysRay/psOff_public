#include "utility.h"

#include "logging.h"

#include <filesystem>
#include <fstream>
#include <memory>
#include <windows.h>
LOG_DEFINE_MODULE(util);

namespace util {
std::vector<std::string_view> splitString(std::string_view str, char const delim) {
  std::vector<std::string_view> ret;

  size_t start;
  size_t end = 0;
  while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
    end = str.find(delim, start);
    ret.push_back(str.substr(start, end - start));
  }
  return ret;
}

std::unique_ptr<std::ifstream> openFile(std::filesystem::path const& fullpath) {
  LOG_USE_MODULE(util);
  LOG_TRACE(L"(%s)", fullpath.c_str());

  auto file = std::make_unique<std::ifstream>();

  file->open(fullpath, std::ios::binary);
  if (file->fail()) return nullptr;

  return file;
}

bool readFile(std::ifstream* file, wchar_t const* filename, char* data, size_t const size) {
  LOG_USE_MODULE(util);

  if (file->read(data, size).fail()) {
    LOG_ERR(L"Couldn't read file %s", filename);
    return false;
  }
  return true;
}

void setThreadName(std::string_view name) {
  auto r = SetThreadDescription(GetCurrentThread(), std::wstring(name.begin(), name.end()).c_str());
}

void setThreadName(std::string_view name, void* nativeHandle) {
  auto r = SetThreadDescription((HANDLE)nativeHandle, std::wstring(name.begin(), name.end()).c_str());
}
} // namespace util