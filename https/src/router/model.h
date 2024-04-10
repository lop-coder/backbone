#ifndef _ROUTER_MODEL_H_
#define _ROUTER_MODEL_H_

#include <string>

#include "annotation/annotationschema.h"
#include "dataset/datasetschema.h"
#include "filter/filterschema.h"
#include "http.h"
#include "image/imageschema.h"
#include "log/logschema.h"
#include "model/modelschema.h"
#include "model/modelschemaV3.h"
#include "nlohmann/json.hpp"
#include "project/projectschema.h"
#include "tag/tagschema.h"
using nlohmann::json;

std::string getLogLevel(int level);

class ModelRouter {
 public:
  ModelRouter();
  ~ModelRouter();

  DECLARE_ROUTER(create);
  DECLARE_ROUTER(listModels);
  DECLARE_ROUTER(get);
  DECLARE_ROUTER(put);
  DECLARE_ROUTER(deleteModel);
  DECLARE_ROUTER(cancelTrainModel);
  DECLARE_ROUTER(getCm);
  DECLARE_ROUTER(duplicate);
  DECLARE_ROUTER(getRelatedDatasets);
  DECLARE_ROUTER(getRelatedModels);
  DECLARE_ROUTER(createModelTests);
  DECLARE_ROUTER(listModelTests);
  DECLARE_ROUTER(getModelTest);
  DECLARE_ROUTER(updateModelTest);
  DECLARE_ROUTER(deleteModelTest);
  DECLARE_ROUTER(cancelModelTest);
  DECLARE_ROUTER(getModelTestCm);
  DECLARE_ROUTER(duplicateModelTest);
  DECLARE_ROUTER(testModel);
  DECLARE_ROUTER(getModelTestLog);
  DECLARE_ROUTER(getModelTestProgress);
  DECLARE_ROUTER(getModelTestResult);
  DECLARE_ROUTER(downloadModelTestResult);
  DECLARE_ROUTER(trainModel);
  DECLARE_ROUTER(getModelTrainBoard);
  DECLARE_ROUTER(getModelTrainLog);
  DECLARE_ROUTER(getModelTrainProgress);
  DECLARE_ROUTER(getModelValResult);
  DECLARE_ROUTER(getModelTrainTags);
  DECLARE_ROUTER(getGPUStatus);
  DECLARE_ROUTER(getTrainDefaultConfig);
  DECLARE_ROUTER(getTestDefaultConfig);
  DECLARE_ROUTER(getCmV2);
  DECLARE_ROUTER(getCmInfo);
  DECLARE_ROUTER(putCmInfo);
  DECLARE_ROUTER(getTestCmV2);
  DECLARE_ROUTER(getTestCmInfo);
  DECLARE_ROUTER(putTestCmInfo);
  DECLARE_ROUTER(postModels);
  DECLARE_ROUTER(getTestVersion);
  DECLARE_ROUTER(getModelTestDataset);
  DECLARE_ROUTER(checkModelUsedByPipeline);
  DECLARE_ROUTER(getCPUStatus);
  DECLARE_ROUTER(getDevicesStatus);
  DECLARE_ROUTER(checkDeviceStatus);

  DECLARE_ROUTER(getCmV3);
  DECLARE_ROUTER(getCmInfoV3);
  DECLARE_ROUTER(putCmInfoV3);
  DECLARE_ROUTER(getTestCmV3);
  DECLARE_ROUTER(getTestCmInfoV3);
  DECLARE_ROUTER(putTestCmInfoV3);

  DECLARE_ROUTER(checkModelDatasetStatus);
  DECLARE_ROUTER(checkModelTestDatasetStatus);
};

#endif  //_ROUTER_MODEL_H_
