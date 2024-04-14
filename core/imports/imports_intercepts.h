#pragma once

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif

/**
 * @brief Called before library setup
 *
 * @return __APICALL
 */
__APICALL void core_initIntercepts();
#undef __APICALL