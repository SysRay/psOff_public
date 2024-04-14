#pragma once
#include <stdint.h>

namespace Err {
constexpr int32_t INVALID_WAVEFORM_DATA     = -2142632952;
constexpr int32_t INVALID_BUFFER_ADDRESS    = -2142633465;
constexpr int32_t INVALID_BUFFER_SIZE       = -2142633466;
constexpr int32_t INVALID_SYSTEM_HANDLE     = -2142633424;
constexpr int32_t INVALID_BUFFER_ALLOCATOR  = -2142633462;
constexpr int32_t INVALID_WAVEFORM_TYPE     = -2142632958;
constexpr int32_t INVALID_OUT_SIZE          = -2142633900;
constexpr int32_t INVALID_OPTION_SIZE       = -2142633855;
constexpr int32_t INVALID_OUT_ADDRESS       = -2142633901;
constexpr int32_t INVALID_NUM_CHANNELS      = -2142633902;
constexpr int32_t INVALID_MAX_GRAIN_SAMPLES = -2142633904;
constexpr int32_t FAIL                      = -2142633983;
} // namespace Err

constexpr int32_t SCE_NGS2_RACK_NAME_LENGTH = 16;

constexpr int32_t SCE_NGS2_SAMPLER_VOICE_ADD_WAVEFORM_BLOCKS = 0x10000001;
