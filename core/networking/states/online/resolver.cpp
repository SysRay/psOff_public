#include "../online.h"
#include "logging.h"
#include "config_emu.h"

#include <Windows.h>
#include <boost/asio.hpp>
#include <unordered_map>

LOG_DEFINE_MODULE(OnlineNetCore_resolv);

using namespace boost::asio;

namespace {
static io_service svc;

struct Resolver {
  char              name[32];
  ip::tcp::resolver res;
  bool              interrupted;
  int               internError;

  Resolver(): res(svc), interrupted(false), name() {}
};

static Resolver* g_resolvers[128] = {nullptr};

class PreMap {
  std::unordered_map<std::string, SceNetInAddr_t> m_mapAddrs;

  public:
  PreMap() {
    LOG_USE_MODULE(OnlineNetCore_resolv);
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

static inline int32_t testResolver(SceNetId rid) {
  if (rid < 0 || rid > 127 || g_resolvers[rid] == nullptr) return Err::Net::ERROR_EINVAL;
  return Ok;
}
} // namespace

SceNetId OnlineNet::resolverCreate(const char* name, int memid, int flags) {
  LOG_USE_MODULE(OnlineNetCore_resolv);

  if (flags != 0) {
    // *sceNetErrnoLoc() = NetErrNo::SCE_NET_EINVAL;
    return -1;
  }

  for (int i = 0; i < 128; i++) {
    if (g_resolvers[i] == nullptr) {
      auto resolv = g_resolvers[i] = new Resolver();
      LOG_TRACE(L"new resolver: %S,%d (memid: %d, flags: %d)", name, i, memid, flags);

      if (auto namelen = ::strlen(name)) {
        if (namelen > sizeof(Resolver::name) - 1) {
          // *sceNetErrnoLoc() = NetErrNo::SCE_NET_ENAMETOOLONG;
          return -1;
        }
        ::strncpy_s(resolv->name, name, namelen);
      }
      return i;
    }
  }

  // *sceNetErrnoLoc() = NetErrNo::SCE_NET_EBADF;
  return -1;
}

int32_t OnlineNet::resolverStartNtoa(SceNetId rid, const char* hostname, SceNetInAddr_t* addr, int timeout, int retries, int flags) {
  LOG_USE_MODULE(OnlineNetCore_resolv);
  if (auto ret = testResolver(rid)) return ret;

  auto resolver         = g_resolvers[rid];
  resolver->internError = 0;

  {
    std::string _hostname(hostname);
    if (auto _raddr = getPreMap().search(_hostname)) {
      *addr = _raddr;
      return Ok;
    }
  }

  ip::tcp::resolver::query query(ip::tcp::v4(), hostname, "0");
  for (int cretr = 0; cretr < retries; ++cretr) {
    if (resolver->interrupted) {
      // *sceNetErrnoLoc() = NetErrNo::SCE_NET_EINTR;
      return Err::Net::ERROR_EFAULT;
    }
    try {
      auto it = resolver->res.resolve(query);
      *addr   = (*it).endpoint().address().to_v4().to_uint();
      return Ok;
    } catch (boost::system::system_error& ex) {
      LOG_ERR(L"%S: failed to resolve %S (%d/%d): %S", __FUNCTION__, hostname, cretr, retries, ex.what());
    }
  }

  // *sceNetErrnoLoc() = NetErrNo::SCE_NET_RESOLVER_ETIMEDOUT;
  return getErr(ErrCode::_ETIMEDOUT);
}

int32_t OnlineNet::resolverStartAton(SceNetId rid, const SceNetInAddr_t* addr, char* hostname, int len, int timeout, int retry, int flags) {
  LOG_USE_MODULE(OnlineNetCore_resolv);
  if (auto ret = testResolver(rid)) return ret;
  LOG_ERR(L"todo %S", __FUNCTION__);

  auto resolver         = g_resolvers[rid];
  resolver->internError = 0;

  {
    auto _hn = getPreMap().reverse_search(*addr);
    if (auto _hnlen = _hn.length()) {
      if (len <= _hnlen) return Err::Net::ERROR_EINVAL;
      _hn.copy(hostname, _hnlen + 1);
      return Ok;
    }
  }

  // *sceNetErrnoLoc() = NetErrNo::SCE_NET_RESOLVER_ETIMEDOUT;
  return getErr(ErrCode::_ETIMEDOUT);
}

int32_t OnlineNet::resolverStartNtoaMultipleRecords(SceNetId rid, const char* hostname, SceNetResolverInfo* info, int timeout, int retries, int flags) {
  LOG_USE_MODULE(OnlineNetCore_resolv);
  if (auto ret = testResolver(rid)) return ret;

  auto resolver         = g_resolvers[rid];
  resolver->internError = 0;

  {
    std::string _hostname(hostname);
    if (auto _raddr = getPreMap().search(_hostname)) {
      info->addrs[0].af      = SCE_NET_AF_INET;
      info->addrs[0].un.addr = _raddr;
      info->records          = 1;
      return Ok;
    }
  }

  ip::tcp::resolver::query query(ip::tcp::v4(), hostname, "0");
  info->records = 0;
  for (int cretr = 0; cretr < retries; ++cretr) {
    // todo: should set sce_net_errno
    if (resolver->interrupted) {
      resolver->internError = NetErrNo::SCE_NET_EINTR;
      return Err::Net::ERROR_EFAULT;
    }
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

  // *sceNetErrnoLoc() = NetErrNo::SCE_NET_RESOLVER_ETIMEDOUT;
  return getErr(ErrCode::_ETIMEDOUT);
}

int32_t OnlineNet::resolverGetError(SceNetId rid, int* result) {
  LOG_USE_MODULE(OnlineNetCore_resolv);
  if (auto ret = testResolver(rid)) return ret;
  LOG_ERR(L"todo %S", __FUNCTION__);
  *result = g_resolvers[rid]->internError;
  return Ok;
}

int32_t OnlineNet::resolverDestroy(SceNetId rid) {
  if (auto ret = testResolver(rid)) return ret;
  delete g_resolvers[rid];
  g_resolvers[rid] = nullptr;
  return Ok;
}

int32_t OnlineNet::resolverAbort(SceNetId rid, int flags) {
  if (auto ret = testResolver(rid)) return ret;
  g_resolvers[rid]->interrupted = true;
  return Ok;
}
