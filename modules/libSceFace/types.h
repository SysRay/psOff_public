#pragma once
#include "codes.h"

typedef void* SceFaceAttribDictPtr;
typedef void* SceFaceAgeDictPtr;
typedef void* SceFaceDetectionDictPtr;
typedef void* SceFacePartsDictPtr;
typedef void* SceFacePartsCheckDictPtr;
typedef void* SceFaceShapeDictPtr;
typedef void* SceFaceIdentifyDictPtr;
typedef void* SceFaceIdentifyLiteDictPtr;
typedef void* SceFaceIdentifyExDictPtr;
typedef void* SceFaceShapeModelDictPtr;

struct SceFaceAgeDistrData {
  float score[AGE_BIN_SIZE];
	int   numIntegrated;
};

struct SceFaceAttribResult {
  unsigned int attribId;
	float        score;
};

struct SceFaceAgeRangeResult {
  unsigned char maxAge;
	unsigned char minAge;
};

struct SceFaceDetectionResult {
  float faceX;
	float faceY;
	float faceW;
	float faceH;
	float faceRoll;
	float facePitch;
	float faceYaw;
	float score;
};

struct SceFacePartsResult {
  unsigned int partsId;
	float        partsX;
	float        partsY;
	float        score;
};

struct SceFaceDetectionParam {
  int   version;
	int   size;
	float magBegin;
	float magStep;
	float magEnd;
	int   xScanStart;
	int   yScanStart;
	int   xScanStep;
	int   yScanStep;
	float xScanOver;
	float yScanOver;
	float thresholdScore;
	int   resultPrecision;
	int   searchType;
};

struct SceFaceIdentifyFeature {
  unsigned char data[IDENTIFY_FEATURE_SIZE];
};

struct SceFaceIdentifyLiteFeature {
  unsigned char data[IDENTIFY_LITE_FEATURE_SIZE];
};

struct SceFaceIdentifyExFeature {
  unsigned char data[IDENTIFY_EX_FEATURE_SIZE];
};

struct SceFaceShapeResult {
  int           modelID;
	int           pointNum;
	float         pointX[SHAPE_POINT_NUM_MAX];
	float         pointY[SHAPE_POINT_NUM_MAX];
	int           isLost[SHAPE_POINT_NUM_MAX];
	float         fourPointX[PARTS_NUM_MAX];
	float         fourPointY[PARTS_NUM_MAX];
	float         rectCenterX;
	float         rectCenterY;
	float         rectWidth;
	float         rectHeight;
	float         faceRoll;
	float         facePitch;
	float         faceYaw;
	float         score;
	unsigned char data[4096];
};

struct SceFacePose {	
  float faceRoll;
	float facePitch;
	float faceYaw;
};

struct SceFaceRegion {	
  float faceRegionX[4];
	float faceRegionY[4];
};