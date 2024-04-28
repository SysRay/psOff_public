#pragma once
#include <optional>
#include <string_view>

namespace std::filesystem {
class path;
}

class ISystemContent {
  public:
  /**
   * @brief Checks if the profided path contain "param.sfo" and reads it
   *
   * @param path
   */
  virtual void init(std::filesystem::path const& path) = 0;

  /**
   * @brief Get an int value from the sfo
   *
   * @param name
   * @return std::optional<uint32_t>
   */
  virtual std::optional<uint32_t> getInt(std::string_view name) const = 0;

  /**
   * @brief Get an string value from the sfo
   *
   * @param name
   * @return std::optional<std::string_view>
   */
  virtual std::optional<std::string_view> getString(std::string_view name) const = 0;
};

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif
__APICALL ISystemContent& accessSystemContent();
#undef __APICALL