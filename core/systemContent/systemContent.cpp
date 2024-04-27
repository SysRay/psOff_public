#define __APICALL_EXTERN
#include "systemContent.h"
#undef __APICALL_EXTERN

#include "logging.h"
#include "utility/utility.h"

#include <filesystem>
#include <fstream>
#include <unordered_map>

LOG_DEFINE_MODULE(SYSTEMCONTENT);

namespace {
struct ParamInfo {
  uint16_t type;
  uint32_t size1;
  uint32_t size2;
  uint16_t nameOffset;
  uint32_t valueOffset;
};

std::unordered_map<std::string, std::pair<uint16_t, std::vector<uint8_t>>> loadSFO(std::filesystem::path const& root) {
  LOG_USE_MODULE(SYSTEMCONTENT);

  auto file = util::openFile(root / L"param.sfo");
  if (!file) {
    LOG_ERR(L"No param.sfo");
    return {};
  }

  // Check Magic
  {
    std::array<uint8_t, 8> buffer;
    if (!util::readFile(file.get(), L"param.sfo", (char*)buffer.data(), buffer.size())) {
      LOG_ERR(L"Couldn't read .sfo magic");
      return {};
    }

    uint32_t const magic1 = *(uint32_t const*)&buffer[0];
    uint32_t const magic2 = *(uint32_t const*)&buffer[4];

    if (magic1 != 0x46535000 && magic2 != 0x00000101) {
      LOG_ERR(L".sfo magic error| 0x%08x 0x%08x", magic1, magic2);
      return {};
    }
  }
  // - Magic

  auto [nameOffset, valueOffset, numParams] = [&]() -> std::tuple<uint32_t, uint32_t, uint32_t> {
    std::array<uint8_t, 12> buffer;
    if (!util::readFile(file.get(), L"param.sfo", (char*)buffer.data(), buffer.size())) {
      LOG_ERR(L"Couldn't read .sfo Header");
      return {};
    }
    return std::make_tuple(*(uint32_t const*)&buffer[0], *(uint32_t const*)&buffer[4], *(uint32_t const*)&buffer[8]);
  }();

  // Read Params
  std::vector<ParamInfo> params(numParams);
  {
    std::array<uint8_t, 16> buffer;
    for (uint16_t n = 0; n < numParams; ++n) {
      if (!util::readFile(file.get(), L"param.sfo", (char*)buffer.data(), buffer.size())) {
        LOG_ERR(L"Couldn't read .sfo paramInfo %llu", n);
        return {};
      }

      params[n] = ParamInfo {.type        = *(uint16_t const*)&buffer[2],
                             .size1       = *(uint32_t const*)&buffer[4],
                             .size2       = *(uint32_t const*)&buffer[8],
                             .nameOffset  = *(uint16_t const*)&buffer[0],
                             .valueOffset = *(uint32_t const*)&buffer[12]};

      if (params[n].type != 0x0204 && params[n].type != 0x0404) {
        LOG_ERR(L"unknown .sfo param %u type:%u", n, params[n].type);
        return {};
      }
    }
  }
  // - params

  // read nameList
  const uint32_t       nameListSize = valueOffset - nameOffset;
  std::vector<uint8_t> nameList(nameListSize);
  file->seekg(nameOffset);
  if (!util::readFile(file.get(), L"param.sfo", (char*)nameList.data(), nameList.size())) {
    LOG_ERR(L"Couldn't read .sfo nameList");
    return {};
  }

  std::unordered_map<std::string, std::pair<uint16_t, std::vector<uint8_t>>> paramList;
  for (uint16_t n = 0; n < numParams; ++n) {
    auto const name = std::string((char*)&nameList[params[n].nameOffset]);

    file->seekg(valueOffset + params[n].valueOffset, std::ios_base::beg);
    std::vector<uint8_t> paramValue(params[n].size1);

    if (!util::readFile(file.get(), L"param.sfo", (char*)paramValue.data(), paramValue.size())) {
      LOG_ERR(L"Couldn't read .sfo param %u value");
      return {};
    }

    if (params[n].type == 0x0404)
      LOG_DEBUG(L"Read .sfo param[%u] %S| size:%u value(uint):%u", n, name.data(), paramValue.size(), *(uint32_t*)paramValue.data());
    else if (params[n].type == 0x0204)
      LOG_DEBUG(L"Read .sfo param[%u] %S| size:%u value(string):%S", n, name.data(), paramValue.size(), (char*)paramValue.data());
    paramList[std::move(name)] = std::make_pair(params[n].type, std::move(paramValue));
  }
  return paramList;
}
} // namespace

class SystemContent: public ISystemContent {
  private:
  std::unordered_map<std::string, std::pair<uint16_t, std::vector<uint8_t>>> m_sfoParams;

  public:
  void init(std::filesystem::path const& path) final;

  std::optional<uint32_t> getInt(std::string_view name) final {
    if (auto it = m_sfoParams.find(name.data()); it != m_sfoParams.end()) {
      if (it->second.first != 0x0404) return {};
      return {*(uint32_t*)it->second.second.data()};
    }
    return {};
  }

  std::optional<std::string_view> getString(std::string_view name) {
    if (auto it = m_sfoParams.find(name.data()); it != m_sfoParams.end()) {
      if (it->second.first != 0x0204) return {};
      return {(const char*)it->second.second.data()};
    }
    return {};
  }
};

void SystemContent::init(std::filesystem::path const& root) {
  if (!m_sfoParams.empty()) return;
  m_sfoParams = loadSFO(root);
}

ISystemContent& accessSystemContent() {
  static SystemContent inst;
  return inst;
}