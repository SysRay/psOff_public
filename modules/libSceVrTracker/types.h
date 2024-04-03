#pragma once
#include "codes.h"
#include "modules_include/sceTypes.h"

enum class SceVrTrackerProfile {
	SCE_VR_TRACKER_PROFILE_000 = 0,
	SCE_VR_TRACKER_PROFILE_100 = 100,
};

enum class SceVrTrackerExecutionMode {
	SCE_VR_TRACKER_EXECUTION_MODE_SERIAL   = 0,
	SCE_VR_TRACKER_EXECUTION_MODE_PARALLEL = 1,
};

enum class SceVrTrackerCalibrationMode {
	SCE_VR_TRACKER_CALIBRATION_MANUAL = 0,
	SCE_VR_TRACKER_CALIBRATION_AUTO   = 1,
};

enum class SceVrTrackerDeviceType {
  SCE_VR_TRACKER_DEVICE_HMD        = 0,
  SCE_VR_TRACKER_DEVICE_DUALSHOCK4 = 1,
  SCE_VR_TRACKER_DEVICE_MOVE       = 2,
  SCE_VR_TRACKER_DEVICE_GUN        = 3,
};

enum class SceVrTrackerPreferenceType {
	SCE_VR_TRACKER_PREFERENCE_FAR_POSITION    = 0,
	SCE_VR_TRACKER_PREFERENCE_STABLE_POSITION = 1,
};

enum class SceVrTrackerCameraMetaCheckMode {
	SCE_VR_TRACKER_CAMERA_META_CHECK_ENABLE    = 0,
	SCE_VR_TRACKER_CAMERA_META_CHECK_DISABLE   = 1,
};

enum class SceVrTrackerDevicePermitType {
	SCE_VR_TRACKER_DEVICE_PERMIT_ALL      = 0,
	SCE_VR_TRACKER_DEVICE_PERMIT_HMD_ONLY = 1,
};

enum SceVrTrackerRobustnessLevel {
	SCE_VR_TRACKER_ROBUSTNESS_LEVEL_HIGH   = 0,
	SCE_VR_TRACKER_ROBUSTNESS_LEVEL_LOW    = 3,
	SCE_VR_TRACKER_ROBUSTNESS_LEVEL_MEDIUM = 6,
	SCE_VR_TRACKER_ROBUSTNESS_LEVEL_LEGACY = 99,
};

struct SceVrTrackerCalibrationSettings {
	SceVrTrackerCalibrationMode hmdPosition;
	SceVrTrackerCalibrationMode padPosition;
	SceVrTrackerCalibrationMode movePosition;
	SceVrTrackerCalibrationMode gunPosition;
	uint32_t reserved[4];
};

struct SceVrTrackerInitParam {
	uint32_t sizeOfThis;
	SceVrTrackerProfile profile;
	SceVrTrackerExecutionMode executionMode;
	int32_t hmdThreadPriority;
	int32_t padThreadPriority;
	int32_t moveThreadPriority;
	int32_t gunThreadPriority;
	int32_t reserved00;
	SceKernelCpumask cpumask;
	SceVrTrackerCalibrationSettings calibrationSettings;
	void* directMemoryOnionPointer;
	uint32_t directMemoryOnionSize;
	uint32_t directMemoryOnionAlignment;
	void* directMemoryGarlicPointer;
	uint32_t directMemoryGarlicSize;
	uint32_t directMemoryGarlicAlignment;
	void* workMemoryPointer;
	uint32_t workMemorySize;
	uint32_t workMemoryAlignment;
	int32_t gpuPipeId;
	int32_t gpuQueueId;
};

struct SceVrTrackerQueryMemoryParam {
	uint32_t sizeOfThis;
	SceVrTrackerProfile profile;
	uint32_t reserved[6];
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

struct SceCameraFramePosition {
	uint32_t x;
	uint32_t y;
	uint32_t xSize;
	uint32_t ySize;
};

struct SceCameraExposureGain {
	uint32_t exposureControl;
	uint32_t exposure;
	uint32_t gain;
	uint32_t mode;
};

struct SceCameraWhiteBalance {
	uint32_t whiteBalanceControl;
	uint32_t gainRed;
	uint32_t gainBlue;
	uint32_t gainGreen;
};

struct SceCameraGamma {
	uint32_t gammaControl;
	uint32_t value;
	uint8_t reserved[16];
};

struct SceCameraMeta {
	uint32_t metaMode;
	uint32_t format[SCE_CAMERA_MAX_DEVICE_NUM][SCE_CAMERA_MAX_FORMAT_LEVEL_NUM];
	uint64_t frame[SCE_CAMERA_MAX_DEVICE_NUM];
	uint64_t timestamp[SCE_CAMERA_MAX_DEVICE_NUM];
	uint32_t deviceTimestamp[SCE_CAMERA_MAX_DEVICE_NUM];
	SceCameraExposureGain exposureGain[SCE_CAMERA_MAX_DEVICE_NUM];
	SceCameraWhiteBalance whiteBalance[SCE_CAMERA_MAX_DEVICE_NUM];
	SceCameraGamma gamma[SCE_CAMERA_MAX_DEVICE_NUM];
	uint32_t luminance[SCE_CAMERA_MAX_DEVICE_NUM];
	SceFVector3 acceleration;
	uint64_t vcounter;
	uint32_t reserved[16];
};

struct SceCameraFrameData {
	uint32_t sizeThis;
	uint32_t readMode;
	SceCameraFramePosition framePosition[SCE_CAMERA_MAX_DEVICE_NUM][SCE_CAMERA_MAX_FORMAT_LEVEL_NUM];
	void *pFramePointerList[SCE_CAMERA_MAX_DEVICE_NUM][SCE_CAMERA_MAX_FORMAT_LEVEL_NUM];
	uint32_t frameSize[SCE_CAMERA_MAX_DEVICE_NUM][SCE_CAMERA_MAX_FORMAT_LEVEL_NUM];
	uint32_t status[SCE_CAMERA_MAX_DEVICE_NUM];
	SceCameraMeta meta;
	void *pFramePointerListGarlic[
	  SCE_CAMERA_MAX_DEVICE_NUM][SCE_CAMERA_MAX_FORMAT_LEVEL_NUM
	];
};

struct SceVrTrackerGpuSubmitParam {
	int32_t sizeOfThis;
	SceVrTrackerPreferenceType padTrackingPreference;
	SceVrTrackerCameraMetaCheckMode cameraMetaCheckMode;
	SceVrTrackerDevicePermitType trackingDevicePermitType;
	SceVrTrackerRobustnessLevel robustnessLevel;
	uint32_t reserved00[10];
	uint32_t reserved01;
	SceCameraFrameData cameraFrameData;
};
