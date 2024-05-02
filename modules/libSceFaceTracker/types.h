#pragma once
#include "codes.h"

enum class SceFaceTrackerFaceDetectDictionary {
 	FACE_FRONTAL_DICT        = 0,
	FACE_ROLL_DICT           = 1,
	FACE_YAW_DICT            = 2,
	FACE_PITCH_DICT          = 3,
	FACE_ROLL_YAW_DICT       = 4,
	FACE_ROLL_YAW_PITCH_DICT = 5,
	FACE_DICT_MAX            = 6,
	FACE_DICT_UNDEF          = 0xFFFF, 
};

typedef void* SceFaceTrackerStartTrackingCallbackFunc (
  void* callbackParam
);

typedef void* SceFaceTrackerFixUserIdCallbackFunc (
  int         trackId,
	int         userId,
	const void* featureData,
	int         featureDataSize,
	void*       callbackParam
);

typedef void* SceFaceTrackerLostTargetCallbackFunc (
  int   trackId,
	int   userId,
	void* callbackParam
);

typedef void* SceFaceTrackerRecoveryFromLostTargetCallbackFunc (
  int   trackId,
	int   userId,
	void* callbackParam
);

typedef void* SceFaceTrackerStopTrackingCallbackFunc (
  int   trackId,
	int   userId,
	void* callbackParam
);

struct SceFaceTrackerAttributeScores {
  float smile;
	float gender;
	float baby;
	float adult;
	float elder;
	float leyeopen;
	float reyeopen;
	float glasses;
};

struct SceFacePartsResult {
  unsigned int partsId;
	float        partsX;
	float        partsY;
	float        score;
};

struct SceFaceTrackerResult {
  int                           status;
	int                           trackId;
	int                           userId;
	float                         posX;
	float                         posY;
	float                         faceWidth;
	float                         faceHeight;
	float                         faceRoll;
	float                         facePitch;
	float                         faceYaw;
	float                         score;
	float                         partsX[PARTS_NUM_MAX];
	float                         partsY[PARTS_NUM_MAX];
	SceFacePartsResult            partsResult[PARTS_NUM_MAX];
	int                           partsCheckFlag;
	SceFaceTrackerAttributeScores attributeScores;
};

struct SceFaceTrackerGlobalSearchConfig {
  float                              magBegin;
	float                              magEnd;
	float                              magStep;
	int                                xScanStart;
	int                                yScanStart;
	int                                xScanStep;
	int                                yScanStep;
	float                              xScanOver;
	float                              yScanOver;
	float                              thresholdScore;
	int                                resultPrecision;
	int                                searchType;
	int                                maxFaceNum;
	SceFaceTrackerFaceDetectDictionary dictionary;
};

struct SceFaceTrackerLocalSearchConfig {
 	float                              magStep;
	float                              xExpandRegion;
	float                              yExpandRegion;
	int                                xScanStep;
	int                                yScanStep;
	float                              thresholdScore;
	SceFaceTrackerFaceDetectDictionary dictionary; 
};

struct SceFaceTrackerIdentifyConfig {
  float fixUserIdThresholdScore;
	int   fixUserIdIdentifyCount;
	float unknownUserThresholdScore;
	int   unknownUserIdentifyCount;
	bool  enableFeatureUpdate;
};

struct SceFaceTrackerConfig {
  SceFaceTrackerGlobalSearchConfig globalSearchConfig;
	SceFaceTrackerLocalSearchConfig  localSearchConfig;
	SceFaceTrackerIdentifyConfig     identifyConfig;
	int                              framesToLost;
	float                            trackThresholdScore;
	int                              localSearchInterval;
	int                              userIdentifyInterval;
	int                              attributeInterval;
	bool                             enableUserIdentify;
	bool                             enableAttribute;
};