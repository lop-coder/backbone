#include "model.h"

#include <iostream>

#include "common.h"
#include "logger.h"
#include "logic.h"
#include "logictype.h"
#include "schema.h"
#include "wrapper.h"
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
/*
resMsg* ModelRouter::getCmV3(std::string method, std::string host,
                             std::string proto, std::string path,
                             std::map<std::string, std::string> querys,
                             std::map<std::string, std::string> headers,
                             std::map<std::string, std::string> form,
                             std::string body)
*/
DEFINITION_ROUTER(ModelRouter, getCmV3) {
  int projectId = 0;
  int modelId = 0;
  int converted = sscanf(path.c_str(), "/v3/projects/%d/models/%d/cm",
                         &projectId, &modelId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";
  json defaultTagsConfigJson =
      json::parse(TurboLogic::getDefaultTagsConfigV3());
  json schema = schemaOfRoot();

  auto thresholds = schemaOfItem("array");
  thresholds["items"] = schemaOfItem("object");
  thresholds["items"]["properties"]["value"] = schemaOfItem("number");
  thresholds["items"]["properties"]["type"] = schemaOfItem("string");
  thresholds["items"]["properties"]["tagName"] = schemaOfItem("string");
  thresholds["items"]["properties"]["tagId"] = schemaOfItem("integer");
  thresholds["items"]["required"] = {"value", "type", "tagName", "tagId"};
  schema["properties"]["type"] = schemaOfItem("string");
  schema["properties"]["type"]["enum"] = {"image", "instance"};
  schema["properties"]["thresholds"] = thresholds;
  schema["properties"]["areaThreshold"] = schemaOfItem("number");
  schema["properties"]["areaThreshold"]["minimum"] = 1;
  schema["properties"]["areaThreshold"]["maximum"] = 100000;
  schema["properties"]["datasetId"] = schemaOfItem("integer");
  schema["properties"]["iou"] = schemaOfItem("number");
  schema["properties"]["iou"]["minimum"] = 0;
  schema["properties"]["iou"]["maximum"] = 1;
  schema["properties"]["minRectWidth"] = schemaOfItem("number");
  schema["properties"]["minRectWidth"]["minimum"] = 0;
  schema["properties"]["minRectHeight"] = schemaOfItem("number");
  schema["properties"]["minRectHeight"]["minimum"] = 0;
  schema["required"] = {"type", "thresholds", "datasetId"};
  if (!jsonParseAndSchemaCheck(body, schema, req, res)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  TurboLogic::GetValConfusionMatrixV3Request request;
  TurboLogic::GetValConfusionMatrixV3Response response;

  if (req.contains("iou")) {
    request.cmData.iou = req["iou"];
  } else {
    request.cmData.iou = defaultTagsConfigJson["iou"];
  }
  if (req.contains("areaThreshold")) {
    request.cmData.areaThreshold = req["areaThreshold"];
  } else {
    request.cmData.areaThreshold = defaultTagsConfigJson["areaThreshold"];
  }
  if (req.contains("minRectHeight")) {
    request.cmData.minRectHeight = req["minRectHeight"];
  } else {
    request.cmData.minRectHeight = defaultTagsConfigJson["minRectHeight"];
  }
  if (req.contains("minRectWidth")) {
    request.cmData.minRectWidth = req["minRectWidth"];
  } else {
    request.cmData.minRectWidth = defaultTagsConfigJson["minRectWidth"];
  }
  request.projectId = projectId;
  request.datasetId = req["datasetId"];
  request.modelId = modelId;
  for (auto& item : req["thresholds"]) {
    TurboLogic::Threshold threshold;
    threshold.tagId = item["tagId"];
    threshold.tagName = item["tagName"];
    threshold.type = item["type"];
    threshold.value = item["value"];
    request.cmData.thresholds.emplace_back(threshold);
  }
  request.cmData.type = req["type"];
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  logic.model()->getValConfusionMatrixV3(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  res["data"] = {
      {"areaThreshold", response.cmData.areaThreshold},
      {"okngStatistics", response.cmData.okngStatistics},
      {"recallFprFnrFppi", response.cmData.recallFprFnrFppi},
      {"cm", response.cmData.cm},
      {"iou", response.cmData.iou},
      {"thresholds", json::array()},
      {"type", response.cmData.type},
      {"datasetId", request.datasetId},
      {"minRectHeight", response.cmData.minRectHeight},
      {"minRectWidth", response.cmData.minRectWidth},
  };
  for (auto& item : response.cmData.thresholds) {
    json threshold = {{"tagId", item.tagId},
                      {"tagName", item.tagName},
                      {"value", item.value},
                      {"type", item.type}};

    res["data"]["thresholds"].push_back(threshold);
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* ModelRouter::getCmInfoV3(std::string method, std::string host,
                                 std::string proto, std::string path,
                                 std::map<std::string, std::string> querys,
                                 std::map<std::string, std::string> headers,
                                 std::map<std::string, std::string> form,
                                 std::string body)
*/
DEFINITION_ROUTER(ModelRouter, getCmInfoV3) {

  int projectId = 0;
  int modelId = 0;
  int converted = sscanf(path.c_str(), "/v3/projects/%d/models/%d/cm_info",
                         &projectId, &modelId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::GetValConfusionMatrixInfoV3Request request;
  TurboLogic::GetValConfusionMatrixInfoV3Response response;
  request.modelId = modelId;
  request.projectId = projectId;
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  logic.model()->getValConfusionMatrixInfoV3(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  res["data"] = {
      {"areaThreshold", response.cmData.areaThreshold},
      {"okngStatistics", response.cmData.okngStatistics},
      {"recallFprFnrFppi", response.cmData.recallFprFnrFppi},
      {"cm", response.cmData.cm},
      {"iou", response.cmData.iou},
      {"thresholds", json::array()},
      {"type", response.cmData.type},
      {"datasetId", response.datasetId},
      {"minRectHeight", response.cmData.minRectHeight},
      {"minRectWidth", response.cmData.minRectWidth},
  };
  for (auto& item : response.cmData.thresholds) {
    json threshold = {{"tagId", item.tagId},
                      {"tagName", item.tagName},
                      {"value", item.value},
                      {"type", item.type}};

    res["data"]["thresholds"].push_back(threshold);
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

DEFINITION_ROUTER(ModelRouter, putCmInfoV3) {

  int projectId = 0;
  int modelId = 0;
  int converted = sscanf(path.c_str(), "/v3/projects/%d/models/%d/cm_info",
                         &projectId, &modelId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";
  json defaultTagsConfigJson =
      json::parse(TurboLogic::getDefaultTagsConfigV3());
  json schema = schemaOfRoot();

  auto thresholds = schemaOfItem("array");
  thresholds["items"] = schemaOfItem("object");
  thresholds["items"]["properties"]["value"] = schemaOfItem("number");
  thresholds["items"]["properties"]["type"] = schemaOfItem("string");
  thresholds["items"]["properties"]["tagName"] = schemaOfItem("string");
  thresholds["items"]["properties"]["tagId"] = schemaOfItem("integer");
  thresholds["items"]["required"] = {"value", "type", "tagName", "tagId"};
  schema["properties"]["type"] = schemaOfItem("string");
  schema["properties"]["type"]["enum"] = {"image", "instance"};
  //schema["properties"]["type"]["oneOf"] = {{{"multipleOf", "image"}},
  //                                         {{"multipleOf", "instance"}}};
  schema["properties"]["thresholds"] = thresholds;
  schema["properties"]["areaThreshold"] = schemaOfItem("number");
  schema["properties"]["areaThreshold"]["minimum"] = 1;
  schema["properties"]["areaThreshold"]["maximum"] = 100000;
  schema["properties"]["datasetId"] = schemaOfItem("integer");
  schema["properties"]["iou"] = schemaOfItem("number");
  schema["properties"]["iou"]["minimum"] = 0;
  schema["properties"]["iou"]["maximum"] = 1;
  schema["properties"]["minRectWidth"] = schemaOfItem("number");
  schema["properties"]["minRectWidth"]["minimum"] = 0;
  schema["properties"]["minRectHeight"] = schemaOfItem("number");
  schema["properties"]["minRectHeight"]["minimum"] = 0;
  schema["required"] = {"type", "thresholds", "datasetId"};

  if (!jsonParseAndSchemaCheck(body, schema, req, res)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  TurboLogic::PutValConfusionMatrixInfoV3Request request;
  TurboLogic::PutValConfusionMatrixInfoV3Response response;

  if (req.contains("iou")) {
    request.cmData.iou = req["iou"];
  } else {
    request.cmData.iou = defaultTagsConfigJson["iou"];
  }
  if (req.contains("areaThreshold")) {
    request.cmData.areaThreshold = req["areaThreshold"];
  } else {
    request.cmData.areaThreshold = defaultTagsConfigJson["areaThreshold"];
  }
  if (req.contains("cm")) {
    request.cmData.cm = req["cm"];
  } else {
    request.cmData.cm = defaultTagsConfigJson["cm"];
  }
  if (req.contains("okngStatistics")) {
    request.cmData.okngStatistics = req["okngStatistics"];
  } else {
    request.cmData.okngStatistics = defaultTagsConfigJson["okngStatistic"];
  }
  if (req.contains("recallFprFnrFppi")) {
    request.cmData.recallFprFnrFppi = req["recallFprFnrFppi"];
  } else {
    request.cmData.recallFprFnrFppi = defaultTagsConfigJson["recallFprFnrFppi"];
  }
  if (req.contains("minRectHeight")) {
    request.cmData.minRectHeight = req["minRectHeight"];
  } else {
    request.cmData.minRectHeight = defaultTagsConfigJson["minRectHeight"];
  }
  if (req.contains("minRectWidth")) {
    request.cmData.minRectWidth = req["minRectWidth"];
  } else {
    request.cmData.minRectWidth = defaultTagsConfigJson["minRectWidth"];
  }
  request.projectId = projectId;
  request.datasetId = req["datasetId"];
  request.modelId = modelId;
  for (auto& item : req["thresholds"]) {
    TurboLogic::Threshold threshold;
    threshold.tagId = item["tagId"];
    threshold.tagName = item["tagName"];
    threshold.type = item["type"];
    threshold.value = item["value"];
    request.cmData.thresholds.emplace_back(threshold);
  }
  request.cmData.type = req["type"];
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  logic.model()->putValConfusionMatrixInfoV3(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

DEFINITION_ROUTER(ModelRouter, getTestCmV3) {

  int projectId = 0;
  int modelId = 0;
  int testId = 0;
  int converted = sscanf(path.c_str(), "/v3/projects/%d/models/%d/tests/%d/cm",
                         &projectId, &modelId, &testId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";
  json defaultTagsConfigJson =
      json::parse(TurboLogic::getDefaultTagsConfigV3());
  json schema = schemaOfRoot();

  auto thresholds = schemaOfItem("array");
  thresholds["items"] = schemaOfItem("object");
  thresholds["items"]["properties"]["value"] = schemaOfItem("number");
  thresholds["items"]["properties"]["type"] = schemaOfItem("string");
  thresholds["items"]["properties"]["tagName"] = schemaOfItem("string");
  thresholds["items"]["properties"]["tagId"] = schemaOfItem("integer");
  thresholds["items"]["required"] = {"value", "type", "tagName", "tagId"};
  schema["properties"]["type"] = schemaOfItem("string");
  schema["properties"]["type"]["enum"] = {"image", "instance"};
  //schema["properties"]["type"]["oneOf"] = {{{"multipleOf", "image"}},
  //                                         {{"multipleOf", "instance"}}};
  schema["properties"]["thresholds"] = thresholds;
  schema["properties"]["areaThreshold"] = schemaOfItem("number");
  schema["properties"]["areaThreshold"]["minimum"] = 1;
  schema["properties"]["areaThreshold"]["maximum"] = 100000;
  schema["properties"]["datasetId"] = schemaOfItem("integer");
  schema["properties"]["iou"] = schemaOfItem("number");
  schema["properties"]["iou"]["minimum"] = 0;
  schema["properties"]["iou"]["maximum"] = 1;
  schema["properties"]["minRectWidth"] = schemaOfItem("number");
  schema["properties"]["minRectWidth"]["minimum"] = 0;
  schema["properties"]["minRectHeight"] = schemaOfItem("number");
  schema["properties"]["minRectHeight"]["minimum"] = 0;
  schema["required"] = {"type", "thresholds", "datasetId"};

  if (!jsonParseAndSchemaCheck(body, schema, req, res)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  TurboLogic::GetTestConfusionMatrixV3Request request;
  TurboLogic::GetTestConfusionMatrixV3Response response;

  if (req.contains("iou")) {
    request.cmData.iou = req["iou"];
  } else {
    request.cmData.iou = defaultTagsConfigJson["iou"];
  }
  if (req.contains("areaThreshold")) {
    request.cmData.areaThreshold = req["areaThreshold"];
  } else {
    request.cmData.areaThreshold = defaultTagsConfigJson["areaThreshold"];
  }
  if (req.contains("minRectHeight")) {
    request.cmData.minRectHeight = req["minRectHeight"];
  } else {
    request.cmData.minRectHeight = defaultTagsConfigJson["minRectHeight"];
  }
  if (req.contains("minRectWidth")) {
    request.cmData.minRectWidth = req["minRectWidth"];
  } else {
    request.cmData.minRectWidth = defaultTagsConfigJson["minRectWidth"];
  }
  request.projectId = projectId;
  request.datasetId = req["datasetId"];
  request.modelId = modelId;
  request.testId = testId;
  for (auto& item : req["thresholds"]) {
    TurboLogic::Threshold threshold;
    threshold.tagId = item["tagId"];
    threshold.tagName = item["tagName"];
    threshold.type = item["type"];
    threshold.value = item["value"];
    request.cmData.thresholds.emplace_back(threshold);
  }
  request.cmData.type = req["type"];
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  logic.model()->getTestConfusionMatrixV3(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  res["data"] = {
      {"areaThreshold", response.cmData.areaThreshold},
      {"okngStatistics", response.cmData.okngStatistics},
      {"recallFprFnrFppi", response.cmData.recallFprFnrFppi},
      {"cm", response.cmData.cm},
      {"iou", response.cmData.iou},
      {"thresholds", json::array()},
      {"type", response.cmData.type},
      {"datasetId", request.datasetId},
      {"minRectHeight", response.cmData.minRectHeight},
      {"minRectWidth", response.cmData.minRectWidth},
  };
  for (auto& item : response.cmData.thresholds) {
    json threshold = {{"tagId", item.tagId},
                      {"tagName", item.tagName},
                      {"value", item.value},
                      {"type", item.type}};

    res["data"]["thresholds"].push_back(threshold);
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

DEFINITION_ROUTER(ModelRouter, getTestCmInfoV3) {
  int projectId = 0;
  int modelId = 0;
  int testId = 0;
  int converted =
      sscanf(path.c_str(), "/v3/projects/%d/models/%d/tests/%d/cm_info",
             &projectId, &modelId, &testId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::GetTestConfusionMatrixInfoV3Request request;
  TurboLogic::GetTestConfusionMatrixInfoV3Response response;
  request.modelId = modelId;
  request.projectId = projectId;
  request.testId = testId;
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  logic.model()->getTestConfusionMatrixInfoV3(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  res["data"] = {
      {"areaThreshold", response.cmData.areaThreshold},
      {"okngStatistics", response.cmData.okngStatistics},
      {"recallFprFnrFppi", response.cmData.recallFprFnrFppi},
      {"cm", response.cmData.cm},
      {"iou", response.cmData.iou},
      {"thresholds", json::array()},
      {"type", response.cmData.type},
      {"datasetId", response.datasetId},
      {"minRectHeight", response.cmData.minRectHeight},
      {"minRectWidth", response.cmData.minRectWidth},
  };
  for (auto& item : response.cmData.thresholds) {
    json threshold = {{"tagId", item.tagId},
                      {"tagName", item.tagName},
                      {"value", item.value},
                      {"type", item.type}};

    res["data"]["thresholds"].push_back(threshold);
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

DEFINITION_ROUTER(ModelRouter, putTestCmInfoV3) {
  int projectId = 0;
  int modelId = 0;
  int testId = 0;
  int converted =
      sscanf(path.c_str(), "/v3/projects/%d/models/%d/tests/%d/cm_info",
             &projectId, &modelId, &testId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";
  json defaultTagsConfigJson =
      json::parse(TurboLogic::getDefaultTagsConfigV3());
  json schema = schemaOfRoot();

  auto thresholds = schemaOfItem("array");
  thresholds["items"] = schemaOfItem("object");
  thresholds["items"]["properties"]["value"] = schemaOfItem("number");
  thresholds["items"]["properties"]["type"] = schemaOfItem("string");
  thresholds["items"]["properties"]["tagName"] = schemaOfItem("string");
  thresholds["items"]["properties"]["tagId"] = schemaOfItem("integer");
  thresholds["items"]["required"] = {"value", "type", "tagName", "tagId"};
  schema["properties"]["type"] = schemaOfItem("string");
  schema["properties"]["type"]["enum"] = {"image", "instance"};
  //schema["properties"]["type"]["oneOf"] = {{{"multipleOf", "image"}},
  //                                         {{"multipleOf", "instance"}}};
  schema["properties"]["thresholds"] = thresholds;
  schema["properties"]["areaThreshold"] = schemaOfItem("number");
  schema["properties"]["areaThreshold"]["minimum"] = 1;
  schema["properties"]["areaThreshold"]["maximum"] = 100000;
  schema["properties"]["datasetId"] = schemaOfItem("integer");
  schema["properties"]["iou"] = schemaOfItem("number");
  schema["properties"]["iou"]["minimum"] = 0;
  schema["properties"]["iou"]["maximum"] = 1;
  schema["properties"]["minRectWidth"] = schemaOfItem("number");
  schema["properties"]["minRectWidth"]["minimum"] = 0;
  schema["properties"]["minRectHeight"] = schemaOfItem("number");
  schema["properties"]["minRectHeight"]["minimum"] = 0;
  schema["required"] = {"type", "thresholds", "datasetId"};

  if (!jsonParseAndSchemaCheck(body, schema, req, res)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  TurboLogic::PutTestConfusionMatrixInfoV3Request request;
  TurboLogic::PutTestConfusionMatrixInfoV3Response response;

  if (req.contains("iou")) {
    request.cmData.iou = req["iou"];
  } else {
    request.cmData.iou = defaultTagsConfigJson["iou"];
  }
  if (req.contains("areaThreshold")) {
    request.cmData.areaThreshold = req["areaThreshold"];
  } else {
    request.cmData.areaThreshold = defaultTagsConfigJson["areaThreshold"];
  }
  if (req.contains("cm")) {
    request.cmData.cm = req["cm"];
  } else {
    request.cmData.cm = defaultTagsConfigJson["cm"];
  }
  if (req.contains("okngStatistics")) {
    request.cmData.okngStatistics = req["okngStatistics"];
  } else {
    request.cmData.okngStatistics = defaultTagsConfigJson["okngStatistic"];
  }
  if (req.contains("recallFprFnrFppi")) {
    request.cmData.recallFprFnrFppi = req["recallFprFnrFppi"];
  } else {
    request.cmData.recallFprFnrFppi = defaultTagsConfigJson["recallFprFnrFppi"];
  }
  if (req.contains("minRectHeight")) {
    request.cmData.minRectHeight = req["minRectHeight"];
  } else {
    request.cmData.minRectHeight = defaultTagsConfigJson["minRectHeight"];
  }
  if (req.contains("minRectWidth")) {
    request.cmData.minRectWidth = req["minRectWidth"];
  } else {
    request.cmData.minRectWidth = defaultTagsConfigJson["minRectWidth"];
  }
  request.projectId = projectId;
  request.datasetId = req["datasetId"];
  request.modelId = modelId;
  request.testId = testId;
  for (auto& item : req["thresholds"]) {
    TurboLogic::Threshold threshold;
    threshold.tagId = item["tagId"];
    threshold.tagName = item["tagName"];
    threshold.type = item["type"];
    threshold.value = item["value"];
    request.cmData.thresholds.emplace_back(threshold);
  }
  request.cmData.type = req["type"];
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  logic.model()->putTestConfusionMatrixInfoV3(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};
