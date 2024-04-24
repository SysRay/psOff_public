#pragma once
#include <stdint.h>

namespace Err {
namespace AudioOut {
constexpr int32_t NOT_OPENED          = -2144993279;
constexpr int32_t BUSY                = -2144993278;
constexpr int32_t INVALID_PORT        = -2144993277;
constexpr int32_t INVALID_POINTER     = -2144993276;
constexpr int32_t PORT_FULL           = -2144993275;
constexpr int32_t INVALID_SIZE        = -2144993274;
constexpr int32_t INVALID_FORMAT      = -2144993273;
constexpr int32_t INVALID_SAMPLE_FREQ = -2144993272;
constexpr int32_t INVALID_VOLUME      = -2144993271;
constexpr int32_t INVALID_PORT_TYPE   = -2144993270;
constexpr int32_t INVALID_CONF_TYPE   = -2144993268;
constexpr int32_t OUT_OF_MEMORY       = -2144993267;
constexpr int32_t ALREADY_INIT        = -2144993266;
constexpr int32_t NOT_INIT            = -2144993265;
constexpr int32_t MEMORY              = -2144993264;
constexpr int32_t SYSTEM_RESOURCE     = -2144993263;
constexpr int32_t TRANS_EVENT         = -2144993262;
constexpr int32_t INVALID_FLAG        = -2144993261;
constexpr int32_t INVALID_MIXLEVEL    = -2144993260;
constexpr int32_t INVALID_ARG         = -2144993259;
constexpr int32_t INVALID_PARAM       = -2144993258;
} // namespace AudioOut
} // namespace Err

constexpr int PORT_OUT_MAX         = 6;
constexpr int PORT_IN_MAX          = 8;
constexpr int SCE_AUDIO_VOLUME_0DB = (1 << 15);
