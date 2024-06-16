#pragma once

#include <stdint.h>
#include <string>

namespace events::system_cross {
class IEventHandler;
}

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#else
#define __APICALL __declspec(dllimport)
#endif

namespace events::system_cross::core {
__APICALL void registerSelf(events::system_cross::IEventHandler* obj);
__APICALL void postEvent(std::string const& stream);
__APICALL void initChild(); // Receiver, that wants to postEvents

} // namespace events::system_cross::core

#undef __APICALL
