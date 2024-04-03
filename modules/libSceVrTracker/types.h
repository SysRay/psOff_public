#pragma once
#include "codes.h"
#include "modules/libSceCamera/types.h"
#include "modules_include/sceTypes.h"

enum class SceVrTrackerProfile {
  PROFILE_000 = 0,
  PROFILE_100 = 100,
};

enum class SceVrTrackerExecutionMode {
  SERIAL   = 0,
  PARALLEL = 1,
};

enum class SceVrTrackerCalibrationMode {
  MANUAL = 0,
  AUTO   = 1,
};

enum class SceVrTrackerDeviceType {
  HMD        = 0,
  DUALSHOCK4 = 1,
  MOVE       = 2,
  GUN        = 3,
};

enum class SceVrTrackerPreferenceType {
  FAR_POSITION    = 0,
  STABLE_POSITION = 1,
};

enum class SceVrTrackerCameraMetaCheckMode {
  ENABLE  = 0,
  DISABLE = 1,
};

enum class SceVrTrackerDevicePermitType {
  ALL      = 0,
  HMD_ONLY = 1,
};

enum SceVrTrackerRobustnessLevel {
  HIGH   = 0,
  LOW    = 3,
  MEDIUM = 6,
  LEGACY = 99,
};

struct SceVrTrackerCalibrationSettings {
  SceVrTrackerCalibrationMode hmdPosition;
  SceVrTrackerCalibrationMode padPosition;
  SceVrTrackerCalibrationMode movePosition;
  SceVrTrackerCalibrationMode gunPosition;
  uint32_t                    reserved[4];
};

struct SceVrTrackerInitParam {
  uint32_t                        sizeOfThis;
  SceVrTrackerProfile             profile;
  SceVrTrackerExecutionMode       executionMode;
  int32_t                         hmdThreadPriority;
  int32_t                         padThreadPriority;
  int32_t                         moveThreadPriority;
  int32_t                         gunThreadPriority;
  int32_t                         reserved00;
  SceKernelCpumask                cpumask;
  SceVrTrackerCalibrationSettings calibrationSettings;
  void*                           directMemoryOnionPointer;
  uint32_t                        directMemoryOnionSize;
  uint32_t                        directMemoryOnionAlignment;
  void*                           directMemoryGarlicPointer;
  uint32_t                        directMemoryGarlicSize;
  uint32_t                        directMemoryGarlicAlignment;
  void*                           workMemoryPointer;
  uint32_t                        workMemorySize;
  uint32_t                        workMemoryAlignment;
  int32_t                         gpuPipeId;
  int32_t                         gpuQueueId;
};

struct SceVrTrackerQueryMemoryParam {
  uint32_t                        sizeOfThis;
  SceVrTrackerProfile             profile;
  uint32_t                        reserved[6];
  SceVrTrackerCalibrationSettings calibrationSettings;
};

struct SceVrTrackerQueryMemoryResult {
  uint32_t sizeOfThis;
  uint32_t directMemoryOnionSize;
  uint32_t directMemoryOnionAlignment;
  uint32_t directMemoryGarlicSize;
  uint32_t directMemoryGarlicAlignment;
  uint32_t workMemorySize;
  uint32_t workMemoryAlignment;
  uint32_t reserved[9];
};

struct SceVrTrackerGpuSubmitParam {
  int32_t                         sizeOfThis;
  SceVrTrackerPreferenceType      padTrackingPreference;
  SceVrTrackerCameraMetaCheckMode cameraMetaCheckMode;
  SceVrTrackerDevicePermitType    trackingDevicePermitType;
  SceVrTrackerRobustnessLevel     robustnessLevel;
  uint32_t                        reserved00[10];
  uint32_t                        reserved01;
  SceCameraFrameData              cameraFrameData;
};
