#pragma once

#include <cstdint>

namespace Err {
namespace CommonDialog {
constexpr int32_t NOT_SYSTEM_INITIALIZED     = -2135425023; /* 0x80B80001 */
constexpr int32_t ALREADY_SYSTEM_INITIALIZED = -2135425022; /* 0x80B80002 */
constexpr int32_t NOT_INITIALIZED            = -2135425021; /* 0x80B80003 */
constexpr int32_t ALREADY_INITIALIZED        = -2135425020; /* 0x80B80004 */
constexpr int32_t NOT_FINISHED               = -2135425019; /* 0x80B80005 */
constexpr int32_t INVALID_STATE              = -2135425018; /* 0x80B80006 */
constexpr int32_t RESULT_NONE                = -2135425017; /* 0x80B80007 */
constexpr int32_t BUSY                       = -2135425016; /* 0x80B80008 */
constexpr int32_t OUT_OF_MEMORY              = -2135425015; /* 0x80B80009 */
constexpr int32_t PARAM_INVALID              = -2135425014; /* 0x80B8000A */
constexpr int32_t NOT_RUNNING                = -2135425013; /* 0x80B8000B */
constexpr int32_t ALREADY_CLOSE              = -2135425012; /* 0x80B8000C */
constexpr int32_t ARG_NULL                   = -2135425011; /* 0x80B8000D */
constexpr int32_t UNEXPECTED_FATAL           = -2135425010; /* 0x80B8000E */
constexpr int32_t NOT_SUPPORTED              = -2135425009; /* 0x80B8000F */
constexpr int32_t INHIBIT_SHAREPLAY_CLIENT   = -2135425008; /* 0x80B80010 */
} // namespace CommonDialog
} // namespace Err
