#pragma once
#include <stdint.h>

namespace Err {
constexpr int32_t FATAL           = -2144993024;
constexpr int32_t INVALID_HANDLE  = -2144993023;
constexpr int32_t INVALID_SIZE    = -2144993022;
constexpr int32_t INVALID_FREQ    = -2144993021;
constexpr int32_t INVALID_TYPE    = -2144993020;
constexpr int32_t INVALID_POINTER = -2144993019;
constexpr int32_t INVALID_PARAM   = -2144993018;
constexpr int32_t PORT_FULL       = -2144993017;
constexpr int32_t OUT_OF_MEMORY   = -2144993016;
constexpr int32_t NOT_OPENED      = -2144993015;
constexpr int32_t BUSY            = -2144993014;
constexpr int32_t SYSTEM_MEMORY   = -2144993013;
constexpr int32_t SYSTEM_IPC      = -2144993012;
} // namespace Err

constexpr uint32_t SCE_AUDIO_IN_FREQ_DEFAULT   = 16000;
constexpr uint32_t SCE_AUDIO_IN_GRAIN_DEFAULT  = 256;
constexpr uint32_t SCE_AUDIO_IN_FREQ_HQ        = 48000;
constexpr uint32_t SCE_AUDIO_IN_TIME_SAMPLE_HQ = 128;
constexpr uint32_t SCE_AUDIO_IN_GRAIN_MAX_HQ   = (SCE_AUDIO_IN_TIME_SAMPLE_HQ * 3);
