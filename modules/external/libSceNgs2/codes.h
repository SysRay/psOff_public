#pragma once
#include <stdint.h>

namespace Err {
namespace Ngs2 {
constexpr int32_t INVALID_WAVEFORM_DATA     = -2142632952;
constexpr int32_t INVALID_BUFFER_ADDRESS    = -2142633465;
constexpr int32_t INVALID_BUFFER_SIZE       = -2142633466;
constexpr int32_t INVALID_SYSTEM_HANDLE     = -2142633424;
constexpr int32_t INVALID_VOICE_HANDLE      = -2142633216;
constexpr int32_t INVALID_RACK_HANDLE       = -2142633375;
constexpr int32_t INVALID_VOICE_INDEX       = -2142633214;
constexpr int32_t INVALID_BUFFER_ALLOCATOR  = -2142633462;
constexpr int32_t INVALID_WAVEFORM_TYPE     = -2142632958;
constexpr int32_t INVALID_OUT_SIZE          = -2142633900;
constexpr int32_t INVALID_OPTION_SIZE       = -2142633855;
constexpr int32_t INVALID_OUT_ADDRESS       = -2142633901;
constexpr int32_t INVALID_NUM_CHANNELS      = -2142633902;
constexpr int32_t INVALID_MAX_GRAIN_SAMPLES = -2142633904;
constexpr int32_t FAIL                      = -2142633983;
} // namespace Ngs2
} // namespace Err

constexpr int32_t SCE_NGS2_MAX_VOICE_CHANNELS = 8;
constexpr int32_t SCE_NGS2_MAX_MATRIX_LEVELS  = 64;
constexpr int32_t SCE_NGS2_RACK_NAME_LENGTH   = 16;
constexpr int32_t SCE_NGS2_SYSTEM_NAME_LENGTH = 16;

constexpr uint16_t SCE_NGS2_RACK_ID_VOICE     = 0x0000;
constexpr uint16_t SCE_NGS2_RACK_ID_SAMPLER   = 0x1000;
constexpr uint16_t SCE_NGS2_RACK_ID_SUBMIXER  = 0x2000;
constexpr uint16_t SCE_NGS2_RACK_ID_MASTERING = 0x3000;

constexpr uint16_t SCE_NGS2_RACK_ID_REVERB = 0x2001;
constexpr uint16_t SCE_NGS2_RACK_ID_EQ     = 0x2002;

constexpr uint16_t SCE_NGS2_RACK_ID_CUSTOM_VOICE     = 0x4000;
constexpr uint16_t SCE_NGS2_RACK_ID_CUSTOM_SAMPLER   = 0x4001;
constexpr uint16_t SCE_NGS2_RACK_ID_CUSTOM_SUBMIXER  = 0x4002;
constexpr uint16_t SCE_NGS2_RACK_ID_CUSTOM_MASTERING = 0x4003;