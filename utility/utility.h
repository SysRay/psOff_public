#pragma once

#include <array>
#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

namespace std::filesystem {
class path;
}

namespace util {

template <typename E>
[[nodiscard]] constexpr auto enumValue(E value) {
  return static_cast<std::underlying_type_t<E>>(value);
}

inline uint64_t getAddress(uint32_t const* data) {
  return (uint64_t)data[1] << 32u | data[0];
}

std::vector<std::string_view> splitString(std::string_view str, char const delim);

template <typename V, typename... T>
constexpr auto array_of(T&&... t) -> std::array<V, sizeof...(T)> {
  return {{std::forward<V>(t)...}};
}

inline bool endsWith(std::string const& value, std::string const& ending) {
  if (ending.size() > value.size()) return false;
  return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

enum class InterfaceState {
  NotInit,
  Init,
  Ready,
  Error,
};

class IntStateErrorOnReturn {
  InterfaceState& m_state;
  bool            m_defused = false;

  public:
  IntStateErrorOnReturn(InterfaceState& state): m_state(state) {}

  ~IntStateErrorOnReturn() {
    if (!m_defused) {
      m_state = InterfaceState::Error;
    }
  }

  void defuse() { m_defused = true; }
};

constexpr const char* getBoolStr(bool const flag) {
  return (flag ? "true" : "false");
}

std::unique_ptr<std::ifstream> openFile(std::filesystem::path const& fullpath);
bool                           readFile(std::ifstream* file, wchar_t const* filename, char* data, size_t const size);

/**
 * @brief Get the Memory Protection object
 *
 * @param prot
 * @return std::pair<int, int> protCPU, protGPU
 */
constexpr std::pair<int, int> getMemoryProtection(int prot) {
  int const protCPU = prot & 0x7;
  int const protGPU = (prot >> 4u) & 0x3;
  return std::make_pair(protCPU, protGPU);
}

constexpr std::uint64_t alignUp(uint64_t value, uint64_t alignment) {
  if (alignment == 0) return value;
  return (value + (alignment - 1)) & ~(alignment - 1);
}

constexpr std::uint64_t alignDown(uint64_t value, uint64_t alignment) {
  if (alignment == 0) return value;
  return value & ~(alignment - 1);
}

constexpr bool isPowerOfTwo(int n) {
  if (n == 0) return false;
  return (n & (n - 1)) == 0;
}

void setThreadName(std::string_view name);
void setThreadName(std::string_view name, void* nativeHandle);

int getPageSize(void);
} // namespace util

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define SYSV_ABI __attribute__((sysv_abi))

#define CLASS_NO_COPY(name)                                                                                                                                    \
  name(const name&)            = delete;                                                                                                                       \
  name& operator=(const name&) = delete;

#define CLASS_NO_MOVE(name)                                                                                                                                    \
  name(name&&) noexcept            = delete;                                                                                                                   \
  name& operator=(name&&) noexcept = delete