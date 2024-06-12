#pragma once

#include <stdint.h>
#include <string>

namespace events::system {
class IEventHandler;
}

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#else
#define __APICALL __declspec(dllimport)
#endif

namespace events::system::core {
__APICALL void registerSelf(events::system::IEventHandler* obj);
__APICALL void postEvent(std::string const& stream);
__APICALL void initChild(); // Receiver, that wants to postEvents

} // namespace events::system::core

#undef __APICALL
