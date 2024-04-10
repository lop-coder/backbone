#include "model.h"

#include <iostream>

#include "common.h"
#include "logger.h"
#include "logic.h"
#include "logictype.h"
#include "schema.h"
//#include "wrapper.h"
////////////////////////////////////////////////////////////////////////////////////////////////////
using namespace TurboHttp;
////////////////////////////////////////////////////////////////////////////////////////////////////
ModelRouter::ModelRouter() {
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/models)", ModelRouter::create);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/models/list)",
                  ModelRouter::listModels);
  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/models/(\d+))",
                  ModelRouter::get);
  REGISTER_ROUTER("PUT", R"(/v1/projects/(\d+)/models/(\d+))",
                  ModelRouter::put);
  REGISTER_ROUTER("DELETE", R"(/v1/projects/(\d+)/models/(\d+))",
                  ModelRouter::deleteModel);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/models/(\d+)/cancel_train)",
                  ModelRouter::cancelTrainModel);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/models/(\d+)/cm)",
                  ModelRouter::getCm);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/models/(\d+)/duplicate)",
                  ModelRouter::duplicate);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/models/(\d+)/related_dataset)",
                  ModelRouter::getRelatedDatasets);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/models/(\d+)/related_model)",
                  ModelRouter::getRelatedModels);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/models/(\d+)/tests)",
                  ModelRouter::createModelTests);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/models/(\d+)/tests/list)",
                  ModelRouter::listModelTests);
  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/models/(\d+)/tests/(\d+))",
                  ModelRouter::getModelTest);
  REGISTER_ROUTER("PUT", R"(/v1/projects/(\d+)/models/(\d+)/tests/(\d+))",
                  ModelRouter::updateModelTest);
  REGISTER_ROUTER("DELETE", R"(/v1/projects/(\d+)/models/(\d+)/tests/(\d+))",
                  ModelRouter::deleteModelTest);

  REGISTER_ROUTER("POST",
                  R"(/v1/projects/(\d+)/models/(\d+)/tests/(\d+)/cancel_test)",
                  ModelRouter::cancelModelTest);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/models/(\d+)/tests/(\d+)/cm)",
                  ModelRouter::getModelTestCm);
  REGISTER_ROUTER("POST",
                  R"(/v1/projects/(\d+)/models/(\d+)/tests/(\d+)/duplicate)",
                  ModelRouter::duplicateModelTest);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/models/(\d+)/tests/(\d+)/test)",
                  ModelRouter::testModel);
  REGISTER_ROUTER("GET",
                  R"(/v1/projects/(\d+)/models/(\d+)/tests/(\d+)/test_log)",
                  ModelRouter::getModelTestLog);
  REGISTER_ROUTER(
      "GET", R"(/v1/projects/(\d+)/models/(\d+)/tests/(\d+)/test_progress)",
      ModelRouter::getModelTestProgress);
  REGISTER_ROUTER("GET",
                  R"(/v1/projects/(\d+)/models/(\d+)/tests/(\d+)/test_result)",
                  ModelRouter::getModelTestResult);

  REGISTER_ROUTER(
      "POST", R"(/v1/projects/(\d+)/models/(\d+)/tests/(\d+)/download_result)",
      ModelRouter::downloadModelTestResult);

  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/models/(\d+)/train)",
                  ModelRouter::trainModel);
  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/models/(\d+)/train_board)",
                  ModelRouter::getModelTrainBoard);
  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/models/(\d+)/train_log)",
                  ModelRouter::getModelTrainLog);
  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/models/(\d+)/train_progress)",
                  ModelRouter::getModelTrainProgress);
  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/models/(\d+)/val_result)",
                  ModelRouter::getModelValResult);
  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/models/(\d+)/train_tags)",
                  ModelRouter::getModelTrainTags);
  REGISTER_ROUTER("GET", R"(/v1/gpus)", ModelRouter::getGPUStatus);
  REGISTER_ROUTER("GET", R"(/v1/projects/models/default_train_config)",
                  ModelRouter::getTrainDefaultConfig);
  REGISTER_ROUTER("GET",
                  R"(/v1/projects/(\d+)/models/(\d+)/default_test_config)",
                  ModelRouter::getTestDefaultConfig);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/models/(\d+)/models)",
                  ModelRouter::postModels);
  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/models/(\d+)/test_version)",
                  ModelRouter::getTestVersion);
  REGISTER_ROUTER("POST", R"(/v2/projects/(\d+)/models/(\d+)/cm)",
                  ModelRouter::getCmV2);
  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/models/(\d+)/cm_info)",
                  ModelRouter::getCmInfo);
  REGISTER_ROUTER("PUT", R"(/v1/projects/(\d+)/models/(\d+)/cm_info)",
                  ModelRouter::putCmInfo);
  REGISTER_ROUTER("POST", R"(/v2/projects/(\d+)/models/(\d+)/tests/(\d+)/cm)",
                  ModelRouter::getTestCmV2);
  REGISTER_ROUTER("GET",
                  R"(/v1/projects/(\d+)/models/(\d+)/tests/(\d+)/cm_info)",
                  ModelRouter::getTestCmInfo);
  REGISTER_ROUTER("PUT",
                  R"(/v1/projects/(\d+)/models/(\d+)/tests/(\d+)/cm_info)",
                  ModelRouter::putTestCmInfo);
  REGISTER_ROUTER(
      "GET", R"(/v1/projects/(\d+)/models/(\d+)/tests/(\d+)/test_datasets)",
      ModelRouter::getModelTestDataset);

  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/models/(\d+)/check)",
                  ModelRouter::checkModelUsedByPipeline);
  REGISTER_ROUTER("GET", "/v1/cpu_status", ModelRouter::getCPUStatus);
  REGISTER_ROUTER("GET", "/v1/devices_status", ModelRouter::getDevicesStatus);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/pre_execute_check_device)",
                  ModelRouter::checkDeviceStatus);

  // v3
  REGISTER_ROUTER("POST", R"(/v3/projects/(\d+)/models/(\d+)/cm)",
                  ModelRouter::getCmV3);
  REGISTER_ROUTER("GET", R"(/v3/projects/(\d+)/models/(\d+)/cm_info)",
                  ModelRouter::getCmInfoV3);
  REGISTER_ROUTER("PUT", R"(/v3/projects/(\d+)/models/(\d+)/cm_info)",
                  ModelRouter::putCmInfoV3);
  REGISTER_ROUTER("POST", R"(/v3/projects/(\d+)/models/(\d+)/tests/(\d+)/cm)",
                  ModelRouter::getTestCmV3);
  REGISTER_ROUTER("GET",
                  R"(/v3/projects/(\d+)/models/(\d+)/tests/(\d+)/cm_info)",
                  ModelRouter::getTestCmInfoV3);
  REGISTER_ROUTER("PUT",
                  R"(/v3/projects/(\d+)/models/(\d+)/tests/(\d+)/cm_info)",
                  ModelRouter::putTestCmInfoV3);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/models/(\d+)/check_dataset)",
                  ModelRouter::checkModelDatasetStatus);

  REGISTER_ROUTER(
      "POST", R"(/v1/projects/(\d+)/models/(\d+)/tests/(\d+)/check_dataset)",
      ModelRouter::checkModelTestDatasetStatus);
}
ModelRouter::~ModelRouter() {}
////////////////////////////////////////////////////////////////////////////////////////////////////
/*
resMsg* ModelRouter::getTestVersion(std::string method, std::string host,
                                    std::string proto, std::string path,
                                    std::map<std::string, std::string> querys,
                                    std::map<std::string, std::string> headers,
                                    std::map<std::string, std::string> form,
                                    std::string body)

*/

DEFINITION_ROUTER(ModelRouter, getTestVersion) {
  int projectId = 0;
  int modelId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/models/%d/test_version",
                         &projectId, &modelId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;

  TurboLogic::GetModelTestVersionRequest request;
  TurboLogic::GetModelTestVersionResponse response;

  request.projectId = projectId;
  request.modelId = modelId;

  logic.model()->getModelTestVersion(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;

  res["data"] = json::object();
  res["data"]["version"] = response.version;

  // std::cout << "request.algorithmType: " << request.algorithmType <<
  // std::endl;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
resMsg* ModelRouter::postModels(std::string method, std::string host,
                                std::string proto, std::string path,
                                std::map<std::string, std::string> querys,
                                std::map<std::string, std::string> headers,
                                std::map<std::string, std::string> form,
                                std::string body)*/
DEFINITION_ROUTER(ModelRouter, postModels) {
  int projectId = 0;
  int modelId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/models/%d/models",
                         &projectId, &modelId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }

  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;

  std::vector<std::string> needEntryList;
  needEntryList.push_back("relationType");
  needEntryList.push_back("status");

  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::GetModelsWithRelevanceRequest request;
  TurboLogic::GetModelsWithRelevanceResponse response;

  request.projectId = projectId;
  request.modelId = modelId;
  request.relationType = req["relationType"];

  request.status.clear();
  if (req["status"].size() > 0) {
    auto status = req["status"];
    for (int i = 0; i < status.size(); i++) {
      if (status[i] == TurboLogic::ProcessStatus::Finished ||
          status[i] == TurboLogic::ProcessStatus::Idle ||
          status[i] == TurboLogic::ProcessStatus::Progress ||
          status[i] == TurboLogic::ProcessStatus::Failed ||
          status[i] == TurboLogic::ProcessStatus::Canceled ||
          status[i] == TurboLogic::ProcessStatus::Cancelling) {
        request.status.push_back(status[i]);
      }
    }
  }
  if (request.status.empty()) {
    request.status.push_back(TurboLogic::ProcessStatus::Finished);
  }

  if (request.relationType != TurboLogic::ModelDatasetType::Training &&
      request.relationType != TurboLogic::ModelDatasetType::Validation &&
      request.relationType != TurboLogic::ModelDatasetType::Test) {
    request.relationType = TurboLogic::ModelDatasetType::Training;
  }

  logic.model()->getModelsWithRelevance(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;

  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"]["models"] = json::array();
  for (auto model : response.models) {
    json detail;
    detail = {
        {"datasetsSame", model.datasetsSame},
        {"valDatasetsSame", model.valDatasetsSame},
        {"modelConfig", model.modelConfig},
        {"modelCreateTime", model.createTime},
        {"modelDescription", model.modelDescription},
        {"modelEditable", model.editable},
        {"modelId", model.modelId},
        {"modelProgress", model.progress},
        {"modelModifiedTime", model.modifyTime},
        {"modelName", model.modelName},
        {"modelStatus", model.trainStatus},
        {"algorithmType", model.algorithmType},
    };
    res["data"]["models"].push_back(detail);
  }

  std::string dumpData;
  dumpResponse(res, dumpData);

  return std::make_shared<resMsg>(res.dump());
}

/*
resMsg* ModelRouter::getTrainDefaultConfig(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(ModelRouter, getTrainDefaultConfig) {
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;

  TurboLogic::AlgorithmType algorithmType =
      TurboLogic::SingleLabelClassification;
  std::string modelType = "";
  if (querys.find("algorithmType") != querys.end()) {
    algorithmType =
        TurboLogic::AlgorithmType(std::stoi(querys["algorithmType"]));
  } else {
    res["code"] = TurboLogic::ErrorType::ErrInvalidArg;
    res["msg"] =
        TurboLogic::getErrorMessage(TurboLogic::ErrorType::ErrInvalidArg);
    return std::make_shared<resMsg>(res.dump());
  }
  if (querys.find("modelType") != querys.end()) {
    modelType = querys["modelType"];
  } else {
    modelType = "transformer";
  }
  if (modelType != "transformer" && modelType != "cnn") {
    res["code"] = TurboLogic::ErrorType::ErrInvalidArg;
    res["msg"] =
        TurboLogic::getErrorMessage(TurboLogic::ErrorType::ErrInvalidArg);
    return std::make_shared<resMsg>(res.dump());
  }
  TurboLogic::GetModelTrainDefaultConfigRequest request;
  TurboLogic::GetModelTrainDefaultConfigResponse response;

  request.modelType = modelType;
  request.algorithmType = algorithmType;
  logic.model()->getModelTrainDefaultConfig(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  res["data"] = response.modelConfig;
  std::cout << "request.algorithmType: " << request.algorithmType << std::endl;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
resMsg* ModelRouter::getTestDefaultConfig(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(ModelRouter, getTestDefaultConfig) {
  int projectId = 0;
  int modelId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/models/%d/default_test_config",
             &projectId, &modelId);
  if (converted == 1) {
    // std::cout << "projectId:" << projectId<<" modelId:"<< modelId<<
    // std::endl;
  }
  std::cout << "projectId:" << projectId << " modelId:" << modelId << std::endl;
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;

  TurboLogic::GetModelTestDefaultConfigRequest request;
  TurboLogic::GetModelTestDefaultConfigResponse response;

  request.modelId = modelId;
  request.projectId = projectId;

  logic.model()->getModelTestDefaultConfig(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  res["data"] = response.modelConfig;

  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
resMsg* ModelRouter::create(std::string method, std::string host,
                            std::string proto, std::string path,
                            std::map<std::string, std::string> querys,
                            std::map<std::string, std::string> headers,
                            std::map<std::string, std::string> form,
                            std::string body)
*/
DEFINITION_ROUTER(ModelRouter, create) {
  int id = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/models", &id);
  if (converted == 1) {
    std::cout << "id:" << id << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  json schema = schemaOfRoot();

  schema["properties"]["modelDescription"] = schemaOfDescription();
  schema["properties"]["modelName"] = schemaOfItem("string");
  schema["properties"]["algorithmType"] = schemaOfAlgoType();
  schema["properties"]["modelConfig"] = schemaOfItem("string");
  schema["required"].push_back("modelDescription");
  schema["required"].push_back("modelName");
  schema["required"].push_back("modelConfig");
  schema["required"].push_back("algorithmType");

  if (!jsonParseAndSchemaCheck(body, schema, req, res)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::CreateModelRequest request;
  TurboLogic::CreateModelResponse response;

  if (req.contains("modelDescription")) {
    request.modelDescription = req["modelDescription"];
  } else {
    request.modelDescription = "";
  }
  request.modelName = req["modelName"];
  request.modelConfig = req["modelConfig"];
  request.algorithmType = TurboLogic::AlgorithmType(req["algorithmType"]);

  if (request.modelConfig != "") {
    try {
      auto modelConfigJson = json::parse(request.modelConfig);
      if (modelConfigJson.contains("optimizer") &&
          modelConfigJson["optimizer"].is_object() &&
          modelConfigJson["optimizer"].contains("lr_scheduler") &&
          modelConfigJson["optimizer"]["lr_scheduler"].is_object() &&
          modelConfigJson["optimizer"].contains("lr_scheduler_step") &&
          modelConfigJson["optimizer"]["lr_scheduler_step"].is_object()) {
        if (modelConfigJson["optimizer"]["lr_scheduler"].contains("lr") &&
            modelConfigJson["optimizer"]["lr_scheduler"]["lr"]
                .is_number_float()) {
          modelConfigJson["optimizer"]["lr_scheduler"]["Lr"] =
              modelConfigJson["optimizer"]["lr_scheduler"]["lr"];
          modelConfigJson["optimizer"]["lr_scheduler"].erase("lr");
        }

        if (modelConfigJson["optimizer"]["lr_scheduler"].contains("power") &&
            modelConfigJson["optimizer"]["lr_scheduler"]["power"]
                .is_number_float()) {
          modelConfigJson["optimizer"]["lr_scheduler"]["Power"] =
              modelConfigJson["optimizer"]["lr_scheduler"]["power"];
          modelConfigJson["optimizer"]["lr_scheduler"].erase("power");
        }
        if (modelConfigJson["optimizer"]["lr_scheduler"].contains(
                "warmup_iters") &&
            modelConfigJson["optimizer"]["lr_scheduler"]["warmup_iters"]
                .is_number_integer()) {
          modelConfigJson["optimizer"]["lr_scheduler"]["WarmupIters"] =
              modelConfigJson["optimizer"]["lr_scheduler"]["warmup_iters"];
          modelConfigJson["optimizer"]["lr_scheduler"].erase("warmup_iters");
        }
        if (modelConfigJson["optimizer"]["lr_scheduler"].contains("steps") &&
            modelConfigJson["optimizer"]["lr_scheduler"]["steps"].is_array()) {
          modelConfigJson["optimizer"]["lr_scheduler"]["Steps"] =
              modelConfigJson["optimizer"]["lr_scheduler"]["steps"];
          modelConfigJson["optimizer"]["lr_scheduler"].erase("steps");
        }

        if (modelConfigJson["optimizer"]["lr_scheduler_step"].contains("lr") &&
            modelConfigJson["optimizer"]["lr_scheduler_step"]["lr"]
                .is_number_float()) {
          modelConfigJson["optimizer"]["lr_scheduler_step"]["Lr"] =
              modelConfigJson["optimizer"]["lr_scheduler_step"]["lr"];
          modelConfigJson["optimizer"]["lr_scheduler_step"].erase("lr");
        }
        if (modelConfigJson["optimizer"]["lr_scheduler_step"].contains(
                "power") &&
            modelConfigJson["optimizer"]["lr_scheduler_step"]["power"]
                .is_number_float()) {
          modelConfigJson["optimizer"]["lr_scheduler_step"]["Power"] =
              modelConfigJson["optimizer"]["lr_scheduler_step"]["power"];
          modelConfigJson["optimizer"]["lr_scheduler_step"].erase("power");
        }
        if (modelConfigJson["optimizer"]["lr_scheduler_step"].contains(
                "warmup_iters") &&
            modelConfigJson["optimizer"]["lr_scheduler_step"]["warmup_iters"]
                .is_number_integer()) {
          modelConfigJson["optimizer"]["lr_scheduler_step"]["WarmupIters"] =
              modelConfigJson["optimizer"]["lr_scheduler_step"]["warmup_iters"];
          modelConfigJson["optimizer"]["lr_scheduler_step"].erase(
              "warmup_iters");
        }
        if (modelConfigJson["optimizer"]["lr_scheduler_step"].contains(
                "steps") &&
            modelConfigJson["optimizer"]["lr_scheduler_step"]["steps"]
                .is_array()) {
          modelConfigJson["optimizer"]["lr_scheduler_step"]["Steps"] =
              modelConfigJson["optimizer"]["lr_scheduler_step"]["steps"];
          modelConfigJson["optimizer"]["lr_scheduler_step"].erase("steps");
        }
      }
    } catch (const nlohmann::detail::parse_error& e) {
      res["msg"] = "parse request modelConfig fail";
      res["code"] = TurboLogic::ErrorType::ErrInvalidArg;
      return std::make_shared<resMsg>(res.dump());
    }
  }

  if (!(checkArg(request.algorithmType, "AlgorithmType", res))) {
    return std::make_shared<resMsg>(res.dump());
  }

  request.projectId = id;
  logic.model()->createModel(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  std::cout << "request.datasetType: " << request.algorithmType << std::endl;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = {
      {"modelConfig", response.modelConfig},
      {"modelCreateTime", response.createTime},
      {"modelDescription", response.modelDescription},
      {"modelEditable", response.editable},
      {"modelId", response.modelId},
      {"modelProgress", response.progress},
      {"modelModifiedTime", response.modifyTime},
      {"modelName", response.modelName},
      {"modelStatus", response.trainStatus},
      {"algorithmType", response.algorithmType},
  };
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
resMsg* ModelRouter::listModels(std::string method, std::string host,
                                std::string proto, std::string path,
                                std::map<std::string, std::string> querys,
                                std::map<std::string, std::string> headers,
                                std::map<std::string, std::string> form,
                                std::string body)
*/
DEFINITION_ROUTER(ModelRouter, listModels) {
  int id = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/models/list", &id);
  if (converted == 1) {
    std::cout << "id:" << id << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";
  json reqExample = {{"modelName", "modelName"},
                     {"modelStatus", {1, 2}},
                     {"algorithmType", {1, 2}}};

  std::vector<std::string> needEntryList;
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }
  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::ListModelsRequest request;
  TurboLogic::ListModelsResponse response;

  if (req.contains("modelName")) {
    request.modelName = req["modelName"];
  } else {
    request.modelName = "";
  }
  if (!(req.contains("modelStatus")) || req["modelStatus"].size() == 0) {
    req["modelStatus"] = {
        TurboLogic::ProcessStatus::Failed,
        TurboLogic::ProcessStatus::Canceled,
        TurboLogic::ProcessStatus::Cancelling,
        TurboLogic::ProcessStatus::Finished,
        TurboLogic::ProcessStatus::Idle,
        TurboLogic::ProcessStatus::Progress,
    };
  }
  for (auto& element : req["modelStatus"]) {
    if (!(checkArg(element, "ProcessStatus", res))) {
      return std::make_shared<resMsg>(res.dump());
    }
    request.modelStatus.push_back(TurboLogic::ProcessStatus(element));
  }

  if (!(req.contains("algorithmType")) || req["algorithmType"].size() == 0) {
    req["algorithmType"] = {
        TurboLogic::AlgorithmType::SingleLabelClassification,
        TurboLogic::AlgorithmType::Segmentation,
        TurboLogic::AlgorithmType::MutiLabelClassification,
        TurboLogic::AlgorithmType::Detection,
        TurboLogic::AlgorithmType::OCR,
    };
  }

  for (auto& element : req["algorithmType"]) {
    if (!(checkArg(element, "AlgorithmType", res))) {
      return std::make_shared<resMsg>(res.dump());
    }
    request.algorithmType.push_back(TurboLogic::AlgorithmType(element));
  }
  request.projectId = id;
  logic.model()->listModels(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"]["models"] = json::array();
  res["data"]["total"] = response.total;
  for (auto model : response.models) {
    json detail;
    detail = {
        {"modelConfig", model.modelConfig},
        {"modelCreateTime", model.createTime},
        {"modelDescription", model.modelDescription},
        {"modelEditable", model.editable},
        {"modelId", model.modelId},
        {"modelProgress", model.progress},
        {"modelModifiedTime", model.modifyTime},
        {"modelName", model.modelName},
        {"modelStatus", model.trainStatus},
        {"algorithmType", model.algorithmType},
    };
    res["data"]["models"].push_back(detail);
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
resMsg* ModelRouter::get(std::string method, std::string host,
                         std::string proto, std::string path,
                         std::map<std::string, std::string> querys,
                         std::map<std::string, std::string> headers,
                         std::map<std::string, std::string> form,
                         std::string body)
                         */
DEFINITION_ROUTER(ModelRouter, get) {
  int projectId = 0;
  int modelId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/models/%d", &projectId, &modelId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::GetModelInfoRequest request;
  TurboLogic::GetModelInfoResponse response;

  request.projectId = projectId;
  request.modelId = modelId;
  logic.model()->getModelInfo(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = {
      {"modelConfig", response.modelConfig},
      {"modelCreateTime", response.createTime},
      {"modelDescription", response.modelDescription},
      {"modelEditable", response.editable},
      {"modelId", request.modelId},
      {"modelProgress", response.progress},
      {"modelModifiedTime", response.modifyTime},
      {"modelName", response.modelName},
      {"modelStatus", response.trainStatus},
      {"algorithmType", response.algorithmType},
  };
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
resMsg* ModelRouter::put(std::string method, std::string host,
                         std::string proto, std::string path,
                         std::map<std::string, std::string> querys,
                         std::map<std::string, std::string> headers,
                         std::map<std::string, std::string> form,
                         std::string body)
*/
DEFINITION_ROUTER(ModelRouter, put) {
  int projectId = 0;
  int modelId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/models/%d", &projectId, &modelId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  json schema = schemaOfRoot();

  schema["properties"]["modelDescription"] = schemaOfDescription();
  schema["properties"]["modelName"] = schemaOfItem("string");
  schema["properties"]["modelConfig"] = schemaOfItem("string");

  if (!jsonParseAndSchemaCheck(body, schema, req, res)) {
    return std::make_shared<resMsg>(res.dump());
  }
  TurboLogic::GetModelInfoRequest getInforequest;
  TurboLogic::GetModelInfoResponse getInforesponse;

  getInforequest.projectId = projectId;
  getInforequest.modelId = modelId;
  logic.model()->getModelInfo(getInforequest, getInforesponse);
  res["code"] = getInforesponse.error.type;
  res["msg"] = getInforesponse.error.message;
  if (getInforesponse.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::UpdateModelInfoRequest request;
  TurboLogic::UpdateModelInfoResponse response;

  if (req.contains("modelConfig")) {
    request.modelConfig = req["modelConfig"];
  } else {
    request.modelConfig = getInforesponse.modelConfig;
  }

  if (req.contains("modelDescription")) {
    request.modelDescription = req["modelDescription"];
  } else {
    request.modelDescription = getInforesponse.modelDescription;
  }

  if (req.contains("modelName")) {
    request.modelName = req["modelName"];
  } else {
    request.modelName = getInforesponse.modelName;
  }

  request.modelId = modelId;
  request.projectId = projectId;
  logic.model()->updateModelInfo(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = {
      {"modelConfig", response.model.modelConfig},
      {"modelCreateTime", response.model.createTime},
      {"modelDescription", response.model.modelDescription},
      {"modelEditable", response.model.editable},
      {"modelId", request.modelId},
      {"modelProgress", response.model.progress},
      {"modelModifiedTime", response.model.modifyTime},
      {"modelName", response.model.modelName},
      {"modelStatus", response.model.trainStatus},
      {"algorithmType", response.model.algorithmType},
  };
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
resMsg* ModelRouter::deleteModel(std::string method, std::string host,
                                 std::string proto, std::string path,
                                 std::map<std::string, std::string> querys,
                                 std::map<std::string, std::string> headers,
                                 std::map<std::string, std::string> form,
                                 std::string body)
*/
DEFINITION_ROUTER(ModelRouter, deleteModel) {
  int projectId = 0;
  int modelId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/models/%d", &projectId, &modelId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::DeleteModelRequest request;
  TurboLogic::DeleteModelResponse response;

  request.modelId = modelId;
  request.projectId = projectId;
  logic.model()->deleteModel(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
resMsg* ModelRouter::cancelTrainModel(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(ModelRouter, cancelTrainModel) {
  int projectId = 0;
  int modelId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/models/%d/cancel_train",
                         &projectId, &modelId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::CancelTrainModelRequest request;
  TurboLogic::CancelTrainModelResponse response;

  request.projectId = projectId;
  request.modelId = modelId;
  logic.model()->cancelTrainModel(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
resMsg* ModelRouter::getCm(std::string method, std::string host,
                           std::string proto, std::string path,
                           std::map<std::string, std::string> querys,
                           std::map<std::string, std::string> headers,
                           std::map<std::string, std::string> form,
                           std::string body)
*/
DEFINITION_ROUTER(ModelRouter, getCm) {
  int projectId = 0;
  int modelId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/models/%d/cm",
                         &projectId, &modelId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  std::vector<std::string> needEntryList;
  needEntryList.push_back("currentTagIds");
  needEntryList.push_back("datasetId");
  needEntryList.push_back("ngAllTagIds");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::GetValConfusionMatrixRequest request;
  TurboLogic::GetValConfusionMatrixResponse response;

  if (req.contains("currentTagIds")) {
    for (auto& element : req["currentTagIds"]) {
      request.currentTagIds.push_back(element);
    }
  } else {
    request.currentTagIds = {0};
  }

  if (req.contains("ngAllTagIds")) {
    for (auto& element : req["ngAllTagIds"]) {
      request.ngAllTagIds.push_back(element);
    }
  } else {
    request.ngAllTagIds = {0};
  }

  request.projectId = projectId;
  request.modelId = modelId;
  request.datasetId = req["datasetId"].get<int>();

  logic.model()->getValConfusionMatrix(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }
  res["data"]["imageCm"] = json::object();
  res["data"]["instanceCm"] = json::object();
  std::vector<std::string> cmTypes;
  cmTypes.push_back("imageCm");
  cmTypes.push_back("instanceCm");
  for (auto& cmType : cmTypes) {
    TurboLogic::ConfusionMatrixInfo info;
    info = response.instances;
    if (cmType == "imageCm") {
      info = response.images;
    }
    res["data"][cmType]["colNames"] = json::array();
    for (auto colName : info.colNames) {
      res["data"][cmType]["colNames"].push_back(colName);
    }
    res["data"][cmType]["colTotal"] = json::array();
    for (auto total : info.colTotal) {
      res["data"][cmType]["colTotal"].push_back(total);
    }
    res["data"][cmType]["data"] = json::array();
    for (auto& firstLayer : info.data) {
      auto first = json::array();
      for (auto& secondLayer : firstLayer) {
        auto second = json::array();
        for (auto& thirdLayer : secondLayer) {
          second.push_back(thirdLayer);
        }
        first.push_back(second);
      }
      res["data"][cmType]["data"].push_back(first);
    }
    res["data"][cmType]["precision"] = json::array();
    for (auto prc : info.precision) {
      res["data"][cmType]["precision"].push_back(prc);
    }
    res["data"][cmType]["recall"] = json::array();
    for (auto rec : info.recall) {
      res["data"][cmType]["recall"].push_back(rec);
    }
    res["data"][cmType]["rowNames"] = json::array();
    for (auto rowName : info.rowNames) {
      res["data"][cmType]["rowNames"].push_back(rowName);
    }
    res["data"][cmType]["rowTotal"] = json::array();
    for (auto rtl : info.rowTotal) {
      res["data"][cmType]["rowTotal"].push_back(rtl);
    }
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
resMsg* ModelRouter::getCmV2(std::string method, std::string host,
                             std::string proto, std::string path,
                             std::map<std::string, std::string> querys,
                             std::map<std::string, std::string> headers,
                             std::map<std::string, std::string> form,
                             std::string body)
*/
DEFINITION_ROUTER(ModelRouter, getCmV2) {
  int projectId = 0;
  int modelId = 0;
  int converted = sscanf(path.c_str(), "/v2/projects/%d/models/%d/cm",
                         &projectId, &modelId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  std::vector<std::string> needEntryList;
  needEntryList.push_back("tagsAll");
  needEntryList.push_back("thresholds");
  needEntryList.push_back("type");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::GetValConfusionMatrixV2Request request;
  TurboLogic::GetValConfusionMatrixV2Response response;

  if (req.contains("iou")) {
    ;
  } else {
    req["iou"] = 0.5;
  }
  req["projectId"] = projectId;
  req["modelId"] = modelId;

  request.reqStr = req.dump();

  logic.model()->getValConfusionMatrixV2(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }
  res["data"] = json::parse(response.resStr);
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
resMsg* ModelRouter::getCmInfo(std::string method, std::string host,
                               std::string proto, std::string path,
                               std::map<std::string, std::string> querys,
                               std::map<std::string, std::string> headers,
                               std::map<std::string, std::string> form,
                               std::string body)
                               */
DEFINITION_ROUTER(ModelRouter, getCmInfo) {
  int projectId = 0;
  int modelId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/models/%d/cm_info",
                         &projectId, &modelId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  // std::vector<std::string> needEntryList;
  // if (!parseBody(body, req, res, needEntryList)) {
  //     return std::make_shared<resMsg>(res.dump());
  // }

  req["projectId"] = projectId;
  req["modelId"] = modelId;

  std::string result;
  logic.model()->getValConfusionMatrixInfo(req.dump(), result);
  res["data"] = json::parse(result);
  res = json::parse(result);
  std::cout << "555" << res << std::endl;
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
resMsg* ModelRouter::putCmInfo(std::string method, std::string host,
                               std::string proto, std::string path,
                               std::map<std::string, std::string> querys,
                               std::map<std::string, std::string> headers,
                               std::map<std::string, std::string> form,
                               std::string body)
                               */
DEFINITION_ROUTER(ModelRouter, putCmInfo) {
  int projectId = 0;
  int modelId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/models/%d/cm_info",
                         &projectId, &modelId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  std::vector<std::string> needEntryList;
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }

  req["projectId"] = projectId;
  req["modelId"] = modelId;

  std::string result;
  logic.model()->updateValConfusionMatrixInfo(req.dump(), result);
  res["data"] = json::parse(result);
  std::cout << "4444444 " << res << std::endl;
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
resMsg* ModelRouter::getTestCmV2(std::string method, std::string host,
                                 std::string proto, std::string path,
                                 std::map<std::string, std::string> querys,
                                 std::map<std::string, std::string> headers,
                                 std::map<std::string, std::string> form,
                                 std::string body)
*/
DEFINITION_ROUTER(ModelRouter, getTestCmV2) {
  int projectId = 0;
  int modelId = 0;
  int testId = 0;
  int converted = sscanf(path.c_str(), "/v2/projects/%d/models/%d/tests/%d/cm",
                         &projectId, &modelId, &testId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  std::vector<std::string> needEntryList;
  needEntryList.push_back("tagsAll");
  needEntryList.push_back("thresholds");
  needEntryList.push_back("type");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }

  std::string request;
  std::string response;

  if (req.contains("iou")) {
    ;
  } else {
    req["iou"] = 0.5;
  }
  req["projectId"] = projectId;
  req["modelId"] = modelId;
  req["testId"] = testId;

  request = req.dump();

  logic.model()->getTestConfusionMatrixV2(request, response);
  res = json::parse(response);
  std::cout << "555" << res << std::endl;
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
resMsg* ModelRouter::getTestCmInfo(std::string method, std::string host,
                                   std::string proto, std::string path,
                                   std::map<std::string, std::string> querys,
                                   std::map<std::string, std::string> headers,
                                   std::map<std::string, std::string> form,
                                   std::string body)
                                   */
DEFINITION_ROUTER(ModelRouter, getTestCmInfo) {
  int projectId = 0;
  int modelId = 0;
  int testId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/models/%d/tests/%d/cm_info",
             &projectId, &modelId, &testId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  // std::vector<std::string> needEntryList;
  // if (!parseBody(body, req, res, needEntryList)) {
  //     return std::make_shared<resMsg>(res.dump());
  // }

  req["projectId"] = projectId;
  req["modelId"] = modelId;
  req["testId"] = testId;

  std::cout << "555" << req << std::endl;

  std::string result;
  logic.model()->getTestConfusionMatrixInfo(req.dump(), result);
  json resData = json::parse(result);
  res["data"] = resData["data"];
  res["msg"] = resData["msg"];
  res["code"] = resData["code"];
  std::cout << "555" << res << std::endl;
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
resMsg* ModelRouter::putTestCmInfo(std::string method, std::string host,
                                   std::string proto, std::string path,
                                   std::map<std::string, std::string> querys,
                                   std::map<std::string, std::string> headers,
                                   std::map<std::string, std::string> form,
                                   std::string body)
*/
DEFINITION_ROUTER(ModelRouter, putTestCmInfo) {
  int projectId = 0;
  int modelId = 0;
  int testId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/models/%d/tests/%d/cm_info",
             &projectId, &modelId, &testId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  std::vector<std::string> needEntryList;
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }

  req["projectId"] = projectId;
  req["modelId"] = modelId;
  req["testId"] = testId;
  std::cout << "555" << req << std::endl;

  std::string result;
  logic.model()->updateTestConfusionMatrixInfo(req.dump(), result);
  res["data"] = json::parse(result);
  std::cout << "4444444 " << res << std::endl;
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
resMsg* ModelRouter::duplicate(std::string method, std::string host,
                               std::string proto, std::string path,
                               std::map<std::string, std::string> querys,
                               std::map<std::string, std::string> headers,
                               std::map<std::string, std::string> form,
                               std::string body)
                               */
DEFINITION_ROUTER(ModelRouter, duplicate) {
  int projectId = 0;
  int modelId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/models/%d/duplicate",
                         &projectId, &modelId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  std::vector<std::string> needEntryList;
  // needEntryList.push_back("modelDescription");
  needEntryList.push_back("modelName");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::DuplicateModelRequest request;
  TurboLogic::DuplicateModelResponse response;

  if (req.contains("modelDescription")) {
    request.newModelDescription = req["modelDescription"];
  } else {
    request.newModelDescription = "";
  }
  request.newModelName = req["modelName"];

  request.projectId = projectId;
  request.modelId = modelId;
  logic.model()->duplicateModel(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = {
      {"modelConfig", response.modelConfig},
      {"modelCreateTime", response.createTime},
      {"modelDescription", response.modelDescription},
      {"modelEditable", response.editable},
      {"modelId", response.newModelId},
      {"modelModifiedTime", response.modifyTime},
      {"modelName", response.modelName},
      {"modelStatus", response.trainStatus},
      {"algorithmType", response.algorithmType},
  };
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
resMsg* ModelRouter::getRelatedDatasets(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
    */
DEFINITION_ROUTER(ModelRouter, getRelatedDatasets) {
  int projectId = 0;
  int modelId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/models/%d/related_dataset",
             &projectId, &modelId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::GetModelRelatedDatasetsRequest request;
  TurboLogic::GetModelRelatedDatasetsResponse response;

  request.projectId = projectId;
  request.modelId = modelId;
  logic.model()->getModelRelatedDatasets(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"]["datasets"] = json::array();

#if 1
  auto func = [](const int& projectId, int& relationType,
                 std::vector<int>& datasetIds,
                 std::vector<std::string>& datasetUUIDs, json& info) {
    LogDebug(" size:{} {}", datasetIds.size(), datasetUUIDs.size());
    for (int i = 0; i < datasetIds.size(); i++) {
      TurboLogic::Logic& ilogic =
          TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
      TurboLogic::GetDatasetInfoRequest getInfoRequest;
      TurboLogic::GetDatasetInfoResponse getInfoResponse;
      getInfoRequest.datasetId = datasetIds[i];
      getInfoRequest.projectId = projectId;
      ilogic.dataset()->getDatasetInfo(getInfoRequest, getInfoResponse);
      json detail = json::object();
      detail["dataset"] = {
          {"datasetCreateTime", getInfoResponse.createTime},
          {"datasetDescription", getInfoResponse.datasetDescription},
          //{"datasetEditable", getInfoResponse.editable},
          {"isUsed", !getInfoResponse.editable},
          {"datasetId", getInfoResponse.datasetId},
          {"datasetModifiedTime", getInfoResponse.modifyTime},
          {"datasetName", getInfoResponse.datasetName},
          {"datasetType", getInfoResponse.datasetType},
          {"parentDatasetId", getInfoResponse.parentDatasetId},
          {"parentDatasetName", getInfoResponse.parentDatasetName},
          {"timestamp", datasetUUIDs[i]}};

      detail["relationType"] = relationType;
      info.push_back(detail);
    }
  };

  int relationType = TurboLogic::ModelDatasetType::Test;
  func(projectId, relationType, response.testDatasetIds,
       response.testDatasetUUIDs, res["data"]["datasets"]);

  relationType = TurboLogic::ModelDatasetType::Training;
  func(projectId, relationType, response.trainingDatasetIds,
       response.trainingDatasetUUIDs, res["data"]["datasets"]);

  relationType = TurboLogic::ModelDatasetType::Validation;
  func(projectId, relationType, response.validationDatasetIds,
       response.validationDatasetUUIDs, res["data"]["datasets"]);

#else
  constexpr auto getDatasetInfo = [&](const int& projectId, int& relationType,
                                      std::vector<int>& datasetIds) {
    auto func = [&](json& info) {
      for (auto id : datasetIds) {
        TurboLogic::Logic& ilogic =
            TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
        TurboLogic::GetDatasetInfoRequest getInfoRequest;
        TurboLogic::GetDatasetInfoResponse getInfoResponse;
        getInfoRequest.datasetId = id;
        getInfoRequest.projectId = projectId;
        ilogic.dataset()->getDatasetInfo(getInfoRequest, getInfoResponse);
        json detail = json::object();
        detail["dataset"] = {
            {"datasetCreateTime", getInfoResponse.createTime},
            {"datasetDescription", getInfoResponse.datasetDescription},
            //{"datasetEditable", getInfoResponse.editable},
            {"isUsed", !getInfoResponse.editable},
            {"datasetId", getInfoResponse.datasetId},
            {"datasetModifiedTime", getInfoResponse.modifyTime},
            {"datasetName", getInfoResponse.datasetName},
            {"datasetType", getInfoResponse.datasetType},
            {"parentDatasetId", getInfoResponse.parentDatasetId},
            {"parentDatasetName", getInfoResponse.parentDatasetName}};
        detail["relationType"] = relationType;
        info.push_back(detail);
      }
    };
    return [](json& info) {
      func(info);
    };
  };
  int relationType = TurboLogic::ModelDatasetType::Test;

  auto getTestDatasetInfos =
      getDatasetInfo(projectId, relationType, response.testDatasetIds);
  getTestDatasetInfos(res["data"]["datasets"]);

  relationType = TurboLogic::ModelDatasetType::Training;

  auto getTrainingDatasetInfos =
      getDatasetInfo(projectId, relationType, response.trainingDatasetIds);
  getTrainingDatasetInfos(res["data"]["datasets"]);

  relationType = TurboLogic::ModelDatasetType::Validation;

  auto getValidationDatasetInfos =
      getDatasetInfo(projectId, relationType, response.validationDatasetIds);

  getValidationDatasetInfos(res["data"]["datasets"]);
#endif

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
resMsg* ModelRouter::getRelatedModels(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(ModelRouter, getRelatedModels) {
  int projectId = 0;
  int modelId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/models/%d/related_model",
             &projectId, &modelId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::GetModelRelatedModelsRequest request;
  TurboLogic::GetModelRelatedModelsResponse response;

  std::vector<std::string> needEntryList;
  needEntryList.push_back("relationType");
  needEntryList.push_back("status");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }
  json reqExample = {{"relationType", 1}, {"status", {1}}};
  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<resMsg>(res.dump());
  }
  if (!(checkArg(req["relationType"], "Integer", res))) {
    return std::make_shared<resMsg>(res.dump());
  }
  request.relationType = req["relationType"].get<int>();
  std::cout << "relationType:" << req["relationType"] << std::endl;
  request.status.clear();
  if (!(req.contains("status")) || req["status"].size() == 0) {
    req["status"] = {TurboLogic::ProcessStatus::Finished};
  }
  for (auto& element : req["status"]) {
    std::cout << "element:" << element << std::endl;
    if (!(checkArg(element, "ProcessStatus", res))) {
      return std::make_shared<resMsg>(res.dump());
    }
    request.status.push_back(TurboLogic::ProcessStatus(element));
  }
  /* if (req["status"].size() > 0) {
    auto status = req["status"];
    for (int i = 0; i < status.size(); i++) {
      request.status.push_back(status[i]);
    }
  } else {
    request.status.push_back(TurboLogic::ProcessStatus::Finished);
  }*/

  if (request.relationType != TurboLogic::ModelDatasetType::Training &&
      request.relationType != TurboLogic::ModelDatasetType::Validation &&
      request.relationType != TurboLogic::ModelDatasetType::Test) {
    request.relationType = TurboLogic::ModelDatasetType::Training;
  }

  request.projectId = projectId;
  request.modelId = modelId;
  // request.datasetId = req["datasetId"];
  logic.model()->getModelRelatedModel(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  // std::cout << "request.datasetType: " << request.datasetType << std::endl;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"]["models"] = json::array();
  for (auto model : response.models) {
    json detail;
    detail = {
        {"modelConfig", model.modelConfig},
        {"modelCreateTime", model.createTime},
        {"modelDescription", model.modelDescription},
        {"modelEditable", model.editable},
        {"modelId", model.modelId},
        {"modelProgress", model.progress},
        {"modelModifiedTime", model.modifyTime},
        {"modelName", model.modelName},
        {"modelStatus", model.trainStatus},
        {"algorithmType", model.algorithmType},
    };
    res["data"]["models"].push_back(detail);
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
resMsg* ModelRouter::createModelTests(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(ModelRouter, createModelTests) {
  int projectId = 0;
  int modelId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/models/%d/tests",
                         &projectId, &modelId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  json schema = schemaOfRoot();

  schema["properties"]["testDescription"] = schemaOfDescription();
  schema["properties"]["version"] = schemaOfItem("string");
  schema["properties"]["modelTestConfig"] = schemaOfItem("string");
  schema["required"].push_back("version");
  schema["required"].push_back("modelTestConfig");

  if (!jsonParseAndSchemaCheck(body, schema, req, res)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::CreateModelTestRequest request;
  TurboLogic::CreateModelTestResponse response;

  if (req.contains("testDescription")) {
    request.testDescription = req["testDescription"].get<std::string>();
  } else {
    request.testDescription = "";
  }

  request.modelTestConfig = req["modelTestConfig"].get<std::string>();
  request.version = req["version"].get<std::string>();

  request.projectId = projectId;
  request.modelId = modelId;
  // request.testDescription = req["testDescription"].get<std::string>();
  logic.model()->createModelTest(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = {
      {"modelId", response.modelId},
      {"modelName", response.modelName},
      {"modelTestId", response.modelTestId},
      {"testConfig", response.testConfig},
      {"testCreateTime", response.testCreateTime},
      {"testDescription", response.testDescription},
      {"testEditable", response.testEditable},
      {"testModifiedTime", response.testModifiedTime},
      {"testStatus", response.testStatus},
      {"testVersion", response.testVersion},
  };
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
resMsg* ModelRouter::listModelTests(std::string method, std::string host,
                                    std::string proto, std::string path,
                                    std::map<std::string, std::string> querys,
                                    std::map<std::string, std::string> headers,
                                    std::map<std::string, std::string> form,
                                    std::string body)
                                    */
DEFINITION_ROUTER(ModelRouter, listModelTests) {
  int projectId = 0;
  int modelId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/models/%d/tests/list",
                         &projectId, &modelId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  json schema = schemaOfRoot();

  auto status = schemaOfItem("array");
  status["items"] = schemaOfItem("integer");

  if (!jsonParseAndSchemaCheck(body, schema, req, res)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::ListModelTestsRequest request;
  TurboLogic::ListModelTestsResponse response;

  request.projectId = projectId;
  request.modelId = modelId;
  request.status = {TurboLogic::ProcessStatus::Canceled,
                    TurboLogic::ProcessStatus::Cancelling,
                    TurboLogic::ProcessStatus::Failed,
                    TurboLogic::ProcessStatus::Finished,
                    TurboLogic::ProcessStatus::Idle,
                    TurboLogic::ProcessStatus::Progress};
  if (req.contains("status") && !req["status"].empty()) {
    request.status = req["status"].get<std::vector<int>>();
  }
  logic.model()->listModelTests(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"]["modelTests"] = json::array();
  for (auto modeltest : response.modelTests) {
    json detail;
    detail = {{"modelId", modeltest.modelId},
              {"modelName", modeltest.modelName},
              {"modelTestId", modeltest.modelTestId},
              {"testConfig", modeltest.testConfig},
              {"testCreateTime", modeltest.testCreateTime},
              {"testDescription", modeltest.testDescription},
              {"testEditable", modeltest.testEditable},
              {"testModifiedTime", modeltest.testModifiedTime},
              {"testStatus", modeltest.testStatus},
              {"testVersion", modeltest.testVersion},
              {"progress", modeltest.progress}};
    res["data"]["modelTests"].push_back(detail);
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
resMsg* ModelRouter::getModelTest(std::string method, std::string host,
                                  std::string proto, std::string path,
                                  std::map<std::string, std::string> querys,
                                  std::map<std::string, std::string> headers,
                                  std::map<std::string, std::string> form,
                                  std::string body)
 */
DEFINITION_ROUTER(ModelRouter, getModelTest) {
  int projectId = 0;
  int modelId = 0;
  int testId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/models/%d/tests/%d",
                         &projectId, &modelId, &testId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::GetModelTestInfoRequest request;
  TurboLogic::GetModelTestInfoResponse response;

  request.projectId = projectId;
  request.modelId = modelId;
  request.testId = testId;
  logic.model()->getModelTestInfo(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = {
      {"modelId", response.modelId},
      {"modelName", response.modelName},
      {"modelTestId", response.modelTestId},
      {"testConfig", response.testConfig},
      {"testCreateTime", response.testCreateTime},
      {"testDescription", response.testDescription},
      {"testEditable", response.testEditable},
      {"testModifiedTime", response.testModifiedTime},
      {"testStatus", response.testStatus},
      {"testVersion", response.testVersion},
  };
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
resMsg* ModelRouter::updateModelTest(std::string method, std::string host,
                                     std::string proto, std::string path,
                                     std::map<std::string, std::string> querys,
                                     std::map<std::string, std::string> headers,
                                     std::map<std::string, std::string> form,
                                     std::string body) */
DEFINITION_ROUTER(ModelRouter, updateModelTest) {
  int projectId = 0;
  int modelId = 0;
  int testId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/models/%d/tests/%d",
                         &projectId, &modelId, &testId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  json schema = schemaOfRoot();

  schema["properties"]["testDescription"] = schemaOfDescription();
  schema["properties"]["testConfig"] = schemaOfItem("string");

  if (!jsonParseAndSchemaCheck(body, schema, req, res)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::GetModelTestInfoRequest getInforequest;
  TurboLogic::GetModelTestInfoResponse getInforesponse;

  getInforequest.projectId = projectId;
  getInforequest.modelId = modelId;
  getInforequest.testId = testId;
  logic.model()->getModelTestInfo(getInforequest, getInforesponse);
  res["code"] = getInforesponse.error.type;
  res["msg"] = getInforesponse.error.message;
  if (getInforesponse.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::UpdateModelTestRequest request;
  TurboLogic::UpdateModelTestResponse response;

  if (req.contains("testConfig")) {
    request.testConfig = req["testConfig"];
  } else {
    request.testConfig = getInforesponse.testConfig;
  }

  if (req.contains("testDescription")) {
    request.testDescription = req["testDescription"];
  } else {
    request.testDescription = getInforesponse.testDescription;
  }

  request.modelId = modelId;
  request.projectId = projectId;
  request.testId = testId;
  logic.model()->updateModelTest(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = {
      {"modelId", response.modelId},
      {"modelName", response.modelName},
      {"modelTestId", response.modelTestId},
      {"testConfig", response.testConfig},
      {"testCreateTime", response.testCreateTime},
      {"testDescription", response.testDescription},
      {"testEditable", response.testEditable},
      {"testModifiedTime", response.testModifiedTime},
      {"testStatus", response.testStatus},
      {"testVersion", response.testVersion},
  };
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
resMsg* ModelRouter::deleteModelTest(std::string method, std::string host,
                                     std::string proto, std::string path,
                                     std::map<std::string, std::string> querys,
                                     std::map<std::string, std::string> headers,
                                     std::map<std::string, std::string> form,
                                     std::string body)
                                     */
DEFINITION_ROUTER(ModelRouter, deleteModelTest) {
  int projectId = 0;
  int modelId = 0;
  int testId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/models/%d/tests/%d",
                         &projectId, &modelId, &testId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::DeleteModelTestRequest request;
  TurboLogic::DeleteModelTestResponse response;

  request.projectId = projectId;
  request.modelId = modelId;
  request.testId = testId;
  logic.model()->deleteModelTest(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
resMsg* ModelRouter::cancelModelTest(std::string method, std::string host,
                                     std::string proto, std::string path,
                                     std::map<std::string, std::string> querys,
                                     std::map<std::string, std::string> headers,
                                     std::map<std::string, std::string> form,
                                     std::string body)
*/
DEFINITION_ROUTER(ModelRouter, cancelModelTest) {
  int projectId = 0;
  int modelId = 0;
  int testId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/models/%d/tests/%d/cancel_test",
             &projectId, &modelId, &testId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::CancelTestModelRequest request;
  TurboLogic::CancelTestModelResponse response;

  request.projectId = projectId;
  request.modelId = modelId;
  request.testId = testId;
  logic.model()->cancelTestModel(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
resMsg* ModelRouter::getModelTestCm(std::string method, std::string host,
                                    std::string proto, std::string path,
                                    std::map<std::string, std::string> querys,
                                    std::map<std::string, std::string> headers,
                                    std::map<std::string, std::string> form,
                                    std::string body)*/
DEFINITION_ROUTER(ModelRouter, getModelTestCm) {
  int projectId = 0;
  int modelId = 0;
  int testId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/models/%d/tests/%d/cm",
                         &projectId, &modelId, &testId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  std::vector<std::string> needEntryList;
  needEntryList.push_back("currentTagIds");
  needEntryList.push_back("datasetId");
  needEntryList.push_back("ngAllTagIds");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::GetTestConfusionMatrixRequest request;
  TurboLogic::GetTestConfusionMatrixResponse response;

  if (req.contains("currentTagIds")) {
    for (auto& element : req["currentTagIds"]) {
      request.currentTagIds.push_back(element);
    }
  } else {
    request.currentTagIds = {0};
  }

  if (req.contains("ngAllTagIds")) {
    for (auto& element : req["ngAllTagIds"]) {
      request.ngAllTagIds.push_back(element);
    }
  } else {
    request.ngAllTagIds = {0};
  }

  request.projectId = projectId;
  request.datasetId = req["datasetId"];

  logic.model()->getTestConfusionMatrix(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }
  res["data"]["imageCm"] = json::object();
  res["data"]["instanceCm"] = json::object();
  std::vector<std::string> cmTypes;
  cmTypes.push_back("imageCm");
  cmTypes.push_back("instanceCm");
  for (auto& cmType : cmTypes) {
    TurboLogic::ConfusionMatrixInfo info;
    info = response.instances;
    if (cmType == "imageCm") {
      info = response.images;
    }
    res["data"][cmType]["colNames"] = json::array();
    for (auto colName : info.colNames) {
      res["data"][cmType]["colNames"].push_back(colName);
    }
    res["data"][cmType]["colTotal"] = json::array();
    for (auto total : info.colTotal) {
      res["data"][cmType]["colTotal"].push_back(total);
    }
    res["data"][cmType]["data"] = json::array();
    for (auto& firstLayer : info.data) {
      auto first = json::array();
      for (auto& secondLayer : firstLayer) {
        auto second = json::array();
        for (auto& thirdLayer : secondLayer) {
          second.push_back(thirdLayer);
        }
        first.push_back(second);
      }
      res["data"][cmType]["data"].push_back(first);
    }
    res["data"][cmType]["precision"] = json::array();
    for (auto prc : info.precision) {
      res["data"][cmType]["precision"].push_back(prc);
    }
    res["data"][cmType]["recall"] = json::array();
    for (auto rec : info.recall) {
      res["data"][cmType]["recall"].push_back(rec);
    }
    res["data"][cmType]["rowNames"] = json::array();
    for (auto rowName : info.rowNames) {
      res["data"][cmType]["rowNames"].push_back(rowName);
    }
    res["data"][cmType]["rowTotal"] = json::array();
    for (auto rtl : info.rowTotal) {
      res["data"][cmType]["rowTotal"].push_back(rtl);
    }
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
resMsg* ModelRouter::duplicateModelTest(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
    */

DEFINITION_ROUTER(ModelRouter, duplicateModelTest) {
  int projectId = 0;
  int modelId = 0;
  int testId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/models/%d/tests/%d/duplicate",
             &projectId, &modelId, &testId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  std::cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << projectId << std::endl;
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  std::vector<std::string> needEntryList;
  // needEntryList.push_back("testDescription");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::DuplicateModelTestRequest request;
  TurboLogic::DuplicateModelTestResponse response;

  if (req.contains("testDescription")) {
    request.testDescription = req["testDescription"].get<std::string>();
  } else {
    request.testDescription = "";
  }

  request.projectId = projectId;
  request.modelId = modelId;
  request.testId = testId;
  logic.model()->duplicateModelTest(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = {
      {"modelId", response.modelId},
      {"modelName", response.modelName},
      {"modelTestId", response.modelTestId},
      {"testConfig", response.testConfig},
      {"testCreateTime", response.testCreateTime},
      {"testDescription", response.testDescription},
      {"testEditable", response.testEditable},
      {"testModifiedTime", response.testModifiedTime},
      {"testStatus", response.testStatus},
      {"testVersion", response.testVersion},
  };
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
resMsg* ModelRouter::testModel(std::string method, std::string host,
                               std::string proto, std::string path,
                               std::map<std::string, std::string> querys,
                               std::map<std::string, std::string> headers,
                               std::map<std::string, std::string> form,
                               std::string body)

*/
DEFINITION_ROUTER(ModelRouter, testModel) {
  int projectId = 0;
  int modelId = 0;
  int testId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/models/%d/tests/%d/test",
             &projectId, &modelId, &testId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::TestModelRequest request;
  TurboLogic::TestModelResponse response;

  request.projectId = projectId;
  request.modelId = modelId;
  request.testId = testId;

  logic.model()->testModel(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
resMsg* ModelRouter::getModelTestLog(std::string method, std::string host,
                                     std::string proto, std::string path,
                                     std::map<std::string, std::string> querys,
                                     std::map<std::string, std::string> headers,
                                     std::map<std::string, std::string> form,
                                     std::string body)
*/
DEFINITION_ROUTER(ModelRouter, getModelTestLog) {
  int projectId = 0;
  int modelId = 0;
  int testId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/models/%d/tests/%d/test_log",
             &projectId, &modelId, &testId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }

  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::GetTestLogRequest request;
  TurboLogic::GetTestLogResponse response;

  if (querys.find("level") != querys.end()) {
    req["level"] = atoi(querys.find("level")->second.c_str());
  } else {
    res["code"] = -99;
    res["msg"] = "need level";
    return std::make_shared<resMsg>(res.dump());
  }

  request.modelId = modelId;
  request.projectId = projectId;
  request.testId = testId;
  request.level = getLogLevel(req["level"].get<int>());
  logic.model()->getTestLog(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;

  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"]["logs"] = json::array();
  for (auto log : response.logs) {
    json detail;
    detail = {{"content", log.content}, {"level", request.level},
              {"reason", log.reason},   {"suggestion", log.suggestion},
              {"time", log.time},       {"errorCode", log.errorCode}};
    res["data"]["logs"].push_back(detail);
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
resMsg* ModelRouter::getModelTestProgress(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/

DEFINITION_ROUTER(ModelRouter, getModelTestProgress) {
  int projectId = 0;
  int modelId = 0;
  int testId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/models/%d/tests/%d/test_progress",
             &projectId, &modelId, &testId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::GetModelTestProgressRequest request;
  TurboLogic::GetModelTestProgressResponse response;

  request.modelId = modelId;
  request.projectId = projectId;
  request.testId = testId;
  logic.model()->getModelTestProgress(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  // std::cout << "request.datasetType: " << request.datasetType << std::endl;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = {{"duration", response.duration},
                 {"endTime", response.endTime},
                 {"progress", response.progress},
                 {"remainingTime", response.remainingTime},
                 {"startTime", response.startTime},
                 {"status", int(response.status)}};
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
resMsg* ModelRouter::getModelTestResult(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(ModelRouter, getModelTestResult) {
  int projectId = 0;
  int modelId = 0;
  int testId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/models/%d/tests/%d/test_result",
             &projectId, &modelId, &testId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  /* std::vector<std::string> needEntryList;
  needEntryList.push_back("datasetId");
  needEntryList.push_back("imageId");
  if (!parseBody(body, req, res, needEntryList)) {
      return std::make_shared<resMsg>(res.dump());
  }*/

  if (querys.find("datasetId") != querys.end()) {
    req["datasetId"] = atoi(querys.find("datasetId")->second.c_str());
  } else {
    res["code"] = -99;
    res["msg"] = "need datasetId";
    return std::make_shared<resMsg>(res.dump());
  }

  if (querys.find("imageId") != querys.end()) {
    req["imageId"] = atoi(querys.find("imageId")->second.c_str());
  } else {

    res["code"] = -99;
    res["msg"] = "need imageId";
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::GetTestResultInfoRequest request;
  TurboLogic::GetTestResultInfoResponse response;

  request.datasetId = req["datasetId"].get<int>();
  request.imageId = req["imageId"].get<int>();

  request.projectId = projectId;
  request.modelId = modelId;
  request.testId = testId;
  logic.model()->getTestResultInfo(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;

  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"]["heatmap"] = response.heatmap;
  res["data"]["isPreded"] = response.isPreded;
  res["data"]["instances"] = json::array();
  for (auto ins : response.items) {
    json detail;
    detail = {
        {"annotationType", int(ins.predictType)},
        {"area", ins.area},
        {"data", ins.data},
        {"width", ins.width},
        {"height", ins.height},
        {"score", ins.score},
        {"tagName", ins.tagName},
        {"x", ins.x},
        {"y", ins.y},
    };
    res["data"]["instances"].push_back(detail);
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
resMsg* ModelRouter::downloadModelTestResult(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
    */
DEFINITION_ROUTER(ModelRouter, downloadModelTestResult) {
  int projectId = 0;
  int modelId = 0;
  int testId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/models/%d/tests/%d/download_result",
             &projectId, &modelId, &testId);
  if (converted == 3) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;

  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  json schema = schemaOfRoot();
  auto downloadPath = schemaOfItem("string");
  schema["properties"]["downloadPath"] = downloadPath;
  schema["properties"]["downloadPath"]["minLength"] = 1;
  schema["required"].push_back("downloadPath");
  if (!jsonParseAndSchemaCheck(body, schema, req, res)) {
    return std::make_shared<resMsg>(res.dump());
  }
  TurboLogic::GetDownloadTestResultRequest request;
  TurboLogic::GetDownloadTestResultResponse response;
  request.downloadPath = req["downloadPath"].get<std::string>();

  request.projectId = projectId;
  request.modelId = modelId;
  request.testId = testId;
  logic.model()->downloadTestResult(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
}

/*
resMsg* ModelRouter::trainModel(std::string method, std::string host,
                                std::string proto, std::string path,
                                std::map<std::string, std::string> querys,
                                std::map<std::string, std::string> headers,
                                std::map<std::string, std::string> form,
                                std::string body)
*/
DEFINITION_ROUTER(ModelRouter, trainModel) {
  int projectId = 0;
  int modelId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/models/%d/train",
                         &projectId, &modelId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::TrainModelRequest request;
  request.projectId = projectId;
  request.modelId = modelId;
  TurboLogic::TrainModelResponse response;

  logic.model()->trainModel(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
resMsg* ModelRouter::getModelTrainBoard(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/

DEFINITION_ROUTER(ModelRouter, getModelTrainBoard) {
  int projectId = 0;
  int modelId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/models/%d/train_board",
                         &projectId, &modelId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::GetTrainBoardDataRequest request;
  request.projectId = projectId;
  request.modelId = modelId;
  TurboLogic::GetTrainBoardDataResponse response;
  logic.model()->getTrainBoardData(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  // std::cout << "request.datasetType: " << request.datasetType << std::endl;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"]["boardData"] = json::object();
  for (auto bd : response.boardDatas) {
    res["data"]["boardData"][bd.first] = json::array();
    for (auto data : bd.second) {
      json detail;
      detail = {
          {"time", data.time},     {"xName", data.xName},
          {"xValue", data.xValue}, {"yName", data.yName},
          {"yValue", data.yValue},
      };
      res["data"]["boardData"][bd.first].push_back(detail);
    }
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
resMsg* ModelRouter::getModelTrainLog(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body
*/
DEFINITION_ROUTER(ModelRouter, getModelTrainLog) {
  int projectId = 0;
  int modelId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/models/%d/train_log",
                         &projectId, &modelId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  /*std::vector<std::string> needEntryList;
  needEntryList.push_back("level");
  if (!parseBody(body, req, res, needEntryList)) {
      return std::make_shared<resMsg>(res.dump());
  }*/

  TurboLogic::GetTrainLogRequest request;
  TurboLogic::GetTrainLogResponse response;

  if (querys.find("level") != querys.end()) {
    req["level"] = atoi(querys.find("level")->second.c_str());
  } else {
    res["code"] = -99;
    res["msg"] = "need level";
    return std::make_shared<resMsg>(res.dump());
  }

  request.modelId = modelId;
  request.projectId = projectId;
  request.level = getLogLevel(req["level"].get<int>());
  logic.model()->getTrainLog(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;

  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"]["logs"] = json::array();
  for (auto log : response.logs) {
    json detail;
    detail = {{"content", log.content}, {"level", request.level},
              {"reason", log.reason},   {"suggestion", log.suggestion},
              {"time", log.time},       {"errorCode", log.errorCode}};
    res["data"]["logs"].push_back(detail);
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
resMsg* ModelRouter::getModelTrainProgress(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/

DEFINITION_ROUTER(ModelRouter, getModelTrainProgress) {
  int projectId = 0;
  int modelId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/models/%d/train_progress",
             &projectId, &modelId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::GetTrainProgressInfoRequest request;
  TurboLogic::GetTrainProgressInfoResponse response;

  request.modelId = modelId;
  request.projectId = projectId;
  logic.model()->getTrainProgressInfo(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  // std::cout << "request.datasetType: " << request.datasetType << std::endl;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = {{"duration", response.duration},
                 {"endTime", response.endTime},
                 {"progress", response.progress},
                 {"remainingTime", response.remainingTime},
                 {"startTime", response.startTime},
                 {"status", int(response.status)}};
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
resMsg* ModelRouter::getModelValResult(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/

DEFINITION_ROUTER(ModelRouter, getModelValResult) {
  int projectId = 0;
  int modelId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/models/%d/val_result",
                         &projectId, &modelId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  if (querys.find("datasetId") != querys.end()) {
    req["datasetId"] = atoi(querys.find("datasetId")->second.c_str());
  } else {
    res["code"] = -99;
    res["msg"] = "need datasetId";
    return std::make_shared<resMsg>(res.dump());
  }

  if (querys.find("imageId") != querys.end()) {
    req["imageId"] = atoi(querys.find("imageId")->second.c_str());
  } else {
    res["code"] = -99;
    res["msg"] = "need imageId";
    return std::make_shared<resMsg>(res.dump());
  }

  // std::vector<std::string> needEntryList;
  // needEntryList.push_back("datasetId");
  // needEntryList.push_back("imageId");
  // if (!parseBody(body, req, res, needEntryList)) {
  //     return std::make_shared<resMsg>(res.dump());
  // }

  TurboLogic::GetValResultInfoRequest request;
  TurboLogic::GetValResultInfoResponse response;

  request.datasetId = req["datasetId"].get<int>();
  request.imageId = req["imageId"].get<int>();

  request.projectId = projectId;
  request.modelId = modelId;
  logic.model()->getValResultInfo(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;

  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"]["heatmap"] = response.heatmap;
  res["data"]["isPreded"] = response.isPreded;
  res["data"]["instances"] = json::array();
  for (auto ins : response.items) {
    json detail;
    detail = {
        {"annotationType", int(ins.predictType)},
        {"area", ins.area},
        {"data", ins.data},
        {"width", ins.width},
        {"height", ins.height},
        {"score", ins.score},
        {"tagName", ins.tagName},
        {"x", ins.x},
        {"y", ins.y},
    };
    res["data"]["instances"].push_back(detail);
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
resMsg* ModelRouter::getGPUStatus(std::string method, std::string host,
                                  std::string proto, std::string path,
                                  std::map<std::string, std::string> querys,
                                  std::map<std::string, std::string> headers,
                                  std::map<std::string, std::string> form,
                                  std::string body)
*/

DEFINITION_ROUTER(ModelRouter, getGPUStatus) {
#if 0
    TurboLogic::Logic &logic = TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
    json res;
    res["data"] = json::object();
    res["code"] = TurboLogic::ErrorType::Success;
    res["msg"] = "";

    TurboLogic::GetGPUStatusResponse response;

    logic.model()->getGPUStatus(response);
    res["code"] = response.error.type;
    res["msg"] = response.error.message;
    //std::cout << "request.datasetType: " << request.datasetType << std::endl;
    if (response.error.type != TurboLogic::ErrorType::Success) {
        return std::make_shared<resMsg>(res.dump());
    }

    res["data"]["gpus"] = json::array();
    for (auto ins : response.gpus) {
        json detail;
        detail = {
            {"id", int(ins.id)},
            {"usable", ins.usable},
        };
        res["data"]["gpus"].push_back(detail);
    }
    std::string dumpData;
    dumpResponse(res, dumpData);
    return std::make_shared<resMsg>(res.dump());
#else
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::ErrUnknown;
  res["msg"] = "";

  TurboLogic::GetGPUStatusResponse response;
  logic.model()->getGPUStatus(response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"]["gpus"] = json::array();
  for (auto gpu : response.gpus) {
    json detail = {{"computeCapability", gpu.computeCapability},
                   {"id", gpu.id},
                   {"name", gpu.name},
                   {"usable", gpu.usable}};
    res["data"]["gpus"].push_back(detail);
  }
  std::string msg;
  try {
    msg = res.dump();
  } catch (const std::exception& e) {
    std::cerr << "parse json error: " << e.what() << std::endl;
    json j_c;
    j_c["code"] = TurboLogic::ErrorType::ErrUnknown;
    std::string errMsg = "pack response fail";
    j_c["msg"] = errMsg;
    j_c["data"] = json::array();
    return std::make_shared<resMsg>(j_c.dump());
  };
  return std::make_shared<resMsg>(res.dump());

#endif
};

/*
resMsg* ModelRouter::getModelTrainTags(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(ModelRouter, getModelTrainTags) {
  int projectId = 0;
  int modelId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/models/%d/train_tags",
                         &projectId, &modelId);

  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::GetModelTrainTagsRequest request;
  TurboLogic::GetModelTrainTagsResponse response;

  request.projectId = projectId;
  request.modelId = modelId;
  logic.model()->getModelTrainTags(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;

  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"]["tags"] = json::array();
  for (auto ins : response.tags) {
    json detail;
    detail = {
        {"id", int(ins.id)},
        {"name", ins.name},
    };
    res["data"]["tags"].push_back(detail);
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
resMsg* ModelRouter::getModelTestDataset(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(ModelRouter, getModelTestDataset) {
  int projectId = 0;
  int modelId = 0;
  int testId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/models/%d/tests/%d/test_datasets",
             &projectId, &modelId, &testId);

  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::GetModelTestRelatedDatasetRequest request;
  TurboLogic::GetModelTestRelatedDatasetResponse response;

  request.projectId = projectId;
  request.modelId = modelId;
  request.testId = testId;
  logic.model()->getModelTestRelatedDataset(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;

  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"]["datasets"] = json::array();

  auto func = [](const int& projectId, int& relationType,
                 std::vector<int>& datasetIds,
                 std::vector<std::string>& datasetUUIDs, json& info) {
    LogDebug(" size:{} {}", datasetIds.size(), datasetUUIDs.size());
    for (int i = 0; i < datasetIds.size(); i++) {
      TurboLogic::Logic& ilogic =
          TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
      TurboLogic::GetDatasetInfoRequest getInfoRequest;
      TurboLogic::GetDatasetInfoResponse getInfoResponse;
      getInfoRequest.datasetId = datasetIds[i];
      getInfoRequest.projectId = projectId;
      ilogic.dataset()->getDatasetInfo(getInfoRequest, getInfoResponse);
      json detail = json::object();
      detail = {{"datasetCreateTime", getInfoResponse.createTime},
                {"datasetDescription", getInfoResponse.datasetDescription},
                //{"datasetEditable", getInfoResponse.editable},
                {"isUsed", !getInfoResponse.editable},
                {"datasetId", getInfoResponse.datasetId},
                {"datasetModifiedTime", getInfoResponse.modifyTime},
                {"datasetName", getInfoResponse.datasetName},
                {"datasetType", getInfoResponse.datasetType},
                {"parentDatasetId", getInfoResponse.parentDatasetId},
                {"parentDatasetName", getInfoResponse.parentDatasetName},
                {"timestamp", datasetUUIDs[i]}};
      info.push_back(detail);
    }
  };

  int relationType = TurboLogic::ModelDatasetType::Test;
  func(projectId, relationType, response.datasetIds, response.datasetUUIDs,
       res["data"]["datasets"]);
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
resMsg* ModelRouter::checkModelUsedByPipeline(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(ModelRouter, checkModelUsedByPipeline) {
  int projectId = 0;
  int modelId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/models/%d/check",
                         &projectId, &modelId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::CheckModelUsedByPipelinneRequest request;
  TurboLogic::CheckModelUsedByPipelinneResponse response;

  request.modelId = modelId;
  request.projectId = projectId;
  logic.model()->checkUsedByPipeline(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  res["data"]["isUsed"] = response.isUsed;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
}

/*
resMsg* ModelRouter::getCPUStatus(std::string method, std::string host,
                                  std::string proto, std::string path,
                                  std::map<std::string, std::string> querys,
                                  std::map<std::string, std::string> headers,
                                  std::map<std::string, std::string> form,
                                  std::string body)
*/
DEFINITION_ROUTER(ModelRouter, getCPUStatus) {
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();

  res["code"] = TurboLogic::ErrorType::ErrUnknown;
  res["msg"] = "";

  TurboLogic::GetCPUStatusResponse response;
  logic.model()->getCPUStatus(response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;

  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"]["taskCount"] = response.taskCount;
  res["data"]["usageRate"] = response.usageRate;

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
resMsg* ModelRouter::getDevicesStatus(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/

DEFINITION_ROUTER(ModelRouter, getDevicesStatus) {
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();

  res["code"] = TurboLogic::ErrorType::ErrUnknown;
  res["msg"] = "";

  TurboLogic::GetDevicesStatusResponse response;

  logic.model()->getDevicesStatus(response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;

  res["data"]["cpu"] = json::object();
  res["data"]["gpus"] = json::array();

  res["data"]["cpu"]["usable"] = response.cpu.usable;
  for (auto iter = response.gpus.begin(); iter != response.gpus.end(); iter++) {
    auto gpu = json::object();
    gpu["computeCapability"] = iter->computeCapability;
    gpu["id"] = iter->id;
    gpu["name"] = iter->name;
    gpu["usable"] = iter->usable;
    res["data"]["gpus"].push_back(gpu);
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
}
/*
resMsg* ModelRouter::checkDeviceStatus(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/

DEFINITION_ROUTER(ModelRouter, checkDeviceStatus) {
  int projectId = 0;
  int converted = sscanf(
      path.c_str(), "/v1/projects/%d/pre_execute_check_device", &projectId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::ErrUnknown;
  res["msg"] = "";
  std::vector<std::string> needEntryList;
  needEntryList.push_back("id");
  needEntryList.push_back("type");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }
  TurboLogic::CheckDeviceStatusRequest request;
  TurboLogic::CheckDeviceStatusResponse response;
  request.projectId = projectId;
  request.id = req["id"];
  request.type = req["type"];
  logic.model()->checkDeviceStatus(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  res["data"]["usable"] = response.usable;
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
}
/*
resMsg* ModelRouter::checkModelDatasetStatus(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(ModelRouter, checkModelDatasetStatus) {
  int projectId = 0;
  int modelId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/models/%d/check_dataset",
             &projectId, &modelId);
  if (converted == 2) {
    std::cout << "projectId:" << projectId << " modelId:" << modelId
              << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::ErrUnknown;
  res["msg"] = "";

  std::vector<std::string> needEntryList;

  needEntryList.push_back("type");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::CheckModelDatasetStatusRequest request;
  TurboLogic::CheckModelDatasetStatusResponse response;
  request.projectId = projectId;
  request.id = modelId;
  request.type = "train";
  request.datasetType = req["type"];
  logic.model()->checkModelDataset(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  res["data"]["isChanged"] = response.isChanged;
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
}

/*
resMsg* ModelRouter::checkModelTestDatasetStatus(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(ModelRouter, checkModelTestDatasetStatus) {

  int projectId = 0;
  int modelId = 0;
  int testId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/models/%d/tests/%d/check_dataset",
             &projectId, &modelId, &testId);
  if (converted == 3) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::ErrUnknown;
  res["msg"] = "";

  TurboLogic::CheckModelDatasetStatusRequest request;
  TurboLogic::CheckModelDatasetStatusResponse response;
  request.projectId = projectId;
  request.id = testId;
  request.type = "test";
  logic.model()->checkModelDataset(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  res["data"]["isChanged"] = response.isChanged;
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
}
