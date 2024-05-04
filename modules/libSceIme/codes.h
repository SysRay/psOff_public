#pragma once
#include <stdint.h>

namespace Err {
namespace Ime {
constexpr int32_t BUSY                         = -2135162879;
constexpr int32_t NOT_OPENED                   = -2135162878;
constexpr int32_t NO_MEMORY                    = -2135162877;
constexpr int32_t CONNECTION_FAILED            = -2135162876;
constexpr int32_t TOO_MANY_REQUESTS            = -2135162875;
constexpr int32_t INVALID_TEXT                 = -2135162874;
constexpr int32_t EVENT_OVERFLOW               = -2135162873;
constexpr int32_t NOT_ACTIVE                   = -2135162872;
constexpr int32_t IME_SUSPENDING               = -2135162871;
constexpr int32_t DEVICE_IN_USE                = -2135162870;
constexpr int32_t INVALID_USER_ID              = -2135162864;
constexpr int32_t INVALID_TYPE                 = -2135162863;
constexpr int32_t INVALID_SUPPORTED_LANGUAGES  = -2135162862;
constexpr int32_t INVALID_ENTER_LABEL          = -2135162861;
constexpr int32_t INVALID_INPUT_METHOD         = -2135162860;
constexpr int32_t INVALID_OPTION               = -2135162859;
constexpr int32_t INVALID_MAX_TEXT_LENGTH      = -2135162858;
constexpr int32_t INVALID_INPUT_TEXT_BUFFER    = -2135162857;
constexpr int32_t INVALID_POSX                 = -2135162856;
constexpr int32_t INVALID_POSY                 = -2135162855;
constexpr int32_t INVALID_HORIZONTAL_ALIGNMENT = -2135162854;
constexpr int32_t INVALID_VERTICAL_ALIGNMENT   = -2135162853;
constexpr int32_t INVALID_EXTENDED             = -2135162852;
constexpr int32_t INVALID_KEYBOARD_TYPE        = -2135162851;
constexpr int32_t INVALID_WORK                 = -2135162848;
constexpr int32_t INVALID_ARG                  = -2135162847;
constexpr int32_t INVALID_HANDLER              = -2135162846;
constexpr int32_t NO_RESOURCE_ID               = -2135162845;
constexpr int32_t INVALID_MODE                 = -2135162844;
constexpr int32_t INVALID_PARAM                = -2135162832;
constexpr int32_t INVALID_ADDRESS              = -2135162831;
constexpr int32_t INVALID_RESERVED             = -2135162830;
constexpr int32_t INVALID_TIMING               = -2135162829;
constexpr int32_t INTERNAL                     = -2135162625;
} // namespace Ime
} // namespace Err

namespace Ime {
constexpr uint32_t OSK_DISPLAY_SIZE_WIDTH          = 1920;
constexpr uint32_t OSK_DISPLAY_SIZE_HEIGHT         = 1080;
constexpr uint32_t OSK_OVER_2K_DISPLAY_SIZE_WIDTH  = 3840;
constexpr uint32_t OSK_OVER_2K_DISPLAY_SIZE_HEIGHT = 2160;
constexpr uint32_t MAX_PREEDIT_LENGTH              = 30;
constexpr uint32_t MAX_EXPANDED_PREEDIT_LENGTH     = 120;
constexpr uint32_t MAX_TEXT_LENGTH                 = 2048;
constexpr uint32_t MAX_TEXT_AREA                   = 4;
constexpr uint32_t MAX_CANDIDATE_WORD_LENGTH       = 55;
constexpr uint32_t MAX_CANDIDATE_LIST_SIZE         = 100;
constexpr uint32_t WORK_BUFFER_SIZE                = 20 * 1024;
constexpr uint32_t KEYBOARD_MAX_NUMBER             = 5;

namespace Option {
constexpr uint32_t DEFAULT                = 0x00;
constexpr uint32_t REPEAT                 = 0x01;
constexpr uint32_t REPEAT_EACH_KEY        = 0x02;
constexpr uint32_t ADD_OSK                = 0x04;
constexpr uint32_t EFFECTIVE_WITH_IME     = 0x08;
constexpr uint32_t DISABLE_RESUME         = 0x10;
constexpr uint32_t CAPSLOCK_WITHOUT_SHIFT = 0x20;
} // namespace Option
} // namespace Ime
