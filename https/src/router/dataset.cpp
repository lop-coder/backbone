#include "dataset.h"

#include <iostream>
#include <nlohmann/json-schema.hpp>
#include <nlohmann/json.hpp>
#include "common.h"

#include "logger.h"
#include "logic.h"
#include "schema.h"

using namespace TurboHttp;
using nlohmann::json;
using nlohmann::json_schema::json_validator;
////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////
DatasetRouter::DatasetRouter() {
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/datasets)",
                  DatasetRouter::create);

  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/datasets/list)",
                  DatasetRouter::list);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/datasets/merge)",
                  DatasetRouter::merge);
  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/datasets/(\d+))",
                  DatasetRouter::get);
  REGISTER_ROUTER("PUT", R"(/v1/projects/(\d+)/datasets/(\d+))",
                  DatasetRouter::put);
  REGISTER_ROUTER("DELETE", R"(/v1/projects/(\d+)/datasets/(\d+))",
                  DatasetRouter::deleteDataset);

  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/datasets/(\d+)/duplicate)",
                  DatasetRouter::duplicate);

  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/datasets/(\d+)/images)",
                  DatasetRouter::listImages);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/datasets/(\d+)/images)",
                  DatasetRouter::addImages);
  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/datasets/(\d+)/images/(\d+))",
                  DatasetRouter::getImage);
  REGISTER_ROUTER("POST",
                  R"(/v1/projects/(\d+)/datasets/(\d+)/images/(\d+)/rotate)",
                  DatasetRouter::rotateImage);

  REGISTER_ROUTER(
      "POST", R"(/v1/projects/(\d+)/datasets/(\d+)/images/(\d+)/annotations)",
      DatasetRouter::addAnnotataions);
  REGISTER_ROUTER(
      "GET", R"(/v1/projects/(\d+)/datasets/(\d+)/images/(\d+)/annotations)",
      DatasetRouter::getAnnotataions);
  REGISTER_ROUTER(
      "PUT", R"(/v1/projects/(\d+)/datasets/(\d+)/images/(\d+)/annotations)",
      DatasetRouter::updateAnnotataions);
  REGISTER_ROUTER(
      "DELETE", R"(/v1/projects/(\d+)/datasets/(\d+)/images/(\d+)/annotations)",
      DatasetRouter::deleteAnnotataions);
  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/datasets/(\d+)/related_model)",
                  DatasetRouter::getRelatedModels);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/datasets/(\d+)/split)",
                  DatasetRouter::split);
  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/datasets/(\d+)/static)",
                  DatasetRouter::getStatistics);
  REGISTER_ROUTER(
      "DELETE",
      R"(/v1/projects/(\d+)/datasets/(\d+)/images/(\d+)/annotations/empty)",
      DatasetRouter::clearAnnotataions);
  REGISTER_ROUTER(
      "POST",
      R"(/v1/projects/(\d+)/datasets/(\d+)/images/(\d+)/annotations/bg)",
      DatasetRouter::setBgAnnotataions);
  REGISTER_ROUTER("DELETE", R"(/v1/projects/(\d+)/datasets/(\d+)/images)",
                  DatasetRouter::deleteImages);
  REGISTER_ROUTER("GET",
                  R"(/v1/projects/(\d+)/datasets/(\d+)/related_infer_model)",
                  DatasetRouter::getRelatedInferModels);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/datasets/(\d+)/restore)",
                  DatasetRouter::restore);
  REGISTER_ROUTER("POST",
                  R"(/v1/projects/(\d+)/datasets/(\d+)/cls_annotations)",
                  DatasetRouter::createClsAnnotations);

  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/datasets/(\d+)/check)",
                  DatasetRouter::checkDatasetUsedByPipeline);

  REGISTER_ROUTER("POST", R"(/v2/projects/(\d+)/datasets)",
                  DatasetRouter::createV2);
  REGISTER_ROUTER("POST", R"(/v2/projects/(\d+)/datasets/merge)",
                  DatasetRouter::mergeV2);
  REGISTER_ROUTER("POST", R"(/v2/projects/(\d+)/datasets/(\d+)/duplicate)",
                  DatasetRouter::duplicateV2);
  REGISTER_ROUTER("POST", R"(/v2/projects/(\d+)/datasets/(\d+)/images)",
                  DatasetRouter::addImagesV2);

  REGISTER_ROUTER("GET", R"(/v2/projects/(\d+)/datasets/(\d+)/tags)",
                  DatasetRouter::listTags);
  REGISTER_ROUTER("POST", R"(/v2/projects/(\d+)/datasets/(\d+)/tags)",
                  DatasetRouter::createTag);
  REGISTER_ROUTER("GET", R"(/v2/projects/(\d+)/datasets/(\d+)/tags/(\d+))",
                  DatasetRouter::getTag);
  REGISTER_ROUTER("PUT", R"(/v2/projects/(\d+)/datasets/(\d+)/tags/(\d+))",
                  DatasetRouter::updateTag);
  REGISTER_ROUTER("DELETE", R"(/v2/projects/(\d+)/datasets/(\d+)/tags/(\d+))",
                  DatasetRouter::deleteTag);
  REGISTER_ROUTER("POST",
                  R"(/v2/projects/(\d+)/datasets/(\d+)/tags/(\d+)/merge_to)",
                  DatasetRouter::mergeTag);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/datasets/(\d+)/image_samples)",
                  DatasetRouter::queryDatasetImageSamples);
  REGISTER_ROUTER("POST",
                  R"(/v1/projects/(\d+)/datasets/(\d+)/image_samples/save)",
                  DatasetRouter::saveDatasetImageSamples);
  REGISTER_ROUTER("POST",
                  R"(/v1/projects/(\d+)/datasets/(\d+)/images/crop_resize)",
                  DatasetRouter::cropReiszeDatasetImages);
  REGISTER_ROUTER(
      "POST",
      R"(/v1/projects/(\d+)/datasets/(\d+)/images/(\d+)/save_as_latestannotated)",
      DatasetRouter::SaveLatestAnnotatedImage);
  REGISTER_ROUTER("GET",
                  R"(/v1/projects/(\d+)/datasets/(\d+)/images/latestannotated)",
                  DatasetRouter::GetLatestAnnotatedImage);

  REGISTER_ROUTER("POST",
                  R"(/v1/projects/(\d+)/datasets/(\d+)/predictions/save)",
                  DatasetRouter::SaveDatasetPredictions);

  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/datasets/(\d+)/attributes)",
                  DatasetRouter::ListDatasetAttrs);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/datasets/(\d+)/attributes)",
                  DatasetRouter::AddDatasetAttrs);
  REGISTER_ROUTER("DELETE",
                  R"(/v1/projects/(\d+)/datasets/(\d+)/attributes/(\d+))",
                  DatasetRouter::DeleteDatasetAttrs);
  REGISTER_ROUTER("POST",
                  R"(/v1/projects/(\d+)/datasets/(\d+)/images/(\d+)/attribute)",
                  DatasetRouter::AddDatasetImageAttrs);
  REGISTER_ROUTER("DELETE",
                  R"(/v1/projects/(\d+)/datasets/(\d+)/images/(\d+)/attribute)",
                  DatasetRouter::DeleteDatasetImageAttrs);
  REGISTER_ROUTER("POST",
                  R"(/v1/projects/(\d+)/datasets/(\d+)/visualization/export)",
                  DatasetRouter::exportVisualization);
  REGISTER_ROUTER("POST",
                  R"(/v1/projects/(\d+)/datasets/(\d+)/visualization/cancel)",
                  DatasetRouter::cancelExportVisualization);
  REGISTER_ROUTER(
      "GET",
      R"(/v1/projects/(\d+)/datasets/(\d+)/visualization/export/progress)",
      DatasetRouter::getExportVisualizationProgress);

  REGISTER_ROUTER("POST",
                  R"(/v1/projects/(\d+)/datasets/(\d+)/annotations/export)",
                  DatasetRouter::exportAnnotation);
  REGISTER_ROUTER(
      "POST", R"(/v1/projects/(\d+)/datasets/(\d+)/annotations/export/cancel)",
      DatasetRouter::cancelExportAnnotation);
  REGISTER_ROUTER("GET",
                  R"(/v1/projects/(\d+)/datasets/(\d+)/annotations/"
                  "export/progress)",
                  DatasetRouter::getExportAnnotationProgress);
}
DatasetRouter::~DatasetRouter() {}
////////////////////////////////////////////////////////////////////////////////////////////////////
/*
std::shared_ptr<resMsg> DatasetRouter::create(std::string method, std::string host,
                              std::string proto, std::string path,
                              std::map<std::string, std::string> querys,
                              std::map<std::string, std::string> headers,std::vector<std::string> matches,
                              std::map<std::string, std::string> form,
                              std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, create) {
  int id = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/datasets", &id);
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
  json reqExample;
  reqExample = {{"datasetName", "dataset"},
                {"datasetType", 1},
                {"datasetDescription", "datasetDescription"}};

  std::vector<std::string> needEntryList;
  needEntryList.push_back("datasetName");
  needEntryList.push_back("datasetType");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }
  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<resMsg>(res.dump());
  }
  TurboLogic::CreateDatasetRequest request;
  TurboLogic::CreateDatasetResponse response;

  if (req.contains("datasetDescription")) {
    request.datasetDescription = req["datasetDescription"];
  } else {
    request.datasetDescription = "";
  }
  request.datasetName = req["datasetName"];
  request.datasetType = TurboLogic::AlgorithmType(req["datasetType"]);
  if (!(checkArg(request.datasetType, "AlgorithmType", res))) {
    return std::make_shared<resMsg>(res.dump());
  }

  request.projectId = id;
  logic.dataset()->createDataset(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  std::cout << "request.datasetType: " << request.datasetType << std::endl;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = {
      {"datasetCreateTime", response.createTime},
      {"datasetDescription", response.datasetDescription},
      //{"datasetEditable", response.editable},
      {"isUsed", !response.editable},
      {"datasetId", response.datasetId},
      {"datasetModifiedTime", response.modifyTime},
      {"datasetName", response.datasetName},
      {"datasetType", response.datasetType},
      {"parentDatasetId", response.parentDatasetId},
      {"parentDatasetName", response.parentDatasetName},
  };
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::list(std::string method, std::string host,
                            std::string proto, std::string path,
                            std::map<std::string, std::string> querys,
                            std::map<std::string, std::string> headers,std::vector<std::string> matches,
                            std::map<std::string, std::string> form,
                            std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, list) {
  int id = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/datasets/list", &id);
  if (converted == 1) {
    std::cout << "id:" << id << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  json reqExample;
  reqExample = {{"datasetName", "dataset"},
                {"datasetType", {1, 2}},
                {"limit", 0},
                {"offset", 0}};
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  std::vector<std::string> needEntryList;
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }
  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::ListDatasetsRequest request;
  TurboLogic::ListDatasetsResponse response;

  if (!(req.contains("datasetType")) || req["datasetType"].size() == 0) {
    req["datasetType"] = {
        TurboLogic::AlgorithmType::SingleLabelClassification,
        TurboLogic::AlgorithmType::Segmentation,
        TurboLogic::AlgorithmType::MutiLabelClassification,
        TurboLogic::AlgorithmType::Detection,
        TurboLogic::AlgorithmType::OCR,
    };
  }
  for (auto& element : req["datasetType"]) {
    if (!(checkArg(element, "AlgorithmType", res))) {
      return std::make_shared<resMsg>(res.dump());
    }
    request.datasetTypeList.push_back(TurboLogic::AlgorithmType(element));
  }

  if (req.contains("datasetName")) {
    request.datasetName = req["datasetName"];
  } else {
    request.datasetName = "";
  }
  if (req.contains("isLabeled")) {
    request.isLabeled = req["isLabeled"].get<int>();
  } else {
    request.isLabeled = 2;
  }

  if (req.contains("limit")) {
    request.limit = req["limit"].get<int>();
  } else {
    request.limit = 999;
  }

  if (req.contains("offset")) {
    request.offset = req["offset"].get<int>();
  } else {
    request.offset = 0;
  }

  request.projectId = id;
  logic.dataset()->listDatasets(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = json::object();
  res["data"]["datasets"] = json::array();
  res["data"]["total"] = response.total;
  for (auto dataset : response.items) {
    json detail;
    detail = {
        {"datasetCreateTime", dataset.createTime},
        {"datasetDescription", dataset.datasetDescription},
        //{"datasetEditable", dataset.editable},
        {"isUsed", !dataset.editable},
        {"datasetId", dataset.datasetId},
        {"datasetModifiedTime", dataset.modifyTime},
        {"datasetName", dataset.datasetName},
        {"datasetType", dataset.datasetType},
        {"parentDatasetId", dataset.parentDatasetId},
        {"parentDatasetName", dataset.parentDatasetName},
        {"isLabeled", dataset.isLabeled},
        {"timestamp", dataset.timeStamp},
    };
    res["data"]["datasets"].push_back(detail);
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::merge(std::string method, std::string host,
                             std::string proto, std::string path,
                             std::map<std::string, std::string> querys,
                             std::map<std::string, std::string> headers,std::vector<std::string> matches,
                             std::map<std::string, std::string> form,
                             std::string body)
                             */
DEFINITION_ROUTER(DatasetRouter, merge) {
  int id = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/datasets/merge", &id);
  if (converted == 1) {
    std::cout << "id:" << id << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  json reqExample;
  reqExample = {
      {"datasetName", "dataset"}, {"datasetIds", {1, 2}}, {"tagIds", {1, 2}}};
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  std::vector<std::string> needEntryList;
  needEntryList.push_back("datasetName");
  needEntryList.push_back("datasetIds");
  needEntryList.push_back("tagIds");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::MergeDatasetRequest request;
  TurboLogic::MergeDatasetResponse response;

  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<resMsg>(res.dump());
  }

  if (req.contains("datasetIds")) {
    for (auto& element : req["datasetIds"]) {
      request.targetDatasetIds.push_back(element);
    }
  }

  if (req.contains("tagNames")) {
    for (auto& element : req["tagNames"]) {
      request.tagNames.push_back(element);
    }
  }

  request.projectId = id;
  request.datasetName = req["datasetName"];
  logic.dataset()->mergeDataset(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = {{"datasetCreateTime", response.dataset.createTime},
                 {"datasetDescription", response.dataset.datasetDescription},
                 //{"datasetEditable", response.dataset.editable},
                 {"isUsed", !response.dataset.editable},
                 {"datasetId", response.dataset.datasetId},
                 {"datasetModifiedTime", response.dataset.modifyTime},
                 {"datasetName", response.dataset.datasetName},
                 {"datasetType", response.dataset.datasetType},
                 {"parentDatasetId", response.dataset.parentDatasetId},
                 {"parentDatasetName", response.dataset.parentDatasetName}};

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::get(std::string method, std::string host,
                           std::string proto, std::string path,
                           std::map<std::string, std::string> querys,
                           std::map<std::string, std::string> headers,std::vector<std::string> matches,
                           std::map<std::string, std::string> form,
                           std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, get) {
  int projectId = 0;
  int datasetId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/datasets/%d",
                         &projectId, &datasetId);
  if (converted == 1) {
    std::cout << "datasetId:" << datasetId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::GetDatasetInfoRequest request;
  TurboLogic::GetDatasetInfoResponse response;

  request.projectId = projectId;
  request.datasetId = datasetId;

  logic.dataset()->getDatasetInfo(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = {
      {"datasetCreateTime", response.createTime},
      {"datasetDescription", response.datasetDescription},
      //{"datasetEditable", response.editable},
      {"isUsed", !response.editable},
      {"datasetId", response.datasetId},
      {"datasetModifiedTime", response.modifyTime},
      {"datasetName", response.datasetName},
      {"datasetType", response.datasetType},
      {"parentDatasetId", response.parentDatasetId},
      {"parentDatasetName", response.parentDatasetName},
      {"timestamp", response.timeStamp},
  };
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
std::shared_ptr<resMsg> DatasetRouter::put(std::string method, std::string host,
                           std::string proto, std::string path,
                           std::map<std::string, std::string> querys,
                           std::map<std::string, std::string> headers,std::vector<std::string> matches,
                           std::map<std::string, std::string> form,
                           std::string body)
                           */
DEFINITION_ROUTER(DatasetRouter, put) {
  int projectId = 0;
  int datasetId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/datasets/%d",
                         &projectId, &datasetId);
  if (converted == 1) {
    std::cout << "datasetId:" << datasetId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";
  json reqExample = {{"datasetName", "dataset"},
                     {"datasetDescription", "datasetDescription"}};

  std::vector<std::string> needEntryList;
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }
  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::UpdateDatasetInfoRequest request;
  TurboLogic::UpdateDatasetInfoResponse response;
  TurboLogic::GetDatasetInfoRequest getDatasetInfoRequest;
  TurboLogic::GetDatasetInfoResponse getDatasetInfoResponse;

  getDatasetInfoRequest.datasetId = datasetId;
  getDatasetInfoRequest.projectId = projectId;
  logic.dataset()->getDatasetInfo(getDatasetInfoRequest,
                                  getDatasetInfoResponse);
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  // �Ȳ�ѯ����� ȱʡ
  if (req.contains("datasetDescription")) {
    request.datasetDescription = req["datasetDescription"];
  } else {
    request.datasetDescription = getDatasetInfoResponse.datasetDescription;
  }

  if (req.contains("datasetName")) {
    request.datasetName = req["datasetName"];
  } else {
    request.datasetName = getDatasetInfoResponse.datasetName;
  }

  request.projectId = projectId;
  request.datasetId = datasetId;
  logic.dataset()->updateDatasetInfo(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = {
      {"datasetCreateTime", response.dataset.createTime},
      {"datasetDescription", response.dataset.datasetDescription},
      //{"datasetEditable", response.dataset.editable},
      {"isUsed", !response.dataset.editable},
      {"datasetId", response.dataset.datasetId},
      {"datasetModifiedTime", response.dataset.modifyTime},
      {"datasetName", response.dataset.datasetName},
      {"datasetType", response.dataset.datasetType},
      {"parentDatasetId", response.dataset.parentDatasetId},
      {"parentDatasetName", response.dataset.parentDatasetName},
  };
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::deleteDataset(std::string method, std::string host,
                                     std::string proto, std::string path,
                                     std::map<std::string, std::string> querys,
                                     std::map<std::string, std::string> headers,std::vector<std::string> matches,
                                     std::map<std::string, std::string> form,
                                     std::string body)

*/
DEFINITION_ROUTER(DatasetRouter, deleteDataset) {
  int projectId = 0;
  int datasetId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/datasets/%d",
                         &projectId, &datasetId);
  if (converted == 1) {
    std::cout << "datasetId:" << datasetId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::DeleteDatasetRequest request;
  TurboLogic::DeleteDatasetResponse response;

  request.datasetId = datasetId;
  request.projectId = projectId;

  logic.dataset()->deleteDataset(request, response);
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
std::shared_ptr<resMsg> DatasetRouter::duplicate(std::string method, std::string host,
                                 std::string proto, std::string path,
                                 std::map<std::string, std::string> querys,
                                 std::map<std::string, std::string> headers,std::vector<std::string> matches,
                                 std::map<std::string, std::string> form,
                                 std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, duplicate) {
  int projectId = 0;
  int datasetId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/duplicate",
                         &projectId, &datasetId);
  if (converted == 1) {
    std::cout << "datasetId:" << datasetId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  json reqExample;
  reqExample = {{"datasetName", "dataset"}, {"tagIds", {1, 2}}};
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  std::vector<std::string> needEntryList;
  needEntryList.push_back("datasetName");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::DuplicateDatasetRequest request;
  TurboLogic::DuplicateDatasetResponse response;

  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<resMsg>(res.dump());
  }

  if (!(req.contains("tagIds")) || req["tagIds"].size() == 0) {
    req["tagIds"] = {};
  }
  for (auto& element : req["tagIds"]) {
    if (!(checkArg(element, "Integer", res))) {
      return std::make_shared<resMsg>(res.dump());
    }
    request.tagIds.push_back(element);
  }

  request.projectId = projectId;
  request.datasetId = datasetId;
  request.datasetName = req["datasetName"];
  request.description = "";
  logic.dataset()->duplicateDataset(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = {
      {"datasetCreateTime", response.dataset.createTime},
      {"datasetDescription", response.dataset.datasetDescription},
      //{"datasetEditable", response.dataset.editable},
      {"isUsed", !response.dataset.editable},
      {"datasetId", response.dataset.datasetId},
      {"datasetModifiedTime", response.dataset.modifyTime},
      {"datasetName", response.dataset.datasetName},
      {"datasetType", response.dataset.datasetType},
      {"parentDatasetId", response.dataset.parentDatasetId},
      {"parentDatasetName", response.dataset.parentDatasetName},
  };
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::listImages(std::string method, std::string host,
                                  std::string proto, std::string path,
                                  std::map<std::string, std::string> querys,
                                  std::map<std::string, std::string> headers,std::vector<std::string> matches,
                                  std::map<std::string, std::string> form,
                                  std::string body)
 */
DEFINITION_ROUTER(DatasetRouter, listImages) {
  int projectId = 0;
  int datasetId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/images",
                         &projectId, &datasetId);
  if (converted == 1) {
    std::cout << "datasetId:" << datasetId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  json reqExample;
  reqExample = {
      {"datasetName", "dataset"}, {"tagIds", {1, 2}}, {"datasetIds", {1, 2}}};
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";
  TurboLogic::GetAllImagesRequest request;
  TurboLogic::GetAllImagesResponse response;
  request.offset = 0;
  request.limit = 20;
  request.order_by = 0;
  request.filter = "";
  if (querys.find("offset") != querys.end()) {
    if (!checkArg(querys.find("offset")->second, "DigitalStr", res)) {
      return std::make_shared<resMsg>(res.dump());
    }
    request.offset = atoi(querys.find("offset")->second.c_str());
  }

  if (querys.find("limit") != querys.end()) {
    if (!checkArg(querys.find("limit")->second, "DigitalStr", res)) {
      return std::make_shared<resMsg>(res.dump());
    }
    request.limit = atoi(querys.find("limit")->second.c_str());
  }
  if (querys.find("order_by") != querys.end()) {
    if (!checkArg(querys.find("order_by")->second, "DigitalStr", res)) {
      return std::make_shared<resMsg>(res.dump());
    }
    request.order_by = atoi(querys.find("order_by")->second.c_str());
  }
  if (querys.find("filter") != querys.end()) {
    request.filter = querys.find("filter")->second;
  }
  request.projectId = projectId;
  request.datasetId = datasetId;
  logic.dataset()->getAllImages(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }
  res["data"] = json::object();
  res["data"]["pageImages"] = json::array();
  res["data"]["total"] = response.total;
  for (auto imageInfo : response.imageInfos) {
    json perImage = json::object();
    perImage["imageCreateTime"] = imageInfo.image.createTime;
    perImage["imageDescription"] = imageInfo.image.description;
    perImage["imageId"] = imageInfo.image.id;
    perImage["imageModifiedTime"] = imageInfo.image.modifyTime;
    perImage["imageName"] = imageInfo.image.name;
    perImage["imagePath"] = imageInfo.image.path;
    perImage["thumbnailPath"] = imageInfo.image.thumbnailPath;
    perImage["annotations"] = json::array();
    perImage["imageAttrs"] = json::array();
    for (auto annotation : imageInfo.annotations) {
      json perAnnotation = {
          {"annotationId", annotation.annotationId},
          {"annotationCreateTime", annotation.createdTime},
          {"annotationData", annotation.data},
          {"annotationModifiedTime", annotation.modifiedTime},
          {"annotationType", annotation.annotationType},
          {"tagId", annotation.tagId},
          {"tagName", annotation.tagName},
          {"height", annotation.height},
          {"width", annotation.width},
          {"x", annotation.xMin},
          {"y", annotation.yMin},
          {"area", annotation.area},
      };
      perImage["annotations"].push_back(perAnnotation);
    }
    for (auto attr : imageInfo.attrs) {
      json attrs = {
          {"imageAttrId", attr.id},
          {"imageAttrName", attr.name},
      };
      perImage["imageAttrs"].push_back(attrs);
    }
    res["data"]["pageImages"].push_back(perImage);
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
std::shared_ptr<resMsg> DatasetRouter::addImages(std::string method, std::string host,
                                 std::string proto, std::string path,
                                 std::map<std::string, std::string> querys,
                                 std::map<std::string, std::string> headers,std::vector<std::string> matches,
                                 std::map<std::string, std::string> form,
                                 std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, addImages) {
  int projectId = 0;
  int datasetId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/images",
                         &projectId, &datasetId);
  if (converted == 1) {
    std::cout << "datasetId:" << datasetId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  json reqExample;
  reqExample = {{"imagesMetaInfo", "dataset"},
                {"imagesPath", {"imagesPath1"}},
                {"isLoadAnn", true},
                {"timeStamp", "timeStamp"}};
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  std::vector<std::string> needEntryList;
  needEntryList.push_back("imagesPath");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::AddImageRequest request;
  TurboLogic::AddImageResponse response;

  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<resMsg>(res.dump());
  }

  if (req.contains("imagesMetaInfo")) {
    request.imageDescription = req["imagesMetaInfo"];
  } else {
    request.imageDescription = "";
  }
  if (req.contains("timeStamp")) {
    request.timeStamp = req["timeStamp"];
  } else {
    request.timeStamp = "";
  }

  if (req.contains("imagesPath")) {
    for (auto& element : req["imagesPath"]) {
      if (!checkArg(element, "String", res)) {
        return std::make_shared<resMsg>(res.dump());
      }
      request.imagesPath.push_back(element);
    }
  }
  request.isLoadAnn = false;

  if (req.contains("isLoadAnn")) {
    request.isLoadAnn = req["isLoadAnn"].get<bool>();
  }

  request.projectId = projectId;
  request.datasetId = datasetId;

  logic.dataset()->addImage(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  // if (response.error.type != TurboLogic::ErrorType::Success) {
  //   return std::make_shared<resMsg>(res.dump());
  // }

  res["data"]["images"] = json::array();
  for (auto error : response.imagesError) {
    json detail;
    detail = {
        {"code", error.type},
        {"msg", error.message},
    };
    res["data"]["images"].push_back(detail);
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::deleteImages(std::string method, std::string host,
                                    std::string proto, std::string path,
                                    std::map<std::string, std::string> querys,
                                    std::map<std::string, std::string> headers,std::vector<std::string> matches,
                                    std::map<std::string, std::string> form,
                                    std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, deleteImages) {
  int projectId = 0;
  int datasetId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/images",
                         &projectId, &datasetId);
  if (converted == 1) {
    std::cout << "datasetId:" << datasetId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  json reqExample;
  reqExample = {{"imageIds", {1, 2}}};
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  std::vector<std::string> needEntryList;
  needEntryList.push_back("imageIds");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::DeleteImagesRequest request;
  TurboLogic::DeleteImagesResponse response;

  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<resMsg>(res.dump());
  }

  if (req.contains("imageIds")) {
    for (auto& element : req["imageIds"]) {
      if (!checkArg(element, "Integer", res)) {
        return std::make_shared<resMsg>(res.dump());
      }
      request.imageIds.push_back(element);
    }
  }

  request.projectId = projectId;
  request.datasetId = datasetId;
  logic.dataset()->deleteImages(request, response);
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
std::shared_ptr<resMsg> DatasetRouter::getImage(std::string method, std::string host,
                                std::string proto, std::string path,
                                std::map<std::string, std::string> querys,
                                std::map<std::string, std::string> headers,std::vector<std::string> matches,
                                std::map<std::string, std::string> form,
                                std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, getImage) {
  int projectId = 0;
  int datasetId = 0;
  int imageId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/images/%d",
                         &projectId, &datasetId, &imageId);
  if (converted == 1) {
    std::cout << "datasetId:" << datasetId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;

  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::GetImageRequest request;
  TurboLogic::GetImageResponse response;

  request.projectId = projectId;
  request.imageId = imageId;
  request.datasetId = datasetId;
  logic.dataset()->getImage(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = json::object();
  res["data"]["annotations"] = json::array();
  res["data"]["imageAttrs"] = json::array();
  for (auto annotation : response.imageInfo.annotations) {
    json perAnnotation = {
        {"annotationId", annotation.annotationId},
        {"annotationCreateTime", annotation.createdTime},
        {"annotationData", annotation.data},
        {"annotationModifiedTime", annotation.modifiedTime},
        {"annotationType", annotation.annotationType},
        {"tagId", annotation.tagId},
        {"tagName", annotation.tagName},
        {"height", annotation.height},
        {"width", annotation.width},
        {"x", annotation.xMin},
        {"y", annotation.yMin},
        {"area", annotation.area},
    };
    res["data"]["annotations"].push_back(perAnnotation);
  }

  for (auto attr : response.imageInfo.attrs) {
    json attrs = {
        {"imageAttrId", attr.id},
        {"imageAttrName", attr.name},
    };
    res["data"]["imageAttrs"].push_back(attrs);
  }

  res["data"]["imageCreateTime"] = response.imageInfo.image.createTime;
  res["data"]["imageDescription"] = response.imageInfo.image.description;
  res["data"]["imageId"] = request.imageId;
  res["data"]["imageModifiedTime"] = response.imageInfo.image.modifyTime;
  res["data"]["imageName"] = response.imageInfo.image.name;
  res["data"]["imagePath"] = response.imageInfo.image.path;
  res["data"]["thumbnailPath"] = response.imageInfo.image.thumbnailPath;
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::rotateImage(std::string method, std::string host,
                                   std::string proto, std::string path,
                                   std::map<std::string, std::string> querys,
                                   std::map<std::string, std::string> headers,std::vector<std::string> matches,
                                   std::map<std::string, std::string> form,
                                   std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, rotateImage) {
  int projectId = 0;
  int datasetId = 0;
  int imageId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/images/%d/rotate",
             &projectId, &datasetId, &imageId);
  if (converted == 1) {
    std::cout << "datasetId:" << datasetId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  json reqExample;
  reqExample = {{"clockwiseQuadrantNum", 0}};
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  std::vector<std::string> needEntryList;
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::RotateImageRequest request;
  TurboLogic::RotateImageResponse response;

  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<resMsg>(res.dump());
  }

  request.projectId = projectId;
  request.datasetId = datasetId;
  request.imageId = imageId;
  request.nClockwiseQuadrant = 0;
  if (req.contains("clockwiseQuadrantNum")) {
    if (!(checkArg(req["clockwiseQuadrantNum"], "Integer", res))) {
      return std::make_shared<resMsg>(res.dump());
    }
    request.nClockwiseQuadrant = req["clockwiseQuadrantNum"];
  }
  std::cout << "request.nClockwiseQuadrant : " << request.nClockwiseQuadrant
            << std::endl;
  logic.image()->rotateImage(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = json::object();
  res["data"]["annotations"] = json::array();
  for (auto annotation : response.annotations) {
    json detail;
    detail = {
        {"annotationCreateTime", annotation.createdTime},
        {"annotationData", annotation.data},
        {"annotationId", annotation.annotationId},
        {"annotationModifiedTime", annotation.modifiedTime},
        {"annotationType", annotation.annotationType},
        {"tagId", annotation.tagId},
    };
    res["data"]["annotations"].push_back(detail);
  }
  res["data"]["imageCreateTime"] = response.image.createTime;
  res["data"]["imageDescription"] = response.image.description;
  res["data"]["imageId"] = response.image.id;
  res["data"]["imageModifiedTime"] = response.image.modifyTime;
  res["data"]["imageName"] = response.image.name;
  res["data"]["imagePath"] = response.image.path;
  res["data"]["thumbnailPath"] = response.image.thumbnailPath;
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::addAnnotataions(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,std::vector<std::string> matches,
    std::map<std::string, std::string> form, std::string body)

*/
DEFINITION_ROUTER(DatasetRouter, addAnnotataions) {
  int projectId = 0;
  int datasetId = 0;
  int imageId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/images/%d/annotations",
             &projectId, &datasetId, &imageId);
  if (converted == 1) {
    std::cout << "datasetId:" << datasetId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  json reqExample;
  json reqItem = {{"annotationData", "annotationData"}, {"tagId", 0}};
  reqExample = {{"annotations", {reqItem}}};
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  std::vector<std::string> needEntryList;
  needEntryList.push_back("annotations");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }

  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<resMsg>(res.dump());
  }
  std::vector<TurboLogic::CreateAnnotationRequest> requests;
  for (json item : req["annotations"]) {
    TurboLogic::CreateAnnotationRequest request;
    if (!checkRequest(item, reqItem, res)) {
      return std::make_shared<resMsg>(res.dump());
    }
    request.datasetId = datasetId;
    request.data = item["annotationData"];
    request.imageId = imageId;
    request.projectId = projectId;
    request.tagId = item["tagId"];
    requests.push_back(request);
  }

  res["data"] = json::object();
  res["data"]["annotationsInfo"] = json::array();
  for (TurboLogic::CreateAnnotationRequest request : requests) {
    TurboLogic::CreateAnnotationResponse response;
    logic.annotation()->createAnnotation(request, response);
    res["code"] = response.error.type;
    res["msg"] = response.error.message;
    if (response.error.type != TurboLogic::ErrorType::Success) {
      return std::make_shared<resMsg>(res.dump());
    }
    json detail;
    detail = {
        {"annotationCreateTime", response.annotation.createdTime},
        {"annotationData", response.annotation.data},
        {"annotationId", response.annotation.annotationId},
        {"annotationModifiedTime", response.annotation.modifiedTime},
        {"annotationType", response.annotation.annotationType},
        {"tagId", response.annotation.tagId},
        {"height", response.annotation.height},
        {"width", response.annotation.width},
        {"x", response.annotation.xMin},
        {"y", response.annotation.yMin},
        {"area", response.annotation.area},
    };
    res["data"]["annotationsInfo"].push_back(detail);
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::getAnnotataions(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,std::vector<std::string> matches,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, getAnnotataions) {
  int projectId = 0;
  int datasetId = 0;
  int imageId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/images/%d/annotations",
             &projectId, &datasetId, &imageId);
  if (converted == 1) {
    std::cout << "datasetId:" << datasetId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  json reqExample;
  reqExample = {
      {"datasetName", "dataset"}, {"tagIds", {1, 2}}, {"datasetIds", {1, 2}}};
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::GetImageAnnotationsRequest request;
  TurboLogic::GetImageAnnotationsResponse response;

  request.projectId = projectId;
  request.imageId = imageId;
  request.datasetId = datasetId;
  logic.annotation()->getImageAnnotations(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = json::object();
  res["data"]["annotations"] = json::array();
  for (auto annotation : response.annotations) {
    json detail;
    detail = {
        {"annotationCreateTime", annotation.createdTime},
        {"annotationData", annotation.data},
        {"annotationId", annotation.annotationId},
        {"annotationModifiedTime", annotation.modifiedTime},
        {"annotationType", annotation.annotationType},
        {"tagId", annotation.tagId},
        {"height", annotation.height},
        {"width", annotation.width},
        {"x", annotation.xMin},
        {"y", annotation.yMin},
        {"area", annotation.area},

    };
    res["data"]["annotations"].push_back(detail);
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
std::shared_ptr<resMsg> DatasetRouter::updateAnnotataions(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,std::vector<std::string> matches,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, updateAnnotataions) {
  int projectId = 0;
  int datasetId = 0;
  int imageId = 0;
  int annotationId = 0;
  int converted = sscanf(path.c_str(),
                         "/v1/projects/%d/datasets/%d/images/%d/annotations/%d",
                         &projectId, &datasetId, &imageId, &annotationId);
  if (converted == 1) {
    std::cout << "datasetId:" << datasetId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  json reqItem = {
      {"annotationData", "annotationData"}, {"annotationId", 1}, {"tagId", 2}};
  json reqExample;
  reqExample = {{"annotations", {reqItem}}};
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  std::vector<std::string> needEntryList;
  needEntryList.push_back("annotations");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }

  std::vector<TurboLogic::UpdateAnnotationDataRequest> requests;
  for (json item : req["annotations"]) {
    TurboLogic::UpdateAnnotationDataRequest request;
    if (!checkRequest(item, reqItem, res)) {
      return std::make_shared<resMsg>(res.dump());
    }
    request.annotationId = item["annotationId"];
    request.data = item["annotationData"];
    request.projectId = projectId;
    request.tagId = item["tagId"];
    requests.push_back(request);
  }

  res["data"] = json::object();
  res["data"]["annotationsInfo"] = json::array();
  for (TurboLogic::UpdateAnnotationDataRequest request : requests) {
    TurboLogic::UpdateAnnotationDataResponse response;
    logic.annotation()->updateAnnotationData(request, response);
    res["code"] = response.error.type;
    res["msg"] = response.error.message;
    if (response.error.type != TurboLogic::ErrorType::Success) {
      return std::make_shared<resMsg>(res.dump());
    }
    json detail;
    detail = {
        {"annotationCreateTime", response.annotation.createdTime},
        {"annotationData", response.annotation.data},
        {"annotationId", response.annotation.annotationId},
        {"annotationModifiedTime", response.annotation.modifiedTime},
        {"annotationType", response.annotation.annotationType},
        {"tagId", response.annotation.tagId},
        {"height", response.annotation.height},
        {"width", response.annotation.width},
        {"x", response.annotation.xMin},
        {"y", response.annotation.yMin},
        {"area", response.annotation.area},
    };
    res["data"]["annotationsInfo"].push_back(detail);
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::deleteAnnotataions(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,std::vector<std::string> matches,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, deleteAnnotataions) {
  int projectId = 0;
  int datasetId = 0;
  int imageId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/images/%d/annotations",
             &projectId, &datasetId, &imageId);
  if (converted == 1) {
    std::cout << "datasetId:" << datasetId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  json reqExample;
  reqExample = {{"annotationIds", {1, 2}}};
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  std::vector<std::string> needEntryList;
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::DeleteAnnotationsRequest request;
  TurboLogic::DeleteAnnotationsResponse response;

  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<resMsg>(res.dump());
  }
  if (!(req.contains("annotationIds")) || req["annotationIds"].size() == 0) {
    req["annotationIds"] = {};
  }
  for (auto& element : req["annotationIds"]) {
    if (!(checkArg(element, "Integer", res))) {
      return std::make_shared<resMsg>(res.dump());
    }
    request.annotationIds.push_back(element);
  }

  request.projectId = projectId;
  request.imageId = imageId;
  logic.annotation()->deleteAnnotations(request, response);
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
std::shared_ptr<resMsg> DatasetRouter::getRelatedModels(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,std::vector<std::string> matches,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, getRelatedModels) {
  int projectId = 0;
  int datasetId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/related_model",
             &projectId, &datasetId);
  if (converted == 1) {
    std::cout << "datasetId:" << datasetId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  json reqExample;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::GetDatasetRelatedModelsRequest request;
  TurboLogic::GetDatasetRelatedModelsResponse response;

  request.projectId = projectId;
  request.datasetId = datasetId;
  // todo
  request.limit = 9999;
  request.offset = 0;
  logic.dataset()->getDatasetRelatedModels(request, response);
  std::cout << "======== getDatasetRelatedInferenceModels =========="
            << std::endl;
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = json::object();
  res["data"]["models"] = json::array();
  for (auto model : response.models) {
    TurboLogic::GetModelInfoRequest getInfoRequest;
    TurboLogic::GetModelInfoResponse getInfoResponse;

    getInfoRequest.modelId = model.modelId;
    getInfoRequest.projectId = projectId;

    logic.model()->getModelInfo(getInfoRequest, getInfoResponse);
    if (getInfoResponse.error.type != TurboLogic::ErrorType::Success) {
      res["data"] = json::object();
      return std::make_shared<resMsg>(res.dump());
    }

    json detail = json::object();
    detail["modelInfo"] = {
        {"modelConfig", getInfoResponse.modelConfig},
        {"modelCreateTime", getInfoResponse.createTime},
        {"modelDescription", getInfoResponse.modelDescription},
        {"modelEditable", getInfoResponse.editable},
        {"modelId", model.modelId},
        {"modelModifiedTime", getInfoResponse.modifyTime},
        {"modelName", getInfoResponse.modelName},
        {"modelStatus", getInfoResponse.trainStatus},
        {"algorithmType", getInfoResponse.algorithmType},
    };
    detail["modelInfo"]["modelTests"] = json::array();
    detail["relationType"] = model.datasetType;
    if (model.datasetType != TurboLogic::Test) {
      res["data"]["models"].push_back(detail);
      continue;
    }
    std::cout << "model.datasetType: " << model.datasetType << std::endl;
    TurboLogic::ListModelTestsRequest listModelTestsRequest;
    TurboLogic::ListModelTestsResponse listModelTestsResponse;
    listModelTestsRequest.projectId = projectId;
    listModelTestsRequest.modelId = model.modelId;
    logic.model()->listModelTests(listModelTestsRequest,
                                  listModelTestsResponse);
    if (listModelTestsResponse.error.type != TurboLogic::ErrorType::Success) {
      res["data"] = json::object();
      return std::make_shared<resMsg>(res.dump());
    }
    for (auto modeltest : listModelTestsResponse.modelTests) {
      json modeltestdetail;
      modeltestdetail = {
          {"modelTestId", modeltest.modelTestId},
          {"testStatus", modeltest.testStatus},
          {"testVersion", modeltest.testVersion},
      };
      detail["modelInfo"]["modelTests"].push_back(modeltestdetail);
    }
    res["data"]["models"].push_back(detail);
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::getRelatedInferModels(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,std::vector<std::string> matches,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, getRelatedInferModels) {
  int projectId = 0;
  int datasetId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/related_infer_model",
             &projectId, &datasetId);
  if (converted == 1) {
    std::cout << "datasetId:" << datasetId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  json reqExample;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::GetDatasetRelatedInferenceModelsRequest request;
  TurboLogic::GetDatasetRelatedInferenceModelsResponse response;

  request.projectId = projectId;
  request.datasetId = datasetId;
  logic.dataset()->getDatasetRelatedInferenceModels(request, response);
  std::cout << "======== getDatasetRelatedInferenceModels =========="
            << std::endl;
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = json::object();
  res["data"]["models"] = json::array();
  for (auto model : response.vals) {
    json detail = json::object();
    detail = {
        {"modelId", model.modelId},
        {"modelName", model.modelName},
        {"modelStatus", model.modelStatus},
    };
    res["data"]["models"].push_back(detail);
  }
  res["data"]["modelTests"] = json::array();
  for (auto modelTest : response.tests) {
    json detail = json::object();
    detail = {
        {"modelId", modelTest.modelId},
        {"modelName", modelTest.modelName},
        {"modelTestId", modelTest.modelTestId},
        {"testVersion", modelTest.testVersion},
        {"modelTestStatus", modelTest.modelTestStatus},
    };
    res["data"]["modelTests"].push_back(detail);
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::split(std::string method, std::string host,
                             std::string proto, std::string path,
                             std::map<std::string, std::string> querys,
                             std::map<std::string, std::string> headers,std::vector<std::string> matches,
                             std::map<std::string, std::string> form,
                             std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, split)

{
  int projectId = 0;
  int datasetId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/split",
                         &projectId, &datasetId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  json reqExample;
  reqExample = {{"splitRatio", 0.1},
                {"testDatasetName", "testDatasetName"},
                {"trainDatasetName", "trainDatasetName"}};
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  std::vector<std::string> needEntryList;
  needEntryList.push_back("splitRatio");
  needEntryList.push_back("testDatasetName");
  needEntryList.push_back("trainDatasetName");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::SplitDatasetRequest request;
  TurboLogic::SplitDatasetResponse response;

  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<resMsg>(res.dump());
  }

  request.projectId = projectId;
  request.splitRatio = req["splitRatio"];
  request.targetDatasetId = datasetId;
  request.testDatasetDesc = "";
  request.trainingDatasetDesc = "";
  request.testDatasetName = req["testDatasetName"];
  request.trainingDatasetName = req["trainDatasetName"];
  logic.dataset()->splitDataset(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = json::object();
  res["data"]["testDataset"] = {
      {"datasetCreateTime", response.testDataset.createTime},
      {"datasetDescription", response.testDataset.datasetDescription},
      //{"datasetEditable", response.testDataset.editable},
      {"isUsed", !response.testDataset.editable},
      {"datasetId", response.testDataset.datasetId},
      {"datasetModifiedTime", response.testDataset.modifyTime},
      {"datasetName", response.testDataset.datasetName},
      {"datasetType", response.testDataset.datasetType},
      {"parentDatasetId", response.testDataset.parentDatasetId},
      {"parentDatasetName", response.testDataset.parentDatasetName}};
  res["data"]["trainDataset"] = {
      {"datasetCreateTime", response.trainingDataset.createTime},
      {"datasetDescription", response.trainingDataset.datasetDescription},
      //{"datasetEditable", response.trainingDataset.editable},
      {"isUsed", !response.trainingDataset.editable},
      {"datasetId", response.trainingDataset.datasetId},
      {"datasetModifiedTime", response.trainingDataset.modifyTime},
      {"datasetName", response.trainingDataset.datasetName},
      {"datasetType", response.trainingDataset.datasetType},
      {"parentDatasetId", response.trainingDataset.parentDatasetId},
      {"parentDatasetName", response.trainingDataset.parentDatasetName}};
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::getStatistics(std::string method, std::string host,
                                     std::string proto, std::string path,
                                     std::map<std::string, std::string> querys,
                                     std::map<std::string, std::string> headers,std::vector<std::string> matches,
                                     std::map<std::string, std::string> form,
                                     std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, getStatistics)

{
  int projectId = 0;
  int datasetId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/static",
                         &projectId, &datasetId);
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

  TurboLogic::GetDatasetStatisticsRequest request;
  TurboLogic::GetDatasetStatisticsResponse response;

  request.projectId = projectId;
  request.datasetId = datasetId;
  logic.dataset()->getDatasetStatistics(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = {{"datasetType", response.datasetType},
                 {"imageNum", response.numImages},
                 {"labeledNum", response.numLabeled},
                 {"parentDatasetId", response.parentDatasetId},
                 {"parentDatasetName", response.parentDatasetName},
                 {"tagNum", response.numTags},
                 {"unLabeledNum", response.numUnLabeled}};
  res["data"]["tagsInfo"] = json::array();
  for (auto tagStatic : response.tagStatistics) {
    json detail;
    detail = {
        {"imageNum", tagStatic.numImages},
        {"instanceNum", tagStatic.numInstances},
    };
    detail["tagInfo"] = {
        {"tagColor", tagStatic.tagInfo.color},
        {"tagCreateTime", tagStatic.tagInfo.createTime},
        {"tagDescription", tagStatic.tagInfo.description},
        {"tagEditable", tagStatic.tagInfo.editable},
        {"tagId", tagStatic.tagId},
        {"tagModifiedTime", tagStatic.tagInfo.modifyTime},
        {"tagName", tagStatic.tagInfo.name},
    };
    res["data"]["tagsInfo"].push_back(detail);
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::setBgAnnotataions(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,std::vector<std::string> matches,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, setBgAnnotataions) {
  int projectId = 0;
  int datasetId = 0;
  int imageId = 0;
  int converted = sscanf(path.c_str(),
                         "/v1/projects/%d/datasets/%d/images/%d/annotations/bg",
                         &projectId, &datasetId, &imageId);
  if (converted == 1) {
    std::cout << "datasetId:" << datasetId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;

  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::SetBackGroundAnnotationsRequest request;
  TurboLogic::SetBackGroundAnnotationsResponse response;

  request.projectId = projectId;
  request.datasetId = datasetId;
  request.imageId = imageId;
  logic.annotation()->setBackGroundAnnotations(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }
  auto annotation = response.annotation;
  res["data"] = {
      {"annotationCreateTime", annotation.createdTime},
      {"annotationData", annotation.data},
      {"annotationId", annotation.annotationId},
      {"annotationModifiedTime", annotation.modifiedTime},
      {"annotationType", annotation.annotationType},
      {"tagId", annotation.tagId},
      {"height", annotation.height},
      {"width", annotation.width},
      {"x", annotation.xMin},
      {"y", annotation.yMin},
      {"area", annotation.area},

  };
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
std::shared_ptr<resMsg> DatasetRouter::clearAnnotataions(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,std::vector<std::string> matches,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, clearAnnotataions) {
  int projectId = 0;
  int datasetId = 0;
  int imageId = 0;
  int converted = sscanf(
      path.c_str(), "/v1/projects/%d/datasets/%d/images/%d/annotations/empty",
      &projectId, &datasetId, &imageId);
  if (converted == 1) {
    std::cout << "datasetId:" << datasetId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;

  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::ClearAnnotationsRequest request;
  TurboLogic::ClearAnnotationsResponse response;

  request.projectId = projectId;
  request.datasetId = datasetId;
  request.imageId = imageId;
  logic.annotation()->clearAnnotations(request, response);
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
std::shared_ptr<resMsg> DatasetRouter::restore(std::string method, std::string host,
                               std::string proto, std::string path,
                               std::map<std::string, std::string> querys,
                               std::map<std::string, std::string> headers,std::vector<std::string> matches,
                               std::map<std::string, std::string> form,
                               std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, restore) {
  int projectId = 0;
  int datasetId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/restore",
                         &projectId, &datasetId);
  if (converted == 1) {
    std::cout << "datasetId:" << datasetId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;

  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  json schema = schemaOfRoot();
  auto imageIds = schemaOfItem("array");
  imageIds["items"] = schemaOfItem("integer");
  auto tagIds = schemaOfItem("array");
  tagIds["items"] = schemaOfItem("integer");

  schema["properties"]["imageIds"] = imageIds;
  schema["properties"]["tagIds"] = tagIds;
  schema["properties"]["datasetName"] = schemaOfItem("string");
  schema["properties"]["mergeAttr"] = schemaOfItem("bool");
  schema["required"] = {"imageIds", "tagIds", "datasetName"};
  if (!jsonParseAndSchemaCheck(body, schema, req, res, true)) {
    return std::make_shared<resMsg>(res.dump());
  }
  TurboLogic::RestoreToAnotherDatasetRequest request;
  TurboLogic::RestoreToAnotherDatasetResponse response;
  if (!(req.contains("imageIds")) || req["imageIds"].size() == 0) {
    req["imageIds"] = {};
  }
  if (!(req.contains("mergeAttr"))) {
    req["mergeAttr"] = true;
  }
  for (auto& element : req["imageIds"]) {
    if (!(checkArg(element, "Integer", res))) {
      return std::make_shared<resMsg>(res.dump());
    }
    request.imageIds.push_back(element);
  }

  if (!(req.contains("tagIds")) || req["tagIds"].size() == 0) {
    req["tagIds"] = {};
  }
  for (auto& element : req["tagIds"]) {
    if (!(checkArg(element, "Integer", res))) {
      return std::make_shared<resMsg>(res.dump());
    }
    request.tagIds.push_back(element);
  }

  request.projectId = projectId;
  request.datasetId = datasetId;
  request.datasetName = req["datasetName"];
  request.mergeAttr = req["mergeAttr"];
  logic.dataset()->restoreToAnotherDataset(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }
  res["data"] = {{"datasetCreateTime", response.dataset.createTime},
                 {"datasetDescription", response.dataset.datasetDescription},
                 //{"datasetEditable", response.dataset.editable},
                 {"isUsed", !response.dataset.editable},
                 {"datasetId", response.dataset.datasetId},
                 {"datasetModifiedTime", response.dataset.modifyTime},
                 {"datasetName", response.dataset.datasetName},
                 {"datasetType", response.dataset.datasetType},
                 {"parentDatasetId", response.dataset.parentDatasetId},
                 {"parentDatasetName", response.dataset.parentDatasetName}};
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::createClsAnnotations(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,std::vector<std::string> matches,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, createClsAnnotations)

{
  int projectId = 0;
  int datasetId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/cls_annotations",
             &projectId, &datasetId);
  if (converted == 1) {
    std::cout << "datasetId:" << datasetId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  json reqExample = {{"imageIds", {1, 2}}, {"tagIds", {1, 2}}};
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";
  std::vector<std::string> needEntryList;
  needEntryList.push_back("imageIds");
  needEntryList.push_back("tagIds");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }
  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::CreateClsAnnotationsRequest request;
  TurboLogic::CreateClsAnnotationsResponse response;
  request.datasetId = datasetId;
  request.projectId = projectId;
  request.imageIds = req["imageIds"].get<std::vector<int>>();
  request.tagIds = req["tagIds"].get<std::vector<int>>();
  /* for (json item : req["annotations"]) {
     if (!checkRequest(item, reqItem, res)) {
       return std::make_shared<resMsg>(res.dump());
     }
     TurboLogic::ClsAnnotation anno;

     anno.imageId = item["imageId"].get<int>();
     if (!(item.contains("tagIds")) || item["tagIds"].size() == 0) {
       req["tagIds"] = {};
     }
     for (auto &element : item["tagIds"]) {
       if (!(checkArg(element, "Integer", res))) {
         return std::make_shared<resMsg>(res.dump());
       }
       anno.tagIds.push_back(element);
     }
     request.annotations.push_back(anno);
   } */

  logic.annotation()->createClsAnnotations(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }
  res["data"] = json::object();
  /* res["data"]["images"] = json::array();
   for (auto imageInfo : response.imageInfos) {
     json perImage = json::object();
     perImage["imageCreateTime"] = imageInfo.image.createTime;
     perImage["imageDescription"] = imageInfo.image.description;
     perImage["imageId"] = imageInfo.image.id;
     perImage["imageModifiedTime"] = imageInfo.image.modifyTime;
     perImage["imageName"] = imageInfo.image.name;
     perImage["imagePath"] = imageInfo.image.path;
     perImage["annotations"] = json::array();
     perImage["thumbnailPath"] = imageInfo.image.thumbnailPath;
     for (auto annotation : imageInfo.annotations) {
       json perAnnotation = {
           {"annotationId", annotation.annotationId},
           {"annotationCreateTime", annotation.createdTime},
           {"annotationData", annotation.data},
           {"annotationModifiedTime", annotation.modifiedTime},
           {"annotationType", annotation.annotationType},
           {"tagId", annotation.tagId},
       };
       perImage["annotations"].push_back(perAnnotation);
     }
     res["data"]["images"].push_back(perImage);
   }*/
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::checkDatasetUsedByPipeline(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,std::vector<std::string> matches,
    std::map<std::string, std::string> form, std::string body)
 */
DEFINITION_ROUTER(DatasetRouter, checkDatasetUsedByPipeline) {
  int projectId = 0;
  int datasetId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/check",
                         &projectId, &datasetId);
  if (converted == 1) {
    std::cout << "datasetId:" << datasetId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::CheckDatasetUsedBypipelinneRequest request;
  TurboLogic::CheckDatasetUsedBypipelinneResponse response;

  request.datasetId = datasetId;
  request.projectId = projectId;

  logic.dataset()->checkUsedBypipelinne(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  res["data"]["isUsed"] = response.isUsed;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::createV2(std::string method, std::string host,
                                std::string proto, std::string path,
                                std::map<std::string, std::string> querys,
                                std::map<std::string, std::string> headers,std::vector<std::string> matches,
                                std::map<std::string, std::string> form,
                                std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, createV2) {
  int id = 0;
  int converted = sscanf(path.c_str(), "/v2/projects/%d/datasets", &id);
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

  auto importTagNames = schemaOfItem("array");
  importTagNames["items"] = schemaOfName();

  schema["properties"]["datasetDescription"] = schemaOfDescription();
  schema["properties"]["datasetName"] = schemaOfName();
  schema["properties"]["datasetType"] = schemaOfAlgoType();
  schema["properties"]["importTagNames"] = importTagNames;

  schema["required"].push_back("datasetDescription");
  schema["required"].push_back("datasetName");
  schema["required"].push_back("datasetType");
  schema["required"].push_back("importTagNames");

  if (!jsonParseAndSchemaCheck(body, schema, req, res)) {
    return std::make_shared<resMsg>(res.dump());
  }
  TurboLogic::CreateDatasetV2Request request;
  TurboLogic::CreateDatasetV2Response response;
  if (req.contains("datasetDescription")) {
    request.datasetDescription = req["datasetDescription"];
  } else {
    request.datasetDescription = "";
  }
  request.datasetName = req["datasetName"];
  request.datasetType = TurboLogic::AlgorithmType(req["datasetType"]);
  if (!(checkArg(request.datasetType, "AlgorithmType", res))) {
    return std::make_shared<resMsg>(res.dump());
  }
  request.projectId = id;
  request.tagNames.clear();
  for (int i = 0; i < req["importTagNames"].size(); i++) {
    request.tagNames.push_back(req["importTagNames"][i]);
  }
  logic.dataset()->createDatasetV2(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  std::cout << "request.datasetType: " << request.datasetType << std::endl;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }
  res["data"] = {
      {"datasetCreateTime", response.createTime},
      {"datasetDescription", response.datasetDescription},
      //{"datasetEditable", response.editable},
      {"isUsed", !response.editable},
      {"datasetId", response.datasetId},
      {"datasetModifiedTime", response.modifyTime},
      {"datasetName", response.datasetName},
      {"datasetType", response.datasetType},
      {"parentDatasetId", response.parentDatasetId},
      {"parentDatasetName", response.parentDatasetName},
  };
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::mergeV2(std::string method, std::string host,
                               std::string proto, std::string path,
                               std::map<std::string, std::string> querys,
                               std::map<std::string, std::string> headers,std::vector<std::string> matches,
                               std::map<std::string, std::string> form,
                               std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, mergeV2) {
  int id = 0;
  int converted = sscanf(path.c_str(), "/v2/projects/%d/datasets/merge", &id);
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

  auto datasetIds = schemaOfItem("array");
  datasetIds["items"] = schemaOfItem("integer");
  datasetIds["minItems"] = 2;

  auto tagNames = schemaOfItem("array");
  tagNames["items"] = schemaOfName();

  schema["properties"]["datasetName"] = schemaOfName();
  schema["properties"]["tagNames"] = tagNames;
  schema["properties"]["datasetIds"] = datasetIds;
  schema["properties"]["mergeAttr"] = schemaOfItem("bool");

  schema["required"].push_back("datasetName");
  // schema["required"].push_back("tagNames");
  schema["required"].push_back("datasetIds");
  //schema["required"].push_back("mergeAttr");

  if (!jsonParseAndSchemaCheck(body, schema, req, res)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::MergeDatasetRequest request;
  TurboLogic::MergeDatasetResponse response;

  if (req.contains("datasetIds")) {
    for (auto& element : req["datasetIds"]) {
      request.targetDatasetIds.push_back(element);
    }
  }
  if (!req.contains("mergeAttr")) {
    req["mergeAttr"] = true;
  }
  if (req.contains("tagNames")) {
    for (auto& element : req["tagNames"]) {
      request.tagNames.push_back(element);
    }
  }

  request.projectId = id;
  request.datasetName = req["datasetName"];
  request.mergeAttr = req["mergeAttr"];
  logic.dataset()->mergeDataset(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = {{"datasetCreateTime", response.dataset.createTime},
                 {"datasetDescription", response.dataset.datasetDescription},
                 //{"datasetEditable", response.dataset.editable},
                 {"isUsed", !response.dataset.editable},
                 {"datasetId", response.dataset.datasetId},
                 {"datasetModifiedTime", response.dataset.modifyTime},
                 {"datasetName", response.dataset.datasetName},
                 {"datasetType", response.dataset.datasetType},
                 {"parentDatasetId", response.dataset.parentDatasetId},
                 {"parentDatasetName", response.dataset.parentDatasetName}};

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::duplicateV2(std::string method, std::string host,
                                   std::string proto, std::string path,
                                   std::map<std::string, std::string> querys,
                                   std::map<std::string, std::string> headers,std::vector<std::string> matches,
                                   std::map<std::string, std::string> form,
                                   std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, duplicateV2) {
  int projectId = 0;
  int datasetId = 0;
  int converted = sscanf(path.c_str(), "/v2/projects/%d/datasets/%d/duplicate",
                         &projectId, &datasetId);
  if (converted == 1) {
    std::cout << "datasetId:" << datasetId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  json reqExample;
  reqExample = {{"datasetName", "dataset"}, {"tagIds", {1, 2}}};
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  std::vector<std::string> needEntryList;
  needEntryList.push_back("datasetName");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }

  json schema = schemaOfRoot();
  auto tagIds = schemaOfItem("array");
  tagIds["items"] = schemaOfItem("integer");

  schema["properties"]["tagIds"] = tagIds;
  schema["properties"]["datasetName"] = schemaOfItem("string");
  schema["properties"]["mergeAttr"] = schemaOfItem("bool");

  schema["required"] = {"datasetName"};
  if (!jsonParseAndSchemaCheck(body, schema, req, res, true)) {
    return std::make_shared<resMsg>(res.dump());
  }
  TurboLogic::DuplicateDatasetRequest request;
  TurboLogic::DuplicateDatasetResponse response;
  if (!req.contains("mergeAttr")) {
    req["mergeAttr"] = true;
  }

  if (!(req.contains("tagIds")) || req["tagIds"].size() == 0) {
    req["tagIds"] = {};
  }
  for (auto& element : req["tagIds"]) {
    if (!(checkArg(element, "Integer", res))) {
      return std::make_shared<resMsg>(res.dump());
    }
    request.tagIds.push_back(element);
  }

  request.projectId = projectId;
  request.datasetId = datasetId;
  request.datasetName = req["datasetName"];
  request.mergeAttr = req["mergeAttr"];
  request.description = "";
  logic.dataset()->duplicateDataset(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = {
      {"datasetCreateTime", response.dataset.createTime},
      {"datasetDescription", response.dataset.datasetDescription},
      //{"datasetEditable", response.dataset.editable},
      {"isUsed", !response.dataset.editable},
      {"datasetId", response.dataset.datasetId},
      {"datasetModifiedTime", response.dataset.modifyTime},
      {"datasetName", response.dataset.datasetName},
      {"datasetType", response.dataset.datasetType},
      {"parentDatasetId", response.dataset.parentDatasetId},
      {"parentDatasetName", response.dataset.parentDatasetName},
  };
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::addImagesV2(std::string method, std::string host,
                                   std::string proto, std::string path,
                                   std::map<std::string, std::string> querys,
                                   std::map<std::string, std::string> headers,std::vector<std::string> matches,
                                   std::map<std::string, std::string> form,
                                   std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, addImagesV2) {
  int projectId = 0;
  int datasetId = 0;
  int converted = sscanf(path.c_str(), "/v2/projects/%d/datasets/%d/images",
                         &projectId, &datasetId);
  if (converted == 1) {
    std::cout << "datasetId:" << datasetId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;

  json schema = schemaOfRoot();

  auto imagesPath = schemaOfItem("array");
  // imagesPath["items"] = schemaOfDescription();
  // imagesPath["minItems"] = 1;

  schema["properties"]["imagesMetaInfo"] = schemaOfDescription();
  // schema["properties"]["imagesPath"] = imagesPath;
  schema["properties"]["isLoadAnn"] = schemaOfItem("boolean");
  schema["properties"]["isMergeTag"] = schemaOfItem("boolean");
  schema["properties"]["timeStamp"] = schemaOfItem("string");
  // schema["required"].push_back("imagesPath");
  schema["required"].push_back("isLoadAnn");
  schema["required"].push_back("isMergeTag");
  schema["required"].push_back("timeStamp");

  if (!jsonParseAndSchemaCheck(body, schema, req, res)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::AddImageV2Request request;
  TurboLogic::AddImageV2Response response;

  if (req.contains("imagesMetaInfo")) {
    request.imageDescription = req["imagesMetaInfo"];
  } else {
    request.imageDescription = "";
  }
  if (req.contains("timeStamp")) {
    request.timeStamp = req["timeStamp"];
  } else {
    request.timeStamp = "";
  }
  if (req.contains("imagesPath")) {
    for (auto& element : req["imagesPath"]) {
      request.imagesPath.push_back(element);
    }
  }
  request.isLoadAnn = false;
  if (req.contains("isLoadAnn")) {
    request.isLoadAnn = req["isLoadAnn"].get<bool>();
  }
  request.isMergeTag = false;
  if (req.contains("isMergeTag")) {
    request.isMergeTag = req["isMergeTag"].get<bool>();
  }
  if (req.contains("rootDir")) {
    request.rootDir = req["rootDir"];
  } else {
    request.rootDir = "";
  }
  request.projectId = projectId;
  request.datasetId = datasetId;
  logic.dataset()->addImageV2(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  // if (response.error.type != TurboLogic::ErrorType::Success) {
  //   return std::make_shared<resMsg>(res.dump());
  // }

  res["data"]["images"] = json::array();
  for (auto error : response.imagesError) {
    json detail;
    detail = {
        {"code", error.type},
        {"msg", error.message},
    };
    res["data"]["images"].push_back(detail);
  }
  std::cout << "json return: " << res << std::endl;
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::listTags(std::string method, std::string host,
                                std::string proto, std::string path,
                                std::map<std::string, std::string> querys,
                                std::map<std::string, std::string> headers,std::vector<std::string> matches,
                                std::map<std::string, std::string> form,
                                std::string body)
 */
DEFINITION_ROUTER(DatasetRouter, listTags) {
  int projectId = 0;
  int datasetId = 0;
  int converted = sscanf(path.c_str(), "/v2/projects/%d/datasets/%d/tags",
                         &projectId, &datasetId);
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

  TurboLogic::ListDatasetTagsRequest request;
  TurboLogic::ListDatasetTagsResponse response;

  request.projectId = projectId;
  request.datasetId = datasetId;
  logic.dataset()->listTags(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }
  res["data"]["tags"] = json::array();
  for (TurboLogic::DatasetTagItem tag : response.items) {
    tag.name = GB2312ToUTF8(tag.name);
    tag.description = GB2312ToUTF8(tag.description);
    json detail = {
        {"tagColor", tag.color},
        {"tagCreateTime", tag.createTime},
        {"tagDescription", tag.description},
        {"tagEditable", tag.editable},
        {"tagId", tag.id},
        {"tagModifiedTime", tag.modifyTime},
        {"tagName", tag.name},
    };
    res["data"]["tags"].push_back(detail);
  }
  res["data"]["total"] = res["data"]["tags"].size();
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::createTag(std::string method, std::string host,
                                 std::string proto, std::string path,
                                 std::map<std::string, std::string> querys,
                                 std::map<std::string, std::string> headers,std::vector<std::string> matches,
                                 std::map<std::string, std::string> form,
                                 std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, createTag) {
  int projectId = 0;
  int datasetId = 0;
  int converted = sscanf(path.c_str(), "/v2/projects/%d/datasets/%d/tags",
                         &projectId, &datasetId);
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
  json reqExample = {{"tagName", "tagName"},
                     {"tagDescription", "tagDescription"}};
  std::vector<std::string> needEntryList;
  needEntryList.push_back("tagName");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }
  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::CreateDatasetTagRequest request;
  TurboLogic::CreateDatasetTagResponse response;

  if (req.contains("tagDescription")) {
    request.tagDescription = req["tagDescription"];
  } else {
    request.tagDescription = "";
  }
  request.projectId = projectId;
  request.datasetId = datasetId;
  request.tagName = req["tagName"];
  if (req.contains("tagColor")) {
    request.tagColor = req["tagColor"];
  } else {
    request.tagColor = "";
  }

  logic.dataset()->createTag(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }
  res["data"] = {
      {"tagColor", response.tagColor},
      {"tagCreateTime", response.createTime},
      {"tagDescription", response.tagDescription},
      {"tagEditable", response.editable},
      {"tagId", response.tagId},
      {"tagModifiedTime", response.modifyTime},
      {"tagName", request.tagName},
  };
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::getTag(std::string method, std::string host,
                              std::string proto, std::string path,
                              std::map<std::string, std::string> querys,
                              std::map<std::string, std::string> headers,std::vector<std::string> matches,
                              std::map<std::string, std::string> form,
                              std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, getTag) {
  int projectId = 0;
  int datasetId = 0;
  int tagId = 0;
  int converted = sscanf(path.c_str(), "/v2/projects/%d/datasets/%d/tags/%d",
                         &projectId, &datasetId, &tagId);
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

  TurboLogic::GetDatasetTagInfoRequest request;
  TurboLogic::GetDatasetTagInfoResponse response;

  request.projectId = projectId;
  request.datasetId = datasetId;
  request.tagId = tagId;
  logic.dataset()->getTagInfo(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }
  res["data"] = {
      {"tagColor", response.tagColor},
      {"tagCreateTime", response.createTime},
      {"tagDescription", response.tagDescription},
      {"tagEditable", response.editable},
      {"tagId", request.tagId},
      {"tagModifiedTime", response.modifyTime},
      {"tagName", response.tagName},
  };
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::updateTag(std::string method, std::string host,
                                 std::string proto, std::string path,
                                 std::map<std::string, std::string> querys,
                                 std::map<std::string, std::string> headers,std::vector<std::string> matches,
                                 std::map<std::string, std::string> form,
                                 std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, updateTag) {
  int projectId = 0;
  int datasetId = 0;
  int tagId = 0;
  int converted = sscanf(path.c_str(), "/v2/projects/%d/datasets/%d/tags/%d",
                         &projectId, &datasetId, &tagId);
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
  json reqExample = {{"tagName", "tagName"}, {"tagColor", "tagColor"}};
  std::vector<std::string> needEntryList;
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }
  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<resMsg>(res.dump());
  }

  // ����ǰ�Ȳ�ѯ
  TurboLogic::GetDatasetTagInfoRequest getTagInfoRequest;
  TurboLogic::GetDatasetTagInfoResponse getTagInfoResponse;

  getTagInfoRequest.projectId = projectId;
  getTagInfoRequest.datasetId = datasetId;
  getTagInfoRequest.tagId = tagId;
  logic.dataset()->getTagInfo(getTagInfoRequest, getTagInfoResponse);
  res["code"] = getTagInfoResponse.error.type;
  res["msg"] = getTagInfoResponse.error.message;
  if (getTagInfoResponse.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::UpdateDatasetTagInfoRequest request;
  TurboLogic::UpdateDatasetTagInfoResponse response;

  if (req.contains("tagColor")) {
    request.tagColor = req["tagColor"];
  } else {
    request.tagColor = getTagInfoResponse.tagColor;
  }

  if (req.contains("tagName")) {
    request.tagName = req["tagName"];
  } else {
    request.tagName = getTagInfoResponse.tagName;
  }
  request.projectId = projectId;
  request.datasetId = datasetId;
  request.tagId = tagId;
  request.tagDescription = getTagInfoResponse.tagDescription;
  logic.dataset()->updateTagInfo(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"] = {
      {"tagColor", response.tag.color},
      {"tagCreateTime", response.tag.createTime},
      {"tagDescription", response.tag.description},
      {"tagEditable", response.tag.editable},
      {"tagId", response.tag.id},
      {"tagModifiedTime", response.tag.modifyTime},
      {"tagName", response.tag.name},
  };
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::deleteTag(std::string method, std::string host,
                                 std::string proto, std::string path,
                                 std::map<std::string, std::string> querys,
                                 std::map<std::string, std::string> headers,std::vector<std::string> matches,
                                 std::map<std::string, std::string> form,
                                 std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, deleteTag) {
  int projectId = 0;
  int datasetId = 0;
  int tagId = 0;
  int converted = sscanf(path.c_str(), "/v2/projects/%d/datasets/%d/tags/%d",
                         &projectId, &datasetId, &tagId);
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

  TurboLogic::DeleteDatasetTagRequest request;
  TurboLogic::DeleteDatasetTagResponse response;

  request.projectId = projectId;
  request.datasetId = datasetId;
  request.tagId = tagId;
  logic.dataset()->deleteTag(request, response);
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
std::shared_ptr<resMsg> DatasetRouter::mergeTag(std::string method, std::string host,
                                std::string proto, std::string path,
                                std::map<std::string, std::string> querys,
                                std::map<std::string, std::string> headers,
                                std::map<std::string, std::string> form,
                                std::string body) */
DEFINITION_ROUTER(DatasetRouter, mergeTag) {
  int projectId = 0;
  int datasetId = 0;
  int tagId = 0;
  int converted =
      sscanf(path.c_str(), "/v2/projects/%d/datasets/%d/tags/%d/merge_to",
             &projectId, &datasetId, &tagId);
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
  json reqExample = {{"tagId", 0}};
  std::vector<std::string> needEntryList;
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }
  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<resMsg>(res.dump());
  }
  TurboLogic::MergeDatasetTagRequest request;
  TurboLogic::MergeDatasetTagResponse response;

  request.projectId = projectId;
  request.datasetId = datasetId;
  request.tagId = tagId;
  request.mergeToTagId = req["tagId"].get<int>();
  logic.dataset()->mergeTag(request, response);
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
std::shared_ptr<resMsg> DatasetRouter::queryDatasetImageSamples(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)*/

DEFINITION_ROUTER(DatasetRouter, queryDatasetImageSamples) {
  int projectId = 0;
  int datasetId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/image_samples",
             &projectId, &datasetId);
  if (converted == 1) {
    std::cout << "datasetId:" << datasetId << std::endl;
  }

  json schema = schemaOfRoot();

  schema["properties"]["imageType"] = schemaOfItem("string");
  schema["properties"]["imageType"]["enum"] = {"Labeled", "Unlabeled", "All",
                                               "New", "AutoLabeled"};
  schema["properties"]["isDesc"] = schemaOfItem("boolean");
  schema["properties"]["isDesc"]["default"] = true;
  schema["properties"]["isFilterThr"] = schemaOfItem("boolean");
  schema["properties"]["isFilterThr"]["default"] = false;
  schema["properties"]["isReversePredTag"] = schemaOfItem("boolean");
  schema["properties"]["isReversePredTag"]["default"] = false;
  schema["properties"]["isReverseTag"] = schemaOfItem("boolean");
  schema["properties"]["isReverseTag"]["default"] = false;
  schema["properties"]["modelId"] = schemaOfItem("integer");
  schema["properties"]["modelId"]["default"] = 0;
  schema["properties"]["testId"] = schemaOfItem("integer");
  schema["properties"]["testId"]["default"] = 0;
  schema["properties"]["predTagName"] = schemaOfItem("string");
  schema["properties"]["predTagName"]["default"] = "";
  schema["properties"]["tagName"] = schemaOfItem("string");
  schema["properties"]["tagName"]["default"] = "";
  schema["properties"]["statisticsType"] = schemaOfItem("string");
  schema["properties"]["statisticsType"]["enum"] = {"instance", "image"};
  schema["properties"]["statisticsType"]["default"] = "instance";
  schema["properties"]["isRefresh"] = schemaOfItem("boolean");
  schema["properties"]["isRefresh"]["default"] = true;
  schema["properties"]["orderBy"] = schemaOfItem("string");
  schema["properties"]["orderBy"]["enum"] = {"createTime", "score"};
  schema["properties"]["orderBy"]["default"] = "createTime";
  schema["properties"]["isRegexFilePath"] = schemaOfItem("boolean");
  schema["properties"]["isRegexFilePath"]["default"] = false;
  schema["properties"]["filePath"] = schemaOfItem("string");
  schema["properties"]["filePath"]["default"] = "";
  schema["properties"]["attrId"] = schemaOfItem("integer");
  schema["properties"]["attrId"]["default"] = 0;
  schema["properties"]["limit"] = schemaOfItem("integer");
  schema["properties"]["limit"]["default"] = 20;
  schema["properties"]["offset"] = schemaOfItem("integer");
  schema["properties"]["offset"]["default"] = 0;
  //schema["properties"]["tagId"] = schemaOfItem("integer");
  //schema["properties"]["tagId"]["default"] = -2;
  //schema["required"] = {"imageType", "orderBy", "statisticsType"};
  schema["required"] = {"imageType", "orderBy"};
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";
  if (!jsonParseAndSchemaCheck(body, schema, req, res, true)) {
    return std::make_shared<resMsg>(res.dump());
  }
  LogTrace("QueryDatasetImageSamplesRequest: {}", req.dump());
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);

  TurboLogic::QueryDatasetImageSamplesRequest request;
  TurboLogic::QueryDatasetImageSamplesResponse response;

  request.imageType = req["imageType"];
  // createTime, score
  request.orderBy = req["orderBy"];
  request.statisticsType = req["statisticsType"];
  request.isDesc = req["isDesc"].get<bool>();
  request.isFilterThr = req["isFilterThr"].get<bool>();
  request.modelId = req["modelId"].get<int>();
  request.testId = req["testId"].get<int>();
  request.predTagName = req["predTagName"].get<std::string>();
  request.limit = req["limit"].get<int>();
  request.offset = req["offset"].get<int>();
  request.isRefresh = req["isRefresh"].get<int>();
  request.isReversePredTag = req["isReversePredTag"].get<int>();
  request.isReverseTag = req["isReverseTag"].get<int>();
  request.tagName = req["tagName"].get<std::string>();
  request.isRegexFilePath = req["isRegexFilePath"].get<int>();
  request.filePath = req["filePath"].get<std::string>();
  request.attrId = req["attrId"].get<int>();

  // -1 means, -2 means not filter
  request.datasetId = datasetId;
  request.projectId = projectId;
  response.total = 0;
  //logic.dataset()->queryDatasetImageSamples(request, response);
  logic.dataset()->queryDatasetImageSamplesV2(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  res["data"]["images"] = json::array();
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }
  res["data"]["total"] = response.total;
  res["data"]["images"] = response.data;
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::saveDatasetImageSamples(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
 */
DEFINITION_ROUTER(DatasetRouter, saveDatasetImageSamples) {
  int projectId = 0;
  int datasetId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/image_samples/save",
             &projectId, &datasetId);
  if (converted == 1) {
    std::cout << "datasetId:" << datasetId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  json reqExample;
  reqExample = {{"imageIds", {1, 2}},
                {"isAll", false},
                {"tagIds", {1, 2}},
                {"targetDatasetId", 1}};
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";
  std::vector<std::string> needEntryList;
  needEntryList.push_back("isAll");
  needEntryList.push_back("tagIds");
  needEntryList.push_back("targetDatasetId");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }
  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::SaveDatasetImageSamplesRequest request;
  TurboLogic::SaveDatasetImageSamplesResponse response;
  if (req["isAll"] == false && !req.contains("imageIds")) {
    res["msg"] = "imageIds not exist when isAll is false!";
    res["code"] = TurboLogic::ErrorType::ErrInvalidArg;
    return std::make_shared<resMsg>(res.dump());
  }
  if (!req.contains("saveAttr")) {
    req["saveAttr"] = true;
  }
  if (req.contains("imageIds")) {
    request.imageIds = req["imageIds"].get<std::vector<int>>();
  }

  request.saveAttr = req["saveAttr"];
  request.isAll = req["isAll"];
  request.tagIds = req["tagIds"].get<std::vector<int>>();
  request.targetDatasetId = req["targetDatasetId"];
  request.datasetId = datasetId;
  request.projectId = projectId;

  logic.dataset()->saveDatasetImageSamples(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }
  res["data"] = {
      {"datasetCreateTime", response.datasetInfo.createTime},
      {"datasetDescription", response.datasetInfo.datasetDescription},
      //{"datasetEditable", response.datasetInfo.editable},
      {"isUsed", !response.datasetInfo.editable},
      {"datasetId", response.datasetInfo.datasetId},
      {"datasetModifiedTime", response.datasetInfo.modifyTime},
      {"datasetName", response.datasetInfo.datasetName},
      {"datasetType", response.datasetInfo.datasetType},
      {"parentDatasetId", response.datasetInfo.parentDatasetId},
      {"parentDatasetName", response.datasetInfo.parentDatasetName},
  };
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
std::shared_ptr<resMsg> DatasetRouter::cropReiszeDatasetImages(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/

DEFINITION_ROUTER(DatasetRouter, cropReiszeDatasetImages) {
  int projectId = 0;
  int datasetId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/images/crop_resize",
             &projectId, &datasetId);
  if (converted == 1) {
    std::cout << "datasetId:" << datasetId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  json reqExample;
  reqExample = {{"imageIds", {1, 2}},
                {"isCenterPos", false},
                {"cropRect", {1, 2}},
                {"resizeHeight", 1},
                {"resizeWidth", 1},
                {"targetDatasetId", 1},
                {"channelMode", "channelMode"},
                {"isCrop", false},
                {"isResize", false},
                {"isReserveAnn", false},
                {"isKeepRatio", false}};
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";
  std::vector<std::string> needEntryList;
  needEntryList.push_back("imageIds");
  needEntryList.push_back("isCenterPos");
  needEntryList.push_back("targetDatasetId");
  needEntryList.push_back("isCrop");
  needEntryList.push_back("isResize");
  needEntryList.push_back("isReserveAnn");
  needEntryList.push_back("isKeepRatio");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<resMsg>(res.dump());
  }
  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::CropReiszeDatasetImagesRequest request;
  TurboLogic::CropReiszeDatasetImagesResponse response;
  if (req["isCrop"] == true &&
      (!req.contains("cropRect") || req["cropRect"].size() != 4)) {
    res["msg"] =
        "cropRect not exist or its size not equal 4  when isCrop is true!";
    res["code"] = TurboLogic::ErrorType::ErrInvalidArg;
    return std::make_shared<resMsg>(res.dump());
  }
  if (req["isResize"] == true &&
      (!req.contains("resizeHeight") || !req.contains("resizeWidth"))) {
    res["msg"] =
        "resizeHeight not exist or resizeWidth not exist  when isResize is "
        "true!";
    res["code"] = TurboLogic::ErrorType::ErrInvalidArg;
    return std::make_shared<resMsg>(res.dump());
  }
  if (!req.contains("saveAttr")) {
    req["saveAttr"] = true;
  }
  if (req.contains("resizeHeight")) {
    request.resizeHeight = req["resizeHeight"].get<int>();
  }
  if (req.contains("resizeWidth")) {
    request.resizeWidth = req["resizeWidth"].get<int>();
  }
  if (req.contains("cropRect")) {
    request.cropRect = req["cropRect"].get<std::vector<int>>();
  }
  if (req.contains("channelMode")) {
    request.channelMode = req["channelMode"].get<std::string>();
  }
  if (req.contains("saveAttr")) {
    request.saveAttr = req["saveAttr"].get<bool>();
  }
  request.isCrop = req["isCrop"];
  request.isResize = req["isResize"];
  request.isCenterPos = req["isCenterPos"];
  request.isKeepRatio = req["isKeepRatio"];
  request.isReserveAnn = req["isReserveAnn"];
  request.imageIds = req["imageIds"].get<std::vector<int>>();
  request.targetDatasetId = req["targetDatasetId"];
  request.datasetId = datasetId;
  request.projectId = projectId;

  logic.dataset()->cropReiszeDatasetImages(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }
  res["data"]["cropResize"] = json::array();
  for (auto error : response.imagesError) {
    json detail;
    detail = {
        {"code", error.type},
        {"msg", error.message},
    };
    res["data"]["cropResize"].push_back(detail);
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::shared_ptr<resMsg> DatasetRouter::SaveLatestAnnotatedImage(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
 */
DEFINITION_ROUTER(DatasetRouter, SaveLatestAnnotatedImage) {
  int projectId = 0;
  int datasetId = 0;
  int imageId = 0;
  int converted =
      sscanf(path.c_str(),
             "/v1/projects/%d/datasets/%d/images/%d/save_as_latestannotated",
             &projectId, &datasetId, &imageId);
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

  TurboLogic::SaveLatestAnnotatedImageRequest request;
  TurboLogic::SaveLatestAnnotatedImageResponse response;

  request.projectId = projectId;
  request.datasetId = datasetId;
  request.imageId = imageId;
  logic.dataset()->SaveLatestAnnotatedImage(request, response);
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
std::shared_ptr<resMsg> DatasetRouter::GetLatestAnnotatedImage(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, GetLatestAnnotatedImage) {
  int projectId = 0;
  int datasetId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/images/latestannotated",
             &projectId, &datasetId);
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

  TurboLogic::GetLatestAnnotatedImageRequest request;
  TurboLogic::GetLatestAnnotatedImageResponse response;

  request.projectId = projectId;
  request.datasetId = datasetId;
  logic.dataset()->GetLatestAnnotatedImage(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"]["annotations"] = json::array();

  for (auto annotation : response.annotations) {
    json detail;
    detail = {
        {"annotationCreateTime", annotation.createdTime},
        {"annotationData", annotation.data},
        {"annotationId", annotation.annotationId},
        {"annotationModifiedTime", annotation.modifiedTime},
        {"annotationType", annotation.annotationType},
        {"tagId", annotation.tagId},
        {"area", annotation.area},
        {"height", annotation.height},
        {"tagName", annotation.tagName},
        {"width", annotation.width},
        {"x", annotation.xMin},
        {"y", annotation.yMin},
    };
    res["data"]["annotations"].push_back(detail);
  }
  res["data"]["imageCreateTime"] = response.createTime;
  res["data"]["imageDescription"] = response.imageDescription;
  res["data"]["imageId"] = response.imageId;
  res["data"]["imageModifiedTime"] = response.modifyTime;
  res["data"]["imageName"] = response.imageName;
  res["data"]["imagePath"] = response.imagePath;
  res["data"]["thumbnailPath"] = response.thumbnailPath;

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
resMsg* DatasetRouter::SaveDatasetPredictions(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
    */
DEFINITION_ROUTER(DatasetRouter, SaveDatasetPredictions) {
  int projectId = 0;
  int datasetId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/predictions/save",
             &projectId, &datasetId);
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

  auto imageIds = schemaOfItem("array");
  imageIds["items"] = schemaOfItem("integer");

  schema["properties"]["isAll"] = schemaOfItem("boolean");
  schema["properties"]["isAll"]["default"] = false;
  schema["properties"]["isOverwrite"] = schemaOfItem("boolean");
  schema["properties"]["isOverwrite"]["default"] = false;
  schema["properties"]["modelId"] = schemaOfItem("integer");
  schema["properties"]["imageIds"] = imageIds;
  schema["properties"]["testId"] = schemaOfItem("integer");
  schema["properties"]["testId"]["default"] = 0;
  schema["required"].push_back("imageIds");
  //schema["required"].push_back("isOverwrite");
  schema["required"].push_back("modelId");
  //schema["required"].push_back("isAll");

  if (!jsonParseAndSchemaCheck(body, schema, req, res, true)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::SaveDatasetPredictionsRequest request;
  TurboLogic::SaveDatasetPredictionsResponse response;
  request.testId = req["testId"];
  request.projectId = projectId;
  request.datasetId = datasetId;
  request.modelId = req["modelId"];
  request.imageIds = req["imageIds"].get<std::vector<int>>();
  request.isAll = req["isAll"];
  request.isOverwrite = req["isOverwrite"];
  logic.dataset()->SaveDatasetPredictions(request, response);
  LogTrace("res 1");
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  LogTrace("res 2");
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }
  LogTrace("res 3");
  res["data"] = json::object();
  std::string dumpData;
  LogTrace("res 4");
  dumpResponse(res, dumpData);
  LogTrace("res 5");
  return std::make_shared<resMsg>(res.dump());
};
/*
resMsg* DatasetRouter::ListDatasetAttrs(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
    */
DEFINITION_ROUTER(DatasetRouter, ListDatasetAttrs) {
  int projectId = 0;
  int datasetId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/attributes",
                         &projectId, &datasetId);
  if (converted == 2) {
    std::cout << "projectId:" << projectId << "datasetId:" << datasetId
              << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::ListDatasetAttrsRequest request;
  TurboLogic::ListDatasetAttrsResponse response;
  request.projectId = projectId;
  request.datasetId = datasetId;
  logic.dataset()->listAttrs(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }
  res["data"] = json::object();
  res["data"]["imageAttrs"] = json::array();
  for (auto iter = response.items.begin(); iter != response.items.end();
       iter++) {
    json item;
    item["imageAttrId"] = iter->id;
    item["imageAttrName"] = iter->name;
    res["data"]["imageAttrs"].push_back(item);
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
resMsg* DatasetRouter::AddDatasetAttrs(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, AddDatasetAttrs) {
  int projectId = 0;
  int datasetId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/attributes",
                         &projectId, &datasetId);
  if (converted == 2) {
    std::cout << "projectId:" << projectId << "datasetId:" << datasetId
              << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  json schema = schemaOfRoot();
  schema["properties"]["imageAttrName"] = schemaOfItem("string");
  schema["required"] = {"imageAttrName"};
  if (!jsonParseAndSchemaCheck(body, schema, req, res, true)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::CreateDatasetAttrRequest request;
  TurboLogic::CreateDatasetAttrResponse response;
  request.projectId = projectId;
  request.datasetId = datasetId;
  request.attrName = req["imageAttrName"];

  logic.dataset()->createAttr(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }
  res["data"] = json::object();
  res["data"]["imageAttrId"] = response.attrId;
  res["data"]["imageAttrName"] = response.attrName;

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
resMsg* DatasetRouter::DeleteDatasetAttrs(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, DeleteDatasetAttrs) {
  int projectId = 0;
  int datasetId = 0;
  int attributeId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/attributes/%d",
             &projectId, &datasetId, &attributeId);
  if (converted == 3) {
    std::cout << "projectId:" << projectId << "datasetId:" << datasetId
              << "attributeId:" << attributeId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::DeleteDatasetAttrRequest request;
  TurboLogic::DeleteDatasetAttrResponse response;
  request.projectId = projectId;
  request.datasetId = datasetId;
  request.attrId = attributeId;
  logic.dataset()->deleteAttr(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }
  res["data"] = json::object();
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
resMsg* DatasetRouter::AddDatasetImageAttrs(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, AddDatasetImageAttrs) {
  int projectId = 0;
  int datasetId = 0;
  int imageId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/images/%d/attribute",
             &projectId, &datasetId, &imageId);
  if (converted == 3) {
    std::cout << "projectId:" << projectId << "datasetId:" << datasetId
              << "imageId:" << imageId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  json schema = schemaOfRoot();
  auto imageAttrIds = schemaOfItem("array");
  imageAttrIds["items"] = schemaOfItem("integer");

  schema["properties"]["imageAttrIds"] = imageAttrIds;
  schema["required"] = {"imageAttrIds"};
  if (!jsonParseAndSchemaCheck(body, schema, req, res, true)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::DatasetAddImageAttrsRequest request;
  TurboLogic::DatasetAddImageAttrsResponse response;
  request.projectId = projectId;
  request.datasetId = datasetId;
  request.imageId = imageId;
  for (auto& item : req["imageAttrIds"]) {
    TurboLogic::DatasetAttrItem attrItem;
    request.attrIds.push_back(item);
  }
  logic.dataset()->addImageAttrs(request, response);
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
resMsg* DatasetRouter::DeleteDatasetImageAttrs(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
 */
DEFINITION_ROUTER(DatasetRouter, DeleteDatasetImageAttrs) {
  int projectId = 0;
  int datasetId = 0;
  int imageId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/images/%d/attribute",
             &projectId, &datasetId, &imageId);
  if (converted == 3) {
    std::cout << "projectId:" << projectId << "datasetId:" << datasetId
              << "imageId:" << imageId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  json schema = schemaOfRoot();
  auto imageAttrIds = schemaOfItem("array");
  imageAttrIds["items"] = schemaOfItem("integer");

  schema["properties"]["imageAttrIds"] = imageAttrIds;
  schema["required"] = {"imageAttrIds"};
  if (!jsonParseAndSchemaCheck(body, schema, req, res, true)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::DatasetDeleteImageAttrsRequest request;
  TurboLogic::DatasetDeleteImageAttrsResponse response;
  request.projectId = projectId;
  request.datasetId = datasetId;
  request.imageId = imageId;
  for (auto& item : req["imageAttrIds"]) {
    request.attrIds.push_back(item);
  }
  logic.dataset()->deleteImageAttrs(request, response);
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
resMsg* DatasetRouter::exportVisualization(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
 */
DEFINITION_ROUTER(DatasetRouter, exportVisualization) {
  int projectId = 0;
  int datasetId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/visualization/export",
             &projectId, &datasetId);
  if (converted == 3) {
    std::cout << "projectId:" << projectId << "datasetId:" << datasetId
              << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);

  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  json schema = schemaOfRoot();

  schema["properties"]["modelId"] = schemaOfItem("integer");
  schema["properties"]["testId"] = schemaOfItem("integer");
  schema["properties"]["showAnno"] = schemaOfItem("boolean");
  schema["properties"]["exportPath"] = schemaOfItem("string");
  schema["required"].push_back("modelId");
  //schema["required"].push_back("testId");
  schema["required"].push_back("showAnno");
  schema["required"].push_back("exportPath");

  if (!jsonParseAndSchemaCheck(body, schema, req, res)) {
    return std::make_shared<resMsg>(res.dump());
    ;
  }

  TurboLogic::ExportVisualRequest request;
  TurboLogic::ExportVisualResponse response;

  if (req.contains("modelId")) {
    request.modelId = req["modelId"];
  } else {
    request.modelId = -1;
  }
  if (req.contains("testId")) {
    request.testId = req["testId"];
  } else {
    request.testId = -1;
  }

  request.showAnno = false;
  if (req.contains("showAnno")) {
    request.showAnno = req["showAnno"].get<bool>();
  }

  if (req.contains("exportPath")) {
    request.exportPath = req["exportPath"];
  } else {
    request.exportPath = "";
  }

  request.projectId = projectId;
  request.datasetId = datasetId;
  logic.dataset()->exportVisualization(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
    ;
  }

  res["data"] = {
      {"errCode", response.errCode},
      {"errMsg", response.errMsg},
      {"status", TurboLogic::ProcessStatus::Progress},
  };

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
  ;
}
/*
resMsg* DatasetRouter::cancelExportVisualization(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, cancelExportVisualization) {
  int projectId = 0;
  int datasetId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/visualization/cancel",
             &projectId, &datasetId);
  if (converted == 3) {
    std::cout << "projectId:" << projectId << "datasetId:" << datasetId
              << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);

  json req;
  json res;
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::CancelExportVisualRequest request;
  TurboLogic::CancelExportVisualResponse response;
  request.projectId = projectId;
  request.datasetId = datasetId;
  logic.dataset()->cancelExportVisualization(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
    ;
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
  ;
}
/*
resMsg* DatasetRouter::getExportVisualizationProgress(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
 */
DEFINITION_ROUTER(DatasetRouter, getExportVisualizationProgress) {
  int projectId = 0;
  int datasetId = 0;
  int converted = sscanf(
      path.c_str(), "/v1/projects/%d/datasets/%d/visualization/export/progress",
      &projectId, &datasetId);
  if (converted == 3) {
    std::cout << "projectId:" << projectId << "datasetId:" << datasetId
              << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);

  json req;
  json res;
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::GetExportVisualProgressRequest request;
  TurboLogic::GetExportVisualProgressResponse response;
  request.projectId = projectId;
  request.datasetId = datasetId;
  logic.dataset()->getExportVisualizationProgress(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
    ;
  }

  res["data"] = {
      {"errCode", response.errCode},
      {"errMsg", response.errMsg},
      {"status", (int)response.status},
      {"progress", response.progress},
  };

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
  ;
}
/*
resMsg* DatasetRouter::exportAnnotation(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
 */
DEFINITION_ROUTER(DatasetRouter, exportAnnotation) {
  int projectId = 0;
  int datasetId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/datasets/%d/annotations/export",
             &projectId, &datasetId);
  if (converted == 2) {
    std::cout << "projectId:" << projectId << "datasetId:" << datasetId
              << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);

  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  json schema = schemaOfRoot();

  schema["properties"]["exportPath"] = schemaOfItem("string");
  schema["required"].push_back("exportPath");

  if (!jsonParseAndSchemaCheck(body, schema, req, res)) {
    return std::make_shared<resMsg>(res.dump());
    ;
  }

  TurboLogic::ExportAnnotationsRequest request;
  TurboLogic::ExportAnnotationsResponse response;

  if (req.contains("exportPath")) {
    request.exportPath = req["exportPath"];
  } else {
    request.exportPath = "";
  }

  request.projectId = projectId;
  request.datasetId = datasetId;
  logic.dataset()->exportAnnotations(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
    ;
  }

  res["data"] = {
      {"errCode", response.errCode},
      {"errMsg", response.errMsg},
      {"status", TurboLogic::ProcessStatus::Progress},
  };

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
  ;
}
/*
resMsg* DatasetRouter::cancelExportAnnotation(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, cancelExportAnnotation) {
  int projectId = 0;
  int datasetId = 0;
  int converted = sscanf(
      path.c_str(), "/v1/projects/%d/datasets/%d/annotations/export/cancel",
      &projectId, &datasetId);
  if (converted == 2) {
    std::cout << "projectId:" << projectId << "datasetId:" << datasetId
              << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);

  json req;
  json res;
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::CancelExportAnnotationsRequest request;
  TurboLogic::CancelExportAnnotationsResponse response;
  request.projectId = projectId;
  request.datasetId = datasetId;
  logic.dataset()->cancelExportAnnotations(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
    ;
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
  ;
}
/*
resMsg* DatasetRouter::getExportAnnotationProgress(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(DatasetRouter, getExportAnnotationProgress) {
  int projectId = 0;
  int datasetId = 0;
  int converted = sscanf(
      path.c_str(), "/v1/projects/%d/datasets/%d/annotations/export/progress",
      &projectId, &datasetId);
  if (converted == 2) {
    std::cout << "projectId:" << projectId << "datasetId:" << datasetId
              << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);

  json req;
  json res;
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::GetExportAnnotationsProgressRequest request;
  TurboLogic::GetExportAnnotationsProgressResponse response;
  request.projectId = projectId;
  request.datasetId = datasetId;
  logic.dataset()->getExportAnnotationsProgress(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
    ;
  }

  res["data"] = {
      {"errCode", response.errCode},
      {"errMsg", response.errMsg},
      {"status", (int)response.status},
      {"progress", response.progress},
  };

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
  ;
}
