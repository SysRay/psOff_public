#pragma once
#include "codes.h"
#include "sceTypes.h"

enum class ScePadPortType {
  STANDARD       = 0,
  SPECIAL        = 2,
  REMOTE_CONTROL = 16,
};

enum class ScePadButtonDataOffset : uint32_t {
  L3 = 1,
  R3,
  OPTIONS,
  UP,
  RIGHT,
  DOWN,
  LEFT,
  L2,
  R2,
  L1,
  R1,
  TRIANGLE,
  CIRCLE,
  CROSS,
  SQUARE,
  TOUCH_PAD = 20,
  INTERCEPTED,
};

enum class ScePadDeviceClass : int32_t {
  INVALID        = -1,
  STANDARD       = 0,
  GUITAR         = 1,
  DRUM           = 2,
  DJ_TURNTABLE   = 3,
  DANCEMAT       = 4,
  NAVIGATION     = 5,
  STEERING_WHEEL = 6,
  STICK          = 7,
  FLIGHT_STICK   = 8,
  GUN            = 9,
};

enum class ScePadConnectionType {
  LOCAL             = 0,
  REMOTE_VITA       = 1,
  REMOTE_DUALSHOCK4 = 2,
};

enum class SceSteeringWheelGear {
  GEAR_1ST     = 0x01,
  GEAR_2ND     = 0x02,
  GEAR_3RD     = 0x04,
  GEAR_4TH     = 0x08,
  GEAR_5TH     = 0x10,
  GEAR_6TH     = 0x20,
  GEAR_7TH     = 0x40,
  REVERSE_GEAR = 0x80,
};

enum class SceGuitarFret {
  FRET_1 = 0x01,
  FRET_2 = 0x02,
  FRET_3 = 0x04,
  FRET_4 = 0x08,
  FRET_5 = 0x10,
};

enum class SceSteeringWheelCapability {
  HAND_BRAKE   = 0x02,
  GEAR_SHIFT   = 0x04,
  THREE_PEDALS = 0x08,
};

enum class SceGuitarCapability {
  SELECTOR_SWITCH = 0x01,
  TILT            = 0x02,
  WHAMMY_BAR      = 0x04,
};

enum class SceDrumCapability {
  FOUR_DRUM_PADS   = 0x01,
  CYMBAL_HI_HAT    = 0x02,
  CYMBAL_RIDE      = 0x04,
  CYMBAL_CRASH     = 0x08,
  EXTRA_FOOT_PEDAL = 0x10,
};

enum class SceFlightCapability {
  FORCE_FEEDBACK     = 0x01,
  ANTENNA_KNOB       = 0x02,
  RANGE_KNOB         = 0x04,
  RUDDER_PEDAL       = 0x08,
  RADAR_CURSOR_STICK = 0x10,
};

struct ScePadAnalogStick {
  uint8_t x;
  uint8_t y;
};

struct ScePadAnalogButtons {
  uint8_t l2;
  uint8_t r2;
  uint8_t padding[2];
};

struct ScePadTouch {
  uint16_t x;
  uint16_t y;
  uint8_t  id;
  uint8_t  reserve[3];
};

struct ScePadTouchData {
  uint8_t     touchNum = 0;
  uint8_t     reserve[3];
  uint32_t    reserve1;
  ScePadTouch touch[SCE_PAD_MAX_TOUCH_NUM];
};

struct ScePadExtensionUnitData {
  uint32_t extensionUnitId;
  uint8_t  reserve[1];
  uint8_t  dataLength;
  uint8_t  data[10];
};

struct ScePadData {
  uint32_t            buttons       = 0;
  ScePadAnalogStick   leftStick     = {.x = 127, .y = 127};
  ScePadAnalogStick   rightStick    = {.x = 127, .y = 127};
  ScePadAnalogButtons analogButtons = {.l2 = 0, .r2 = 0};
  SceFQuaternion      orientation {0.f, 0.f, 0.f, 1.f};

  SceFVector3             acceleration {0.f, 0.f, 0.f};
  SceFVector3             angularVelocity {0.f, 0.f, 0.f};
  ScePadTouchData         touchData;
  bool                    connected = false;
  uint64_t                timestamp = 0;
  ScePadExtensionUnitData extensionUnitData;
  uint8_t                 connectedCount = 0;
  uint8_t                 reserve[2];
  uint8_t                 deviceUniqueDataLen = 0;
  uint8_t                 deviceUniqueData[SCE_PAD_MAX_DEVICE_UNIQUE_DATA_SIZE];
};

struct ScePadVibrationParam {
  uint8_t largeMotor;
  uint8_t smallMotor;
};

struct ScePadColor {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t reserve[1];
};

struct ScePadTouchPadInformation {
  float pixelDensity;

  struct {
    uint16_t x;
    uint16_t y;
  } resolution;
};

struct ScePadStickInformation {
  uint8_t deadZoneLeft;
  uint8_t deadZoneRight;
};

struct ScePadControllerInformation {
  ScePadTouchPadInformation touchPadInfo;
  ScePadStickInformation    stickInfo;
  uint8_t                   connectionType;
  uint8_t                   connectedCount;
  bool                      connected;
  ScePadDeviceClass         deviceClass;
  uint8_t                   reserve[8];
};

struct ScePadDeviceClassData {
  ScePadDeviceClass deviceClass;

  bool bDataValid;

  union {
    struct {
      float steeringWheelAngle;

      uint16_t steeringWheel;

      uint16_t acceleratorPedal;

      uint16_t brakePedal;

      uint16_t clutchPedal;

      uint16_t handBlake;

      uint8_t gear;
      uint8_t reserved[1];
    } steeringWheel;

    struct {
      uint8_t toneNumber;
      uint8_t whammyBar;

      uint8_t tilt;

      uint8_t fret;
      uint8_t fretSolo;
      uint8_t reserved[11];
    } guitar;

    struct {
      uint8_t snare;

      uint8_t tom1;

      uint8_t tom2;

      uint8_t floorTom;

      uint8_t hihatCymbal;

      uint8_t rideCymbal;

      uint8_t crashCymbal;

      uint8_t reserved[9];
    } drum;

    struct {
      uint16_t stickAxisX;

      uint16_t stickAxisY;

      uint8_t stickTwist;

      uint8_t throttle;

      uint8_t trigger;

      uint8_t rudderPedal;

      uint8_t brakePedalLeft;

      uint8_t brakePedalRight;

      uint8_t antennaKnob;

      uint8_t rangeKnob;

      uint8_t reserved[4];
    } flightStick;
  } classData;
};

struct ScePadDeviceClassExtendedInformation {
  ScePadDeviceClass deviceClass;

  uint8_t reserved[4];

  union {
    struct {
      uint8_t  capability;
      uint8_t  reserved1[1];
      uint16_t maxPhysicalWheelAngle;
      uint8_t  reserved2[8];
    } steeringWheel;

    struct {
      uint8_t capability;
      uint8_t quantityOfSelectorSwitch;
      uint8_t reserved[10];
    } guitar;

    struct {
      uint8_t capability;
      uint8_t reserved[11];
    } drum;

    struct {
      uint8_t capability;
      uint8_t reserved[11];
    } flightStick;

    uint8_t data[12];
  } classData;
};

struct ScePadExtControllerInformation {};
