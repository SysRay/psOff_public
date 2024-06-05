#pragma once
#include "codes.h"

enum class MoveButton : uint32_t {
  SQUARE      = 1 << 7, // Square button.
  CROSS       = 1 << 6, // Cross button.
  CIRCLE      = 1 << 5, // Circle button.
  TRIANGLE    = 1 << 4, // Triangle button.
  MOVE        = 1 << 2, // Move button.
  T           = 1 << 1, // T button.
  START       = 1 << 3, // START button.
  SELECT      = 1 << 0, // SELECT button.
  INTERCEPTED = 1 << 15 // System has intercepted button presses.
};

/**
 * @brief Success values for
 * sceMoveGetDeviceInfo(), sceMoveReadStateRecent(), sceMoveReadStateLatest(), sceMoveSetLightSphere(),
 *  and sceMoveSetVibration()
 *
 */
enum class RetValues : int { NO_CONTROLLER_CONNECTED = 1, NO_EXTERNAL_PORT_DEVICE = 9, TYPE_STANDARD = 0, MAX_STATE_NUM = 32 };

struct SceMoveDeviceInfo {
  float sphereRadius           = 0.f;             ///< Radius of the physical sphere (in mm).
  float accelToSphereOffset[3] = {0.f, 0.f, 0.f}; ///< Offset of accelerometer location relative to sphere center in device coordinates.
};

struct SceMoveButtonData {
  uint16_t digitalButtons = 0; ///< State of each of the button controls. See #SCE_MOVE_BUTTON_CIRCLE, #SCE_MOVE_BUTTON_CROSS, #SCE_MOVE_BUTTON_MOVE,
                               ///< #SCE_MOVE_BUTTON_SELECT, #SCE_MOVE_BUTTON_SQUARE, #SCE_MOVE_BUTTON_START, #SCE_MOVE_BUTTON_T, and #SCE_MOVE_BUTTON_TRIANGLE.
  uint16_t analogT = 0;        ///< Analog value of T button.
};

struct SceMoveExtensionPortData {
  uint16_t      status       = 0;
  uint16_t      digital1     = 0;
  uint16_t      digital2     = 0;
  uint16_t      analogRightX = 0;
  uint16_t      analogRightY = 0;
  uint16_t      analogLeftX  = 0;
  uint16_t      analogLeftY  = 0;
  unsigned char custom[5]    = {0, 0, 0, 0, 0};
};

struct SceMoveData {
  float                    accelerometer[3] = {0.f, 0.f, 0.f}; ///< Accelerometer readings in sensor coordinates (G units).
  float                    gyro[3]          = {0.f, 0.f, 0.f}; ///< Gyro readings in sensor coordinates (radians/s).
  SceMoveButtonData        pad {};                             ///< State of the motion controller buttons.
  SceMoveExtensionPortData ext {};                             ///< Reserved.
  int64_t                  timestamp   = 0;                    ///< System timestamp of this data (microseconds).
  int                      counter     = 0;                    ///< Numbers the sensor data sequentially.
  float                    temperature = 0.f;                  ///< The temperature of the sensor (used internally).
};