#pragma once

#include "exports/runtimeExport.h"

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif
__APICALL IRuntimeExport* accessRuntimeExport();

#undef __APICALL