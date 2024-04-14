#include <string>

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif

// All communication  strictly goes through the core library. (May change later)

/**
 * @brief Get the Emulator Version
 *
 * @return __APICALL
 */
__APICALL std::string_view getEmulatorVersion();
#undef __APICALL
