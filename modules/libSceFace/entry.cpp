#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceFace);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceFace";

EXPORT SYSV_ABI int sceFaceDetection(const unsigned char* imgPtr, int width, int height, int rowstride, 
                                     const SceFaceDetectionDictPtr detectDictPtr, float magBegin, float magStep, float magEnd, 
                                     int xScanStep, int yScanStep, float thresholdScore, int resultPrecision, 
                                     SceFaceDetectionResult resultFaceArray[], int resultFaceArraySize, int* resultFaceNum, void* workMemory,
                                     int workMemorySize) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceDetectionEx(const unsigned char* imgPtr, int width, int height,
	                                     int rowstride, const SceFaceDetectionDictPtr detectDictPtr, const SceFaceDetectionParam* detectParam,
	                                     SceFaceDetectionResult resultFaceArray[], int resultFaceArraySize, int* resultFaceNum, void* workMemory,
                                       int workMemorySize) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceDetectionLocal(const unsigned char* imgPtr, int width, int height, int rowstride,
	                                        const SceFaceDetectionDictPtr detectDictPtr, float magStep, float xExpandRegion, float yExpandRegion,
	                                        int xScanStep, int yScanStep, float thresholdScore, const SceFaceDetectionResult referenceFaceArray[],
	                                        int referenceFaceArraySize, SceFaceDetectionResult resultFaceArray[], int resultFaceArraySize, int* resultFaceNum,
	                                        void* workMemory, int workMemorySize) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceDetectionGetDefaultParam(SceFaceDetectionParam* detectParam) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceDetectionGetWorkingMemorySize(int width, int height, int rowstride, const SceFaceDetectionDictPtr detectDictPtr) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceParts(const unsigned char* imgPtr, int width, int height, int rowstride,
	                               const SceFacePartsDictPtr partsDictPtr, int xScanStep, int yScanStep, const SceFaceDetectionResult* detectedFace,
	                               SceFacePartsResult resultPartsArray[], int resultPartsArraySize, int* resultPartsNum, void* workMemory,
	                               int workMemorySize) {
  return Ok;
}

EXPORT SYSV_ABI int sceFacePartsEx(const unsigned char* imgPtr, int width, int height, int rowstride,
	                                 const SceFacePartsDictPtr partsDictPtr, const SceFacePartsCheckDictPtr partsCheckDictPtr, int xScanStep, int yScanStep,
	                                 const SceFaceDetectionResult* detectedFace, SceFacePartsResult resultPartsArray[], int resultPartsArraySize, int* resultPartsNum,
	                                 void* workMemory, int workMemorySize) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceAllParts(const unsigned char* imgPtr, int width, int height, int rowstride,
	                                  const SceFacePartsDictPtr partsDictPtr, const SceFaceShapeDictPtr shapeDictPtr, int xScanStep, int yScanStep,
	                                  const SceFaceDetectionResult* detectedFace, SceFacePartsResult resultPartsArray[], int resultPartsArraySize, int* resultPartsNum,
	                                  void* workMemory, int workMemorySize) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceEstimatePoseRegion(int width, int height, const SceFaceDetectionResult* detectedFace, const SceFacePartsResult detectedPartsArray[],
	                                            int detectedPartsNum, SceFacePose* resultFacePose, SceFaceRegion* resultFaceRegion) {
  return Ok;
}

EXPORT SYSV_ABI int sceFacePartsResultCheck(const SceFaceDetectionResult* detectedFace, const SceFacePartsResult detectedPartsArray[], int detectedPartsNum, const SceFacePartsCheckDictPtr partsCheckDictPtr,
	                                          bool* good) {
  return Ok;
}

EXPORT SYSV_ABI int sceFacePartsGetWorkingMemorySize(int width, int height, int rowstride, const SceFacePartsDictPtr partsDictPtr) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceAllPartsGetWorkingMemorySize(int width, int height, int rowstride, const SceFacePartsDictPtr partsDictPtr) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceAttribute(const unsigned char* imgPtr, int width, int height, int rowstride,
	                                   const SceFaceAttribDictPtr attribDictPtr, const SceFaceDetectionResult* detectedFace, const SceFacePartsResult detectedPartsArray[], int detectedPartsNum,
	                                   SceFaceAttribResult resultAttribArray[], int resultAttribArraySize, int* resultAttribNum, void* workMemory,
	                                   int workMemorySize) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceAgeRangeEstimate(const unsigned char* imgPtr, int width, int height, int rowstride,
	                                          const SceFaceAgeDictPtr ageDictPtr, const SceFaceDetectionResult* detectedFace, const SceFacePartsResult detectedPartsArray[], int detectedPartsNum,
	                                          SceFaceAgeRangeResult* resultAge, void* workMemory, int workMemorySize) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceAgeRangeIntegrate(const SceFaceAgeRangeResult* ageRange, SceFaceAgeDistrData* ageDistrData, int* ageResult) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceAttributeGetWorkingMemorySize(int width, int height, int rowstride, const SceFaceAttribDictPtr attribDictPtr) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceAgeGetWorkingMemorySize(int width, int height, int rowstride, const SceFaceAgeDictPtr ageDictPtr) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceIdentifyGetFeature(const unsigned char* imgPtr, int width, int height, int rowstride,
	                                            const SceFaceIdentifyDictPtr identifyDictPtr, const SceFaceDetectionResult* detectedFace, const SceFacePartsResult detectedPartsArray[], int detectedPartsNum,
	                                            SceFaceIdentifyFeature* resultFeature, void* workMemory, int workMemorySize) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceIdentifyLiteGetFeature(const unsigned char* imgPtr, int width, int height, int rowstride,
	                                                const SceFaceIdentifyLiteDictPtr identifyDictPtr, const SceFaceDetectionResult* detectedFace, const SceFacePartsResult detectedPartsArray[], int detectedPartsNum,
	                                                SceFaceIdentifyLiteFeature* resultFeature, void* workMemory, int workMemorySize) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceIdentifyExGetFeature(const unsigned char* imgPtr, int width, int height, int rowstride,
	                                              const SceFaceIdentifyExDictPtr identifyDictPtr, const SceFaceDetectionResult* detectedFace, const SceFacePartsResult detectedPartsArray[], int detectedPartsNum,
	                                              const SceFaceAttribResult detectedAttribArray[], int detectedAttribNum, SceFaceIdentifyExFeature* resultFeature, void* workMemory,
	                                              int workMemorySize) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceIdentifySimilarity(const SceFaceIdentifyFeature* extractedFeature, const SceFaceIdentifyFeature registeredFeatureArray[], int registeredFeatureNum, const SceFaceIdentifyDictPtr identifyDictPtr,
	                                            float* maxScore, int* maxScoreId, float resultScoreArray[]) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceIdentifyLiteSimilarity(const SceFaceIdentifyLiteFeature* extractedFeature, const SceFaceIdentifyLiteFeature registeredFeatureArray[], int registeredFeatureNum, const SceFaceIdentifyLiteDictPtr identifyDictPtr,
	                                                float *maxScore, int* maxScoreId, float resultScoreArray[]) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceIdentifyExSimilarity(const SceFaceIdentifyExFeature* extractedFeature, const SceFaceIdentifyExFeature registeredFeatureArray[], int registeredFeatureNum, const SceFaceIdentifyExDictPtr identifyDictPtr,
	                                              float* maxScore, int* maxScoreId, float resultScoreArray[]) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceIdentifyGetWorkingMemorySize(int width, int height, int rowstride, const SceFaceIdentifyDictPtr identifyDictPtr) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceIdentifyLiteGetWorkingMemorySize(int width, int height, int rowstride, const SceFaceIdentifyLiteDictPtr identifyDictPtr) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceIdentifyExGetWorkingMemorySize(int width, int height, int rowstride, const SceFaceIdentifyExDictPtr identifyDictPtr) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceShapeFit(const unsigned char* imgPtr, int width, int height, int rowstride,
	                                  const SceFaceShapeModelDictPtr shapeDictPtr, SceFaceShapeResult* shape, float lostThreshold, const SceFaceDetectionResult* detectedFace,
	                                  const SceFacePartsResult detectedPartsArray[], int detectedPartsNum, void* workMemory,int workMemorySize) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceShapeTrack(const unsigned char* imgPtrCur, const unsigned char* imgPtrPrv, int width, int height,
	                                    int rowstride, const SceFaceShapeModelDictPtr shapeDictPtr, SceFaceShapeResult* shape, float lostThreshold,
	                                    void* workMemory, int workMemorySize) {
  return Ok;
}

EXPORT SYSV_ABI int sceFaceShapeGetWorkingMemorySize(int width, int height, int rowstride, const SceFaceShapeModelDictPtr shapeDictPtr,
	                                                   int maxFaceWidth, int maxFaceHeight, bool isVideoInput) {
  return Ok;
}
}