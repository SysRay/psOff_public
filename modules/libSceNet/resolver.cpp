#include "common.h"
#include "config_emu.h"
#include "logging.h"
#include "resolverTypes.h"
#include "socketTypes.h"

#include <Windows.h>
#include <boost/asio.hpp>
#include <unordered_map>

using namespace boost::asio;

LOG_DEFINE_MODULE(libSceNet);

namespace {
#define TEST_RESOLVER                                                                                                                                          \
  if (rid < 0 || rid > 128 || g_resolvers[rid] == nullptr) return Err::ERROR_EINVAL

static io_service svc;

struct Resolver {
  char              name[32];
  ip::tcp::resolver res;
  bool              interrupted;

  Resolver(): res(svc), interrupted(false), name() {}
};

static Resolver* g_resolvers[128] = {nullptr};

class PreMap {
  std::unordered_map<std::string, SceNetInAddr_t> m_mapAddrs;

  public:
  PreMap() {
    LOG_USE_MODULE(libSceNet);
    auto [lock, jData] = accessConfig()->accessModule(ConfigModFlag::RESOLVE);

    if ((*jData).is_object()) {
      for (auto [key, value]: (*jData).items()) {
        try {
          SceNetInAddr_t addr;
          if (value.is_number_integer()) {
            value.get_to(addr);
          } else {
            std::string saddr;
            value.get_to(saddr);
            addr = ip::address_v4::from_string(saddr).to_ulong();
          }

          m_mapAddrs.insert(std::pair<std::string, SceNetInAddr_t>(key, addr));
        } catch (json::exception& ex) {
          LOG_ERR(L"Invalid resolve.json field: %S", key.c_str());
        }
      }
      return;
    }

    LOG_ERR(L"Something wrong with your resolve.json!");
  }

  SceNetInAddr_t search(const std::string& hostname) {
    auto it = m_mapAddrs.find(hostname);
    if (it != m_mapAddrs.end()) {
      return it->second;
    }

    return 0x00000000;
  }

  std::string_view const reverse_search(const SceNetInAddr_t addr) {
    auto it = std::find_if(m_mapAddrs.begin(), m_mapAddrs.end(), [&addr](auto&& p) { return p.second == addr; });
    if (it == m_mapAddrs.end()) return "";
    return it->first;
  }
};

PreMap& getPreMap() {
  static PreMap imp;
  return imp;
}
} // namespace

extern "C" {
EXPORT SYSV_ABI SceNetId sceNetResolverCreate(const char* name, int memid, int flags) {
  LOG_USE_MODULE(libSceNet);
  LOG_ERR(L"todo %S", __FUNCTION__);
  static size_t count  = 0;
  g_resolvers[++count] = new Resolver();
  if (auto namelen = ::strnlen_s(name, 31)) ::strncpy_s(g_resolvers[count]->name, name, namelen);
  return count;
}

EXPORT SYSV_ABI int sceNetResolverStartNtoa(SceNetId rid, const char* hostname, SceNetInAddr_t* addr, int timeout, int retries, int flags) {
  LOG_USE_MODULE(libSceNet);
  TEST_RESOLVER;

  std::string _hostname(hostname);
  if (auto _raddr = getPreMap().search(_hostname)) {
    *addr = _raddr;
    return Ok;
  }

  auto                     resolver = g_resolvers[rid];
  ip::tcp::resolver::query query(ip::tcp::v4(), hostname, "0");
  for (int cretr = 0; cretr < retries; ++cretr) {
    // todo: should set sce_net_errno
    if (resolver->interrupted) return Err::ERROR_EFAULT;
    try {
      auto it = resolver->res.resolve(query);
      *addr   = (*it).endpoint().address().to_v4().to_uint();
      return Ok;
    } catch (boost::system::system_error& ex) {
      LOG_ERR(L"%S: failed to resolve %S (%d/%d): %S", __FUNCTION__, hostname, cretr, retries, ex.what());
    }
  }

  return getErr(ErrCode::_ETIMEDOUT);
}

EXPORT SYSV_ABI int sceNetResolverStartAton(SceNetId rid, const SceNetInAddr_t* addr, char* hostname, int len, int timeout, int retry, int flags) {
  TEST_RESOLVER;
  LOG_USE_MODULE(libSceNet);
  LOG_ERR(L"todo %S", __FUNCTION__);

  auto _hn = getPreMap().reverse_search(*addr);
  if (auto _hnlen = _hn.length()) {
    if (len <= _hnlen) return Err::ERROR_EINVAL;
    _hn.copy(hostname, _hnlen + 1);
    return Ok;
  }

  return getErr(ErrCode::_ETIMEDOUT);
}

EXPORT SYSV_ABI int sceNetResolverStartNtoaMultipleRecords(SceNetId rid, const char* hostname, SceNetResolverInfo* info, int timeout, int retries, int flags) {
  LOG_USE_MODULE(libSceNet);
  TEST_RESOLVER;

  std::string _hostname(hostname);
  if (auto _raddr = getPreMap().search(_hostname)) {
    info->addrs[0].af      = SCE_NET_AF_INET;
    info->addrs[0].un.addr = _raddr;
    info->records          = 1;
    return Ok;
  }

  auto                     resolver = g_resolvers[rid];
  ip::tcp::resolver::query query(ip::tcp::v4(), hostname, "0");
  info->records = 0;
  for (int cretr = 0; cretr < retries; ++cretr) {
    // todo: should set sce_net_errno
    if (resolver->interrupted) return Err::ERROR_EFAULT;
    try {
      for (auto addr: resolver->res.resolve(query)) {
        auto& iaddr   = info->addrs[info->records++];
        iaddr.af      = SCE_NET_AF_INET;
        iaddr.un.addr = addr.endpoint().address().to_v4().to_uint();
        if (info->records == SCE_NET_RESOLVER_MULTIPLE_RECORDS_MAX) break;
      }
      info->records -= 1; // We should decrease value by 1 to get the actual records count
      info->dns4records = info->records;
      return Ok;
    } catch (boost::system::system_error& ex) {
      LOG_ERR(L"%S: failed to resolve %S (%d/%d): %S", __FUNCTION__, hostname, cretr, retries, ex.what());
    }
  }

  return getErr(ErrCode::_ETIMEDOUT);
}

EXPORT SYSV_ABI int sceNetResolverGetError(SceNetId rid, int* result) {
  TEST_RESOLVER;
  LOG_USE_MODULE(libSceNet);
  LOG_ERR(L"todo %S", __FUNCTION__);
  *result = (int)ErrCode::_ETIMEDOUT;
  return Ok;
}

EXPORT SYSV_ABI int sceNetResolverDestroy(SceNetId rid) {
  TEST_RESOLVER;
  delete g_resolvers[rid];
  g_resolvers[rid] = nullptr;
  return Ok;
}

EXPORT SYSV_ABI int sceNetResolverAbort(SceNetId rid, int flags) {
  TEST_RESOLVER;
  g_resolvers[rid]->interrupted = true;
  return Ok;
}
}
