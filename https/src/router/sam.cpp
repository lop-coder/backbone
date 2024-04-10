#include "sam.h"

#include <iostream>
#include <unordered_map>
#include "common.h"
#include "logic.h"
#include "schema.h"
using namespace TurboHttp;
////////////////////////////////////////////////////////////////////////////////////////////////////
SAMRouter::SAMRouter() {
  REGISTER_ROUTER("POST", "/v1/tools/sam/start", SAMRouter::start);
  REGISTER_ROUTER("POST", "/v1/tools/sam/stop", SAMRouter::stop);
  REGISTER_ROUTER("POST", "/v1/projects/:projectId/sam/embedding",
                  SAMRouter::embedding);
  REGISTER_ROUTER("POST", "/v1/projects/:projectId/sam/segment",
                  SAMRouter::segment);
}
SAMRouter::~SAMRouter() {}
////////////////////////////////////////////////////////////////////////////////////////////////////
/*
resMsg* SAMRouter::start(std::string method, std::string host,
                         std::string proto, std::string path,
                         std::map<std::string, std::string> querys,
                         std::map<std::string, std::string> headers,
                         std::map<std::string, std::string> form,
                         std::string body)
*/
DEFINITION_ROUTER(SAMRouter, start) {
  /*
  int projectId = 0;
  int modelId = 0;
  int converted = sscanf(path.c_str(), "/v1/tools/sam/start,
                         &projectId, &modelId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }*/
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";
  TurboLogic::CreateSAMRequest request;
  TurboLogic::CreateSAMResponse response;
  logic.SAM()->startSAM(request, response);
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
resMsg* SAMRouter::stop(std::string method, std::string host, std::string proto,
                        std::string path,
                        std::map<std::string, std::string> querys,
                        std::map<std::string, std::string> headers,
                        std::map<std::string, std::string> form,
                        std::string body)
*/
DEFINITION_ROUTER(SAMRouter, stop) {

  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::StopSAMResponse response;
  logic.SAM()->stopSAM(response);
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
resMsg* SAMRouter::embedding(std::string method, std::string host,
                             std::string proto, std::string path,
                             std::map<std::string, std::string> querys,
                             std::map<std::string, std::string> headers,
                             std::map<std::string, std::string> form,
                             std::string body)
*/
DEFINITION_ROUTER(SAMRouter, embedding) {
  int projectId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/sam/embedding", &projectId);
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

  schema["properties"]["imageIds"] = imageIds;
  schema["required"].push_back("imageIds");

  if (!jsonParseAndSchemaCheck(body, schema, req, res)) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::EmbeddingRequest request;
  TurboLogic::EmbeddingResponse response;

  request.projectId = projectId;
  //request.imageIds = req["imageIds"].get<std::vector<int>>();
  LogWarn("{}", req["imageIds"].dump(-2));
  for (auto item : req["imageIds"].items()) {
    request.imageIds.push_back(item.value());
    LogWarn("imageId:{}", item.value());
  }

  logic.SAM()->embedding(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }
  res["data"] = {
      {"imageId", response.imageId},
  };
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
/*
std::vector<int> points_on;
std::vector<int> points_out;
std::vector<std::vector<int>> points_in;
std::string type;
*/
/*
resMsg* SAMRouter::segment(std::string method, std::string host,
                           std::string proto, std::string path,
                           std::map<std::string, std::string> querys,
                           std::map<std::string, std::string> headers,
                           std::map<std::string, std::string> form,
                           std::string body)
*/
DEFINITION_ROUTER(SAMRouter, segment) {
  int projectId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/sam/segment", &projectId);
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

  auto points = schemaOfItem("array");
  points["items"] = schemaOfItem("object");
  points["items"]["properties"]["x"] = schemaOfItem("integer");
  points["items"]["properties"]["y"] = schemaOfItem("integer");
  points["items"]["properties"]["label"] = schemaOfItem("integer");
  points["items"]["required"] = {"x", "y", "label"};

  auto bboxs = schemaOfItem("array");
  bboxs["items"] = schemaOfItem("object");
  bboxs["items"]["properties"]["xMin"] = schemaOfItem("integer");
  bboxs["items"]["properties"]["yMin"] = schemaOfItem("integer");
  bboxs["items"]["properties"]["xMax"] = schemaOfItem("integer");
  bboxs["items"]["properties"]["yMax"] = schemaOfItem("integer");
  bboxs["items"]["required"] = {"xMin", "yMin", "xMax", "yMax"};

  schema["properties"]["imageId"] = schemaOfItem("integer");
  schema["properties"]["points"] = points;
  schema["properties"]["bboxs"] = bboxs;

  schema["required"].push_back("imageId");
  schema["required"].push_back("points");
  schema["required"].push_back("bboxs");

  if (!jsonParseAndSchemaCheck(body, schema, req, res)) {
    return std::make_shared<resMsg>(res.dump());
  }
  TurboLogic::SegmentRequest request;
  TurboLogic::SegmentResponse response;
  TurboLogic::points point;
  TurboLogic::bboxs bbox;
  request.projectId = projectId;
  request.imageId = req["imageId"];
  for (auto& item : req["points"].items()) {
    point.x = item.value()["x"];
    point.y = item.value()["y"];
    point.label = item.value()["label"];
    request.points.push_back(point);
  }
  for (auto& item : req["bboxs"].items()) {
    bbox.x_min = item.value()["xMin"];
    bbox.y_min = item.value()["yMin"];
    bbox.x_max = item.value()["xMax"];
    bbox.y_max = item.value()["yMax"];
    request.bboxs.push_back(bbox);
  }
  logic.SAM()->segment(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }
  res["data"]["polys"] = json(response.polys);
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
