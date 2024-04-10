#include "sdk.h"

#include <iostream>
#include <string>
#include <unordered_map>

#include "common.h"
#include "logic.h"
#include "schema.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
SDKRouter::SDKRouter() {
  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/sdks)", SDKRouter::getSdks);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/sdks)", SDKRouter::setSdks);
  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/sdks/(\d+))", SDKRouter::getSdk);
  REGISTER_ROUTER("DELETE", R"(/v1/projects/(\d+)/sdks/(\d+))",
                  SDKRouter::deleteSdk);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/sdks/(\d+)/cancel_generate)",
                  SDKRouter::cancelGenerateSdk);
  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/sdks/(\d+)/sdk_log)",
                  SDKRouter::getSdkLog);
  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/sdks/(\d+)/sdk_progress)",
                  SDKRouter::getSdkProgress);

  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/sdks/check)",
                  SDKRouter::checkSdks);
}
SDKRouter::~SDKRouter() {}
////////////////////////////////////////////////////////////////////////////////////////////////////
/*
resMsg* SDKRouter::getSdks(std::string method, std::string host,
                           std::string proto, std::string path,
                           std::map<std::string, std::string> querys,
                           std::map<std::string, std::string> headers,
                           std::map<std::string, std::string> form,
                           std::string body)
*/
DEFINITION_ROUTER(SDKRouter, getSdks) {
  int projectId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/sdks", &projectId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  json req;
  json res;

  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  res["code"] = TurboLogic::ErrorType::ErrUnknown;

  TurboLogic::ListSDKsRequest request;
  TurboLogic::ListSDKsResponse response;
  if (querys.find("name") != querys.end()) {
    if (!isString(querys.find("name")->second)) {
      res["code"] = TurboLogic::ErrorType::ErrInvalidArg;
      res["msg"] = "name type is invalid!";
      return std::make_shared<TurboHttp::resMsg>(res.dump());
    }
    request.name = querys.find("name")->second;
  }
  request.projectId = projectId;

  logic.sdk()->listSDKs(request, response);

  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    res["data"] = json::object();
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  res["data"] = json::object();
  res["data"]["sdks"] = json::array();

  for (auto iter = response.items.rbegin(); iter != response.items.rend();
       iter++) {
    json detail;

    detail = {{"deployInfo", iter->deployInfo},
              {"modelId", iter->modelId},
              {"quantifyType", iter->quantifyType},
              {"modelName", iter->modelName},
              {"algorithmType", std::stoi(iter->algorithmType)},
              {"sdkCreateTime", iter->createTime},
              {"sdkDescription", iter->description},
              {"sdkId", iter->id},
              {"sdkMd5", iter->Md5},
              {"sdkModifiedTime", iter->modifiedTime},
              {"sdkName", iter->name},
              {"sdkPath", iter->path},
              {"sdkSize", iter->size},
              {"sdkStatus", std::stoi(iter->status)},
              {"testId", iter->testId},
              {"testVersion", iter->version}};
    res["data"]["sdks"].push_back(detail);
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
    return std::make_shared<TurboHttp::resMsg>(j_c.dump());
  };
  return std::make_shared<TurboHttp::resMsg>(res.dump());
}

/*
resMsg* SDKRouter::setSdks(std::string method, std::string host,
                           std::string proto, std::string path,
                           std::map<std::string, std::string> querys,
                           std::map<std::string, std::string> headers,
                           std::map<std::string, std::string> form,
                           std::string body)
 */
DEFINITION_ROUTER(SDKRouter, setSdks) {
  int projectId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/sdks", &projectId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  json req;
  json res;

  json schema = schemaOfRoot();

  schema["properties"]["description"] = schemaOfDescription();
  schema["properties"]["backend"] = schemaOfItem("string");
  schema["properties"]["deployDevice"] = schemaOfItem("string");
  schema["properties"]["deployOS"] = schemaOfItem("string");
  schema["properties"]["modelId"] = schemaOfItem("integer");
  schema["properties"]["quantifyType"] = schemaOfItem("string");
  schema["properties"]["testVersion"] = schemaOfItem("string");
  schema["required"].push_back("backend");
  schema["required"].push_back("deployOS");
  schema["required"].push_back("modelId");

  if (!jsonParseAndSchemaCheck(body, schema, req, res)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  /// res["code"] = TurboLogic::ErrorType::ErrUnknown;
  TurboLogic::CreateSDKRequest request;
  TurboLogic::CreateSDKResponse response;
  request.projectId = projectId;

  if (req.contains("deployDevice")) {
    request.deployDevice = req["deployDevice"];
  } else {
    request.deployDevice = "";
  }
  if (req.contains("quantifyType")) {
    request.quantifyType = req["quantifyType"];
  } else {
    request.quantifyType = "";
  }
  if (req.contains("description")) {
    request.description = req["description"];
  } else {
    request.description = "";
  }
  if (req.contains("testVersion")) {
    request.version = req["testVersion"];
  } else {
    request.version = "";
  }
  request.backend = req["backend"];
  // request.deployDevice=req["deployDevice"];
  request.deployOS = req["deployOS"];
  request.modelId = req["modelId"];
  // request.quantifyType=req["quantifyType"];
  logic.sdk()->createSDK(request, response);
  res["code"] = response.error.type;
  // res["msg"] = ""; //response.error.message;
  res["msg"] = response.error.message;

  json data = {{"deployInfo", response.deployInfo},
               {"modelName", response.modelName},
               {"algorithmType", response.algorithmType},
               {"sdkCreateTime", response.createTime},
               {"sdkDescription", response.description},
               {"sdkId", response.id},
               {"sdkMd5", response.Md5},
               {"sdkModifiedTime", response.modifiedTime},
               {"sdkName", response.name},
               {"sdkPath", response.path},
               {"sdkSize", response.size},
               {"sdkStatus", response.status},
               {"testVersion", response.version}};
  res["data"] = data;

  std::string msg;
  try {
    msg = res.dump();
  } catch (const std::exception& e) {
    std::cerr << "parse json error: " << e.what() << std::endl;
    json j_c;
    j_c["code"] = TurboLogic::ErrorType::ErrUnknown;
    std::string errMsg = std::string("pack response fail:") + e.what();
    j_c["msg"] = errMsg;
    j_c["data"] = json::array();
    return std::make_shared<TurboHttp::resMsg>(j_c.dump());
  };
  return std::make_shared<TurboHttp::resMsg>(res.dump());
}

/*
resMsg* SDKRouter::getSdk(std::string method, std::string host,
                          std::string proto, std::string path,
                          std::map<std::string, std::string> querys,
                          std::map<std::string, std::string> headers,
                          std::map<std::string, std::string> form,
                          std::string body)
*/
DEFINITION_ROUTER(SDKRouter, getSdk) {
  int projectId = 0;
  int sdkId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/sdks/%d", &projectId, &sdkId);
  if (converted == 2) {
    std::cout << "projectId:" << projectId << " sdkId:" << sdkId << std::endl;
  }
  json req;
  json res;

  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  res["code"] = TurboLogic::ErrorType::ErrUnknown;

  TurboLogic::GetSDKInfoRequest request;
  TurboLogic::GetSDKInfoResponse response;

  request.projectId = projectId;
  request.sdkId = sdkId;
  logic.sdk()->getSDKInfo(request, response);

  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    res["data"] = json::object();
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  res["data"] = json::object();
  res["data"]["deployInfo"] = response.deployInfo;
  res["data"]["modelId"] = response.modelId;
  res["data"]["modelName"] = response.modelName;
  res["data"]["algorithmType"] = response.algorithmType;
  res["data"]["sdkCreateTime"] = response.createTime;
  res["data"]["sdkDescription"] = response.description;
  res["data"]["sdkId"] = response.id;
  res["data"]["sdkMd5"] = response.Md5;
  res["data"]["sdkModifiedTime"] = response.modifiedTime;
  res["data"]["sdkName"] = response.name;
  res["data"]["sdkPath"] = response.path;
  res["data"]["sdkSize"] = response.size;
  res["data"]["sdkStatus"] = response.status;
  res["data"]["testId"] = response.testId;
  res["data"]["testVersion"] = response.version;
  res["data"]["quantifyType"] = response.quantifyType;

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
    return std::make_shared<TurboHttp::resMsg>(j_c.dump());
  };
  return std::make_shared<TurboHttp::resMsg>(res.dump());
}

/*
resMsg* SDKRouter::deleteSdk(std::string method, std::string host,
                             std::string proto, std::string path,
                             std::map<std::string, std::string> querys,
                             std::map<std::string, std::string> headers,
                             std::map<std::string, std::string> form,
                             std::string body)
*/
DEFINITION_ROUTER(SDKRouter, deleteSdk) {
  int projectId = 0;
  int sdkId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/sdks/%d", &projectId, &sdkId);
  if (converted == 2) {
    std::cout << "projectId:" << projectId << " sdkId:" << sdkId << std::endl;
  }
  json req;
  json res;

  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  res["code"] = TurboLogic::ErrorType::ErrUnknown;
  TurboLogic::DeleteSDKRequest request;
  TurboLogic::DeleteSDKResponse response;
  request.projectId = projectId;
  request.sdkId = sdkId;

  logic.sdk()->deleteSDK(request, response);

  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  return std::make_shared<TurboHttp::resMsg>(res.dump());
}

/*
resMsg* SDKRouter::cancelGenerateSdk(std::string method, std::string host,
                                     std::string proto, std::string path,
                                     std::map<std::string, std::string> querys,
                                     std::map<std::string, std::string> headers,
                                     std::map<std::string, std::string> form,
                                     std::string body)
 */
DEFINITION_ROUTER(SDKRouter, cancelGenerateSdk) {
  int projectId = 0;
  int sdkId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/sdks/%d/cancel_generate",
             &projectId, &sdkId);
  if (converted == 2) {
    std::cout << "projectId:" << projectId << " sdkId:" << sdkId << std::endl;
  }
  json req;
  json res;

  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  res["code"] = TurboLogic::ErrorType::ErrUnknown;
  TurboLogic::CancelSDKRequest request;
  TurboLogic::CancelSDKResponse response;
  request.projectId = projectId;
  request.sdkId = sdkId;
  logic.sdk()->cancelCreateSDK(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  return std::make_shared<TurboHttp::resMsg>(res.dump());
}

/*
resMsg* SDKRouter::getSdkLog(std::string method, std::string host,
                             std::string proto, std::string path,
                             std::map<std::string, std::string> querys,
                             std::map<std::string, std::string> headers,
                             std::map<std::string, std::string> form,
                             std::string body)
*/

DEFINITION_ROUTER(SDKRouter, getSdkLog) {
  int projectId = 0;
  int sdkId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/sdks/%d/sdk_log",
                         &projectId, &sdkId);
  if (converted == 2) {
    std::cout << "projectId:" << projectId << " sdkId:" << sdkId << std::endl;
  }
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  int level = 0;
  auto iter = querys.find("level");
  if (iter != querys.end()) {
    level = std::stoi(iter->second);
  } else {
    res["code"] = TurboLogic::ErrorType::ErrUnknown;
    res["msg"] = std::string("need level").data();
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  if (getLogLevel(level) == "") {
    res["code"] = TurboLogic::ErrorType::ErrUnknown;
    res["msg"] = std::string("need level").data();
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  res["code"] = TurboLogic::ErrorType::ErrUnknown;
  TurboLogic::GetSDKLogRequest request;
  TurboLogic::GetSDKLogResponse response;
  request.projectId = projectId;
  request.sdkId = sdkId;
  request.level = getLogLevel(level);
  logic.sdk()->getSDKLog(request, response);
  res["data"] = json::object();
  res["data"]["logs"] = json::array();

  for (auto log : response.logs) {
    json detail;
    detail = {{"content", log.content}, {"level", level},
              {"reason", log.reason},   {"suggestion", log.suggestion},
              {"time", log.time},       {"errorCode", log.errorCode}};
    res["data"]["logs"].push_back(detail);
  }

  res["code"] = response.error.type;
  res["msg"] = response.error.message;

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
    return std::make_shared<TurboHttp::resMsg>(j_c.dump());
  };
  return std::make_shared<TurboHttp::resMsg>(res.dump());
}
/*
resMsg* SDKRouter::getSdkProgress(std::string method, std::string host,
                                  std::string proto, std::string path,
                                  std::map<std::string, std::string> querys,
                                  std::map<std::string, std::string> headers,
                                  std::map<std::string, std::string> form,
                                  std::string body)
*/
DEFINITION_ROUTER(SDKRouter, getSdkProgress) {
  int projectId = 0;
  int sdkId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/sdks/%d/sdk_progress",
                         &projectId, &sdkId);
  if (converted == 2) {
    std::cout << "projectId:" << projectId << " sdkId:" << sdkId << std::endl;
  }
  json req;
  json res;

  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  res["code"] = TurboLogic::ErrorType::ErrUnknown;
  TurboLogic::GetSDKProgressRequest request;
  TurboLogic::GetSDKProgressResponse response;
  request.projectId = projectId;
  request.sdkId = sdkId;

  logic.sdk()->getSDKCreateProgress(request, response);

  res["code"] = response.error.type;
  res["msg"] = response.error.message;

  json data = {
      {"duration", response.duration},
      {"endTime", response.endTime},
      {"progress", response.progress},
      {"remainingTime", response.remainingTime},
      {"startTime", response.startTime},
      {"status", response.status},
  };
  res["data"] = data;

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
    return std::make_shared<TurboHttp::resMsg>(j_c.dump());
  };
  return std::make_shared<TurboHttp::resMsg>(res.dump());
}

/*
resMsg* SDKRouter::checkSdks(std::string method, std::string host,
                             std::string proto, std::string path,
                             std::map<std::string, std::string> querys,
                             std::map<std::string, std::string> headers,
                             std::map<std::string, std::string> form,
                             std::string body)
*/
DEFINITION_ROUTER(SDKRouter, checkSdks) {
  int projectId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/sdks/check", &projectId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  json req;
  json res;
  std::vector<std::string> needEntryList;

  needEntryList.push_back("backend");
  needEntryList.push_back("deployOS");
  needEntryList.push_back("modelId");

  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  /// res["code"] = TurboLogic::ErrorType::ErrUnknown;
  TurboLogic::CheckSDKRequest request;
  TurboLogic::CheckSDKResponse response;
  request.projectId = projectId;

  if (req.contains("deployDevice")) {
    request.deployDevice = req["deployDevice"];
  } else {
    request.deployDevice = "";
  }
  if (req.contains("quantifyType")) {
    request.quantifyType = req["quantifyType"];
  } else {
    request.quantifyType = "";
  }
  if (req.contains("testVersion")) {
    request.version = req["testVersion"];
  } else {
    request.version = "";
  }
  request.backend = req["backend"];

  request.deployOS = req["deployOS"];
  request.modelId = req["modelId"];

  logic.sdk()->checkSDK(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;

  json data = {{"generated", response.generated}};
  res["data"] = data;

  std::string msg;
  try {
    msg = res.dump();
  } catch (const std::exception& e) {
    std::cerr << "parse json error: " << e.what() << std::endl;
    json j_c;
    j_c["code"] = TurboLogic::ErrorType::ErrUnknown;
    std::string errMsg = std::string("pack response fail:") + e.what();
    j_c["msg"] = errMsg;
    j_c["data"] = json::object();
    return std::make_shared<TurboHttp::resMsg>(j_c.dump());
  };
  return std::make_shared<TurboHttp::resMsg>(res.dump());
}
