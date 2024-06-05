#pragma once

namespace events::video {
class IEventHandler;
}

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#else
#define __APICALL __declspec(dllimport)
#endif

namespace events::video::core {
__APICALL void registerSelf(events::video::IEventHandler* obj);
__APICALL void postEvent(int eventId, void const* data);
} // namespace events::video::core

#undef __APICALL