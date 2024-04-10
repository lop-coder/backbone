#include "project.h"

#include <iostream>

#include "common.h"
#include "logic.h"
#include "schema.h"
////////////////////////////////////////////////////////////////////////////////////////////////////
ProjectRouter::ProjectRouter() {
  REGISTER_ROUTER("POST", R"(/v1/project/(\d+)/close)", ProjectRouter::close);
  REGISTER_ROUTER("POST", R"(/v1/project/(\d+)/open)", ProjectRouter::open);
  REGISTER_ROUTEREX("GET", "/v1/projects", ProjectRouter::list, false);
  REGISTER_ROUTER("POST", "/v1/projects", ProjectRouter::create);
  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+))", ProjectRouter::get);
  REGISTER_ROUTER("PUT", R"(/v1/projects/(\d+))", ProjectRouter::update);
  REGISTER_ROUTER("DELETE", R"(/v1/projects/(\d+))",
                  ProjectRouter::deleteProject);
  REGISTER_ROUTEREX("GET", "/v1/turbo_configs", ProjectRouter::getTurboConfigs,
                    false);
  REGISTER_ROUTER("PUT", "/v1/turbo_configs",
                  ProjectRouter::updateTurboConfigs);
  REGISTER_ROUTEREX("GET", "/v1/path_storage", ProjectRouter::getPathStorage,
                    false);
  REGISTER_ROUTEREX("GET", "/v1/pre_close_check", ProjectRouter::closePreCheck,
                    false);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/check_object)",
                  ProjectRouter::checkProjectObject);
  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/status)",
                  ProjectRouter::checkProjectStatus);
  REGISTER_ROUTEREX("GET", "/v1/uecode", ProjectRouter::getUECode, false);
  REGISTER_ROUTEREX("POST", "/v1/activate", ProjectRouter::activate, false);
  REGISTER_ROUTEREX("GET", "/v1/activation", ProjectRouter::getActivationInfo,
                    false);
  REGISTER_ROUTER("POST", "/v1/crop_resize_preview",
                  ProjectRouter::cropReiszeDatasetImagesPreview);
  REGISTER_ROUTER("POST", "/v1/cancel_datasetimagesample_query",
                  ProjectRouter::cancelDatasetImageSampleQuery);
  REGISTER_ROUTER("POST", "/v1/projects/:projectId/export",
                  ProjectRouter::exportProject);
  REGISTER_ROUTER("POST", "/v1/projects/import", ProjectRouter::importProject);
  REGISTER_ROUTER("POST", "/v1/projects/:projectId/export/cancel",
                  ProjectRouter::cancelExportProject);
  REGISTER_ROUTER("GET", "/v1/projects/:projectId/export/progress",
                  ProjectRouter::getExportProjectProgress);
  REGISTER_ROUTER("POST", "/v1/projects/import/cancel",
                  ProjectRouter::cancelImportProject);
  REGISTER_ROUTER("GET", "/v1/projects/import/progress",
                  ProjectRouter::getImportProjectProgress);
  REGISTER_ROUTER("POST", "/v1/log/export", ProjectRouter::exportLog);
  REGISTER_ROUTER("POST", "/v1/line_to_polygon",
                  ProjectRouter::translateLine2Polygon);
}

ProjectRouter::~ProjectRouter() {}
////////////////////////////////////////////////////////////////////////////////////////////////////

/*
resMsg* ProjectRouter::close(std::string method, std::string host,
                             std::string proto, std::string path,
                             std::map<std::string, std::string> querys,
                             std::map<std::string, std::string> headers,
                             std::map<std::string, std::string> form,
                             std::string body)

*/
DEFINITION_ROUTER(ProjectRouter, close) {
  int id = 0;
  int converted = sscanf(path.c_str(), "/v1/project/%d/close", &id);
  if (converted == 1) {
    std::cout << "id:" << id << std::endl;
  }
  json req;
  json res;
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  res["code"] = TurboLogic::ErrorType::ErrUnknown;
  TurboLogic::CloseProjectRequest request;
  TurboLogic::CloseProjectResponse response;
  request.projectId = id;
  logic.project()->closeProject(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* ProjectRouter::open(std::string method, std::string host,
                            std::string proto, std::string path,
                            std::map<std::string, std::string> querys,
                            std::map<std::string, std::string> headers,
                            std::map<std::string, std::string> form,
                            std::string body)
*/
DEFINITION_ROUTER(ProjectRouter, open) {
  int id = 0;
  int converted = sscanf(path.c_str(), "/v1/project/%d/open", &id);
  if (converted == 1) {
    std::cout << "id:" << id << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["code"] = TurboLogic::ErrorType::ErrUnknown;
  res["data"] = json::object();
  res["msg"] = "";
  TurboLogic::OpenProjectRequest request;
  TurboLogic::OpenProjectResponse response;
  request.projectId = id;
  logic.project()->openProject(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    res["data"] = json::object();
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  res["data"] = {{"projectCreateTime", response.projectCreateTime},
                 {"projectDescription", response.projectDescription},
                 {"projectId", response.projectId},
                 {"projectModifiedTime", response.projectModifiedTime},
                 {"projectName", response.projectName},
                 {"projectPath", response.projectPath},
                 {"projectStatus", response.projectStatus}};
  std::string msg;
  try {
    msg = res.dump(2);
  } catch (const std::exception& e) {
    std::cerr << "parse json error: " << e.what() << std::endl;
    json j_c;
    j_c["code"] = TurboLogic::ErrorType::ErrUnknown;
    std::string errMsg = "pack response fail";
    j_c["msg"] = errMsg;
    j_c["data"] = json::object();
    return std::make_shared<TurboHttp::resMsg>(j_c.dump());
  };
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* ProjectRouter::deleteProject(std::string method, std::string host,
                                     std::string proto, std::string path,
                                     std::map<std::string, std::string> querys,
                                     std::map<std::string, std::string> headers,
                                     std::map<std::string, std::string> form,
                                     std::string body)
*/
DEFINITION_ROUTER(ProjectRouter, deleteProject) {
  int id = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d", &id);
  if (converted == 1) {
    std::cout << "id:" << id << std::endl;
  }
  json req;
  json res;
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  res["code"] = TurboLogic::ErrorType::ErrUnknown;
  TurboLogic::DeleteProjectRequest request;
  TurboLogic::DeleteProjectResponse response;
  request.projectId = id;
  logic.project()->deleteProject(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* ProjectRouter::update(std::string method, std::string host,
                              std::string proto, std::string path,
                              std::map<std::string, std::string> querys,
                              std::map<std::string, std::string> headers,
                              std::map<std::string, std::string> form,
                              std::string body)
*/
DEFINITION_ROUTER(ProjectRouter, update)

{
  int id = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d", &id);
  if (converted == 1) {
    std::cout << "id:" << id << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["code"] = TurboLogic::ErrorType::ErrUnknown;
  json schema = schemaOfRoot();

  schema["properties"]["projectName"] = schemaOfName();
  schema["properties"]["projectDescription"] = schemaOfDescription();
  schema["required"] = {"projectName"};
  if (!jsonParseAndSchemaCheck(body, schema, req, res, true)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  TurboLogic::UpdateProjectInfoRequest request;
  TurboLogic::UpdateProjectInfoResponse response;
  request.projectId = id;

  // 为了满足缺省更新，先查后更新。
  TurboLogic::GetProjectInfoRequest getRequest;
  TurboLogic::GetProjectInfoResponse getResponse;
  getRequest.projectId = id;
  logic.project()->getProjectInfo(getRequest, getResponse);
  if (getResponse.error.type != TurboLogic::ErrorType::Success) {
    res["data"] = json::object();
    res["msg"] = getResponse.error.message;
    res["code"] = getResponse.error.type;
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  if (req.contains("projectDescription")) {
    request.projectDescription = req["projectDescription"];
  } else {
    request.projectDescription = getResponse.projectDescription;
  }

  if (req.contains("projectName")) {
    request.projectName = req["projectName"];
  } else {
    request.projectName = getResponse.projectName;
  }
  logic.project()->updateProjectInfo(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    res["data"] = json::object();
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  res["data"] = {
      {"projectCreateTime", response.projectDetail.projectCreateTime},
      {"projectDescription", response.projectDetail.projectDescription},
      {"projectId", response.projectDetail.projectId},
      {"projectModifiedTime", response.projectDetail.projectModifiedTime},
      {"projectName", response.projectDetail.projectName},
      {"projectPath", response.projectDetail.projectPath},
      {"projectStatus", response.projectDetail.projectStatus}};
  std::string msg;
  try {
    msg = res.dump();
  } catch (const std::exception& e) {
    std::cerr << "parse json error: " << e.what() << std::endl;
    json j_c;
    j_c["code"] = TurboLogic::ErrorType::ErrUnknown;
    std::string errMsg = "pack response fail";
    j_c["msg"] = errMsg;
    j_c["data"] = json::object();
    return std::make_shared<TurboHttp::resMsg>(j_c.dump());
  };
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* ProjectRouter::get(std::string method, std::string host,
                           std::string proto, std::string path,
                           std::map<std::string, std::string> querys,
                           std::map<std::string, std::string> headers,
                           std::map<std::string, std::string> form,
                           std::string body)
 */
DEFINITION_ROUTER(ProjectRouter, get)

{
  int id = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d", &id);
  if (converted == 1) {
    std::cout << "id:" << id << std::endl;
  }
  json req;
  json res;
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  res["code"] = TurboLogic::ErrorType::ErrUnknown;
  if (body != "") {
    try {
      req = json::parse(body);
    } catch (const std::exception& e) {
      std::string errMsg = "parse request body fail";
      res["msg"] = errMsg;
      return std::make_shared<TurboHttp::resMsg>(res.dump());
    };
  }
  TurboLogic::GetProjectInfoRequest request;
  TurboLogic::GetProjectInfoResponse response;
  request.projectId = id;
  logic.project()->getProjectInfo(request, response);
  if (response.error.type != TurboLogic::ErrorType::Success) {
    res["data"] = json::object();
    res["msg"] = response.error.message;
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  res["data"] = {{"projectCreateTime", response.projectCreateTime},
                 {"projectDescription", response.projectDescription},
                 {"projectId", response.projectId},
                 {"projectModifiedTime", response.projectModifiedTime},
                 {"projectName", response.projectName},
                 {"projectPath", response.projectPath},
                 {"projectStatus", response.projectStatus}};
  std::string msg;
  try {
    msg = res.dump();
  } catch (const std::exception& e) {
    std::cerr << "parse json error: " << e.what() << std::endl;
    json j_c;
    j_c["code"] = TurboLogic::ErrorType::ErrUnknown;
    std::string errMsg = "pack response fail";
    j_c["msg"] = errMsg;
    j_c["data"] = json::object();
    return std::make_shared<TurboHttp::resMsg>(j_c.dump());
  };

  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* ProjectRouter::create(std::string method, std::string host,
                              std::string proto, std::string path,
                              std::map<std::string, std::string> querys,
                              std::map<std::string, std::string> headers,
                              std::map<std::string, std::string> form,
                              std::string body)
*/
DEFINITION_ROUTER(ProjectRouter, create) {
  int id = 0;
  int converted = sscanf(path.c_str(), "/v1/projects");
  if (converted == 1) {
    std::cout << "id:" << id << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  json schema = schemaOfRoot();

  schema["properties"]["projectName"] = schemaOfName();
  schema["properties"]["projectDescription"] = schemaOfDescription();
  schema["properties"]["projectDescription"]["default"] = "";
  schema["required"] = {"projectName"};
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  if (!jsonParseAndSchemaCheck(body, schema, req, res, true)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  TurboLogic::CreateProjectRequest request;
  TurboLogic::CreateProjectResponse response;

  request.projectDescription = req["projectDescription"];
  request.projectName = req["projectName"];
  logic.project()->createProject(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    res["data"] = json::object();
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  res["data"] = {{"projectCreateTime", response.projectCreateTime},
                 {"projectDescription", response.projectDescription},
                 {"projectId", response.projectId},
                 {"projectModifiedTime", response.projectModifiedTime},
                 {"projectName", response.projectName},
                 {"projectPath", response.projectPath},
                 {"projectStatus", response.projectStatus}};
  std::string msg;
  try {
    msg = res.dump();
  } catch (const std::exception& e) {
    std::cerr << "parse json error: " << e.what() << std::endl;
    json j_c;
    j_c["code"] = TurboLogic::ErrorType::ErrUnknown;
    std::string errMsg = "pack response fail";
    j_c["msg"] = errMsg;
    j_c["data"] = json::object();
    return std::make_shared<TurboHttp::resMsg>(j_c.dump());
  };
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* ProjectRouter::list(std::string method, std::string host,
                            std::string proto, std::string path,
                            std::map<std::string, std::string> querys,
                            std::map<std::string, std::string> headers,
                            std::map<std::string, std::string> form,
                            std::string body)
*/
DEFINITION_ROUTER(ProjectRouter, list) {
  int id = 0;
  int converted = sscanf(path.c_str(), "/v1/projects");
  if (converted == 1) {
    std::cout << "id:" << id << std::endl;
  }

  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";
  TurboLogic::ListProjectsRequest request;
  TurboLogic::ListProjectsResponse response;
  request.offset = 0;
  request.limit = 20;
  request.order_by = 0;

  if (querys.find("offset") != querys.end()) {
    if (!isDigits(querys.find("offset")->second)) {
      res["code"] = TurboLogic::ErrorType::ErrInvalidArg;
      res["msg"] = "offset type is invalid!";
      return std::make_shared<TurboHttp::resMsg>(res.dump());
    }
    request.offset = atoi(querys.find("offset")->second.c_str());
  }
  if (querys.find("limit") != querys.end()) {
    if (!isDigits(querys.find("limit")->second)) {
      res["code"] = TurboLogic::ErrorType::ErrInvalidArg;
      res["msg"] = "limit type is invalid!";
      return std::make_shared<TurboHttp::resMsg>(res.dump());
    }
    request.limit = atoi(querys.find("limit")->second.c_str());
  }
  if (querys.find("order_by") != querys.end()) {
    if (!isDigits(querys.find("order_by")->second)) {
      res["code"] = TurboLogic::ErrorType::ErrInvalidArg;
      res["msg"] = "order_by type is invalid!";
      return std::make_shared<TurboHttp::resMsg>(res.dump());
    }
    request.order_by = atoi(querys.find("order_by")->second.c_str());
  }
  if (querys.find("name") != querys.end()) {
    if (!isString(querys.find("name")->second)) {
      res["code"] = TurboLogic::ErrorType::ErrInvalidArg;
      res["msg"] = "name type is invalid!";
      return std::make_shared<TurboHttp::resMsg>(res.dump());
    }
    request.name = querys.find("name")->second;
  }
  logic.project()->listProjects(request, response);
  std::cout << "logic.project(): " << request.order_by << std::endl;
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    res["data"] = json::object();
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  res["data"] = json::object();
  res["data"]["projects"] = json::array();
  res["data"]["total"] = response.total;
  res["data"]["resultCount"] = 0;
  for (auto project : response.projects) {
    json detail;
    detail = {{"projectCreateTime", project.projectCreateTime},
              {"projectDescription", project.projectDescription},
              {"projectId", project.projectId},
              {"projectModifiedTime", project.projectModifiedTime},
              {"projectName", project.projectName},
              {"projectPath", project.projectPath},
              {"projectStatus", project.projectStatus}};
    res["data"]["projects"].push_back(detail);
    res["data"]["resultCount"] = res["data"]["resultCount"] + 1;
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
};

/*
resMsg* ProjectRouter::getTurboConfigs(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
 */
DEFINITION_ROUTER(ProjectRouter, getTurboConfigs) {
  int id = 0;
  int converted = sscanf(path.c_str(), "/v1/turbo_configs");
  if (converted == 1) {
    std::cout << "id:" << id << std::endl;
  }

  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";
  TurboLogic::GetTurboConfigsResponse response;

  logic.project()->getTurboConfigs(response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    res["data"] = json::object();
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  res["data"] = json::object();
  res["data"]["config"] = json::object();
  res["data"]["config"]["workspace"] = response.config.workspace;

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* ProjectRouter::updateTurboConfigs(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
 */
DEFINITION_ROUTER(ProjectRouter, updateTurboConfigs) {
  int id = 0;
  int converted = sscanf(path.c_str(), "/v1/turbo_configs");
  if (converted == 1) {
    std::cout << "id:" << id << std::endl;
  }
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";
  json reqExample;
  reqExample = {
      {"workspace", "workspace"},
  };

  std::vector<std::string> needEntryList;
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  TurboLogic::TurboConfig request;
  TurboLogic::GetTurboConfigsResponse preresponse;
  logic.project()->getTurboConfigs(preresponse);
  if (req.contains("workspace")) {
    request.workspace = req["workspace"].get<std::string>();
  } else {
    request.workspace = preresponse.config.workspace;
  }
  TurboLogic::GetTurboConfigsResponse response;

  logic.project()->updateTurboConfigs(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    res["data"] = json::object();
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  res["data"] = json::object();
  res["data"]["config"] = json::object();
  res["data"]["config"]["workspace"] = response.config.workspace;

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* ProjectRouter::getPathStorage(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(ProjectRouter, getPathStorage) {
  int id = 0;
  int converted = sscanf(path.c_str(), "/v1/path_storage");
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
  std::string p = "";
  if (querys.find("path") != querys.end()) {
    p = querys.find("path")->second;
  }
  TurboLogic::GetPathStorageResponse response;

  logic.project()->getPathStorage(p, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    res["data"] = json::object();
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  res["data"] = json::object();
  res["data"]["enableCapacity"] = response.enableCapacity;
  res["data"]["path"] = response.path;
  res["data"]["totalCapacity"] = response.totalCapacity;
  res["data"]["usageCapacity"] = response.usageCapacity;

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* ProjectRouter::closePreCheck(std::string method, std::string host,
                                     std::string proto, std::string path,
                                     std::map<std::string, std::string> querys,
                                     std::map<std::string, std::string> headers,
                                     std::map<std::string, std::string> form,
                                     std::string body)
*/
DEFINITION_ROUTER(ProjectRouter, closePreCheck) {
  int id = 0;
  int converted = sscanf(path.c_str(), "/v1/pre_close_check");
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
  int projectId = 0;
  if (querys.find("projectId") != querys.end()) {
    if (!isDigits(querys.find("projectId")->second)) {
      res["code"] = TurboLogic::ErrorType::ErrInvalidArg;
      res["msg"] = "projectId type is invalid!";
      return std::make_shared<TurboHttp::resMsg>(res.dump());
    }
    projectId = atoi(querys.find("projectId")->second.c_str());
  }

  TurboLogic::ClosePreCheckResponse response;

  logic.project()->closePreCheck(response, projectId);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    res["data"] = json::object();
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  res["data"] = json::object();
  res["data"]["isOk"] = response.isOk;
  res["data"]["suggestion"] = response.suggestion;

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* ProjectRouter::checkProjectObject(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(ProjectRouter, checkProjectObject) {
  int projectId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/check_object", &projectId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  json reqExample;
  reqExample = {{"id", 1}, {"type", "type"}};
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  std::vector<std::string> needEntryList;
  needEntryList.push_back("id");
  needEntryList.push_back("type");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  TurboLogic::CheckProjectObjectRequest request;
  TurboLogic::CheckProjectObjectResponse response;

  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  request.projectId = projectId;
  request.id = req["id"].get<int>();
  request.type = req["type"].get<std::string>();
  logic.project()->checkProjectObject(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  res["data"] = json::object();
  res["data"] = {{"isOk", response.isOk}, {"sugguestion", response.suggestion}};
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* ProjectRouter::checkProjectStatus(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(ProjectRouter, checkProjectStatus) {
  int projectId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/status", &projectId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);

  json res;

  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";
  TurboLogic::GetProjectInfoRequest request;
  TurboLogic::CheckProjectObjectResponse response;

  request.projectId = projectId;
  logic.project()->checkProjectStatus(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  res["data"] = json::object();
  res["data"] = {{"isOk", response.isOk}, {"sugguestion", response.suggestion}};
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* ProjectRouter::getUECode(std::string method, std::string host,
                                 std::string proto, std::string path,
                                 std::map<std::string, std::string> querys,
                                 std::map<std::string, std::string> headers,
                                 std::map<std::string, std::string> form,
                                 std::string body)
 */
DEFINITION_ROUTER(ProjectRouter, getUECode) {
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);

  json res;

  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";
  TurboLogic::GetUECodeResponse response;
  logic.project()->getUECode(response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  res["data"] = json::object();
  res["data"] = {{"UECode", response.UECode}};
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* ProjectRouter::activate(std::string method, std::string host,
                                std::string proto, std::string path,
                                std::map<std::string, std::string> querys,
                                std::map<std::string, std::string> headers,
                                std::map<std::string, std::string> form,
                                std::string body)
*/
DEFINITION_ROUTER(ProjectRouter, activate) {
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);

  json res;

  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";
  json reqExample;
  json req;
  reqExample = {{"licensePath", "licensePath"}};

  std::vector<std::string> needEntryList;
  needEntryList.push_back("licensePath");
  //needEntryList.push_back("CDKey");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  TurboLogic::ActivateRequest request;
  TurboLogic::ActivateResponse response;
  request.licensePath = req["licensePath"].get<std::string>();
  logic.project()->activate(request, response);
  // update verify_status
  TurboHttp::defaultHttpServer()->authentication(
      checkVerifyStatus(response.authorizationStatus));
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  res["data"] = json::object();
  res["data"] = {{"authorizationStatus", response.authorizationStatus},
                 {"authorizationTime", response.authorizationTime},
                 {"authorizationType", response.authorizationType},
                 {"expireDate", response.expireDate}};
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* ProjectRouter::getActivationInfo(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
 */
DEFINITION_ROUTER(ProjectRouter, getActivationInfo) {
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);

  json res;

  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::ActivateResponse response;
  logic.project()->getActivateInfo(response);
  // update verify_status
  TurboHttp::defaultHttpServer()->authentication(
      checkVerifyStatus(response.authorizationStatus));
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  res["data"] = json::object();
  res["data"] = {{"authorizationStatus", response.authorizationStatus},
                 {"authorizationTime", response.authorizationTime},
                 {"authorizationType", response.authorizationType},
                 {"expireDate", response.expireDate}};
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* ProjectRouter::cropReiszeDatasetImagesPreview(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
 */
DEFINITION_ROUTER(ProjectRouter, cropReiszeDatasetImagesPreview) {
  json req;
  json res;
  json reqExample;
  reqExample = {{"imagePath", "imagePath"}, {"isCenterPos", false},
                {"cropRect", {1, 2}},       {"resizeHeight", 1},
                {"resizeWidth", 1},         {"channelMode", "channelMode"},
                {"isCrop", false},          {"isResize", false},
                {"isKeepRatio", false}};
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";
  std::vector<std::string> needEntryList;
  needEntryList.push_back("imagePath");
  needEntryList.push_back("isCenterPos");
  needEntryList.push_back("isCrop");
  needEntryList.push_back("isResize");
  needEntryList.push_back("isKeepRatio");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  TurboLogic::CropReiszeDatasetImagesPreviewRequest request;
  TurboLogic::CropReiszeDatasetImagesPreviewResponse response;
  if (req["isCrop"] == true &&
      (!req.contains("cropRect") || req["cropRect"].size() != 4)) {
    res["msg"] =
        "cropRect not exist or its size not equal 4  when isCrop is true!";
    res["code"] = TurboLogic::ErrorType::ErrInvalidArg;
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  if (req["isResize"] == true &&
      (!req.contains("resizeHeight") || !req.contains("resizeWidth"))) {
    res["msg"] =
        "resizeHeight not exist or resizeWidth not exist  when isResize is "
        "true!";
    res["code"] = TurboLogic::ErrorType::ErrInvalidArg;
    return std::make_shared<TurboHttp::resMsg>(res.dump());
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
  request.isKeepRatio = req["isKeepRatio"];
  request.isCrop = req["isCrop"];
  request.isResize = req["isResize"];
  request.isCenterPos = req["isCenterPos"];
  request.imagePath = req["imagePath"].get<std::string>();
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  logic.project()->cropReiszeDatasetImagesPreview(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  res["data"] = json::object();
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  res["data"]["cropResizeHeight"] = response.cropResizeHeight;
  res["data"]["cropResizeWidth"] = response.cropResizeWidth;
  res["data"]["cropResizeImagePath"] = response.cropResizeImagePath;
  res["data"]["height"] = response.height;
  res["data"]["imagePath"] = response.imagePath;
  res["data"]["width"] = response.width;
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* ProjectRouter::cancelDatasetImageSampleQuery(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
 */

DEFINITION_ROUTER(ProjectRouter, cancelDatasetImageSampleQuery) {
  json req;
  json res;

  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::cancelDatasetImageSamplesQueryResponse response;
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  logic.project()->cancelDatasetImageSamplesQuery(response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  res["data"] = json::object();
  res["data"]["isOk"] = response.isOk;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

DEFINITION_ROUTER(ProjectRouter, importProject) {
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  json schema = schemaOfRoot();

  schema["properties"]["filePath"] = schemaOfItem("string");
  schema["required"] = {"filePath"};
  if (!jsonParseAndSchemaCheck(body, schema, req, res, true)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  TurboLogic::ImportProjectRequest request;
  TurboLogic::ImportProjectResponse response;
  request.filePath = req["filePath"];
  logic.project()->importProject(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  res["data"] = {{"errCode", response.errCode},
                 {"errMsg", response.errMsg},
                 {"progress", response.progress},
                 {"status", response.status}};
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

DEFINITION_ROUTER(ProjectRouter, exportProject) {
  int projectId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/export", &projectId);
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

  schema["properties"]["exportPath"] = schemaOfItem("string");
  schema["properties"]["softwareVersion"] = schemaOfItem("string");
  schema["required"] = {"exportPath", "softwareVersion"};
  if (!jsonParseAndSchemaCheck(body, schema, req, res, true)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  TurboLogic::ExportProjectRequest request;
  TurboLogic::ExportProjectResponse response;
  request.projectId = projectId;
  request.exportPath = req["exportPath"];
  request.softwareVersion = req["softwareVersion"];
  logic.project()->exportProject(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  res["data"] = {{"errCode", response.errCode},
                 {"errMsg", response.errMsg},
                 {"progress", response.progress},
                 {"status", response.status}};
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
}

DEFINITION_ROUTER(ProjectRouter, cancelExportProject) {
  int projectId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/export/cancel", &projectId);
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

  TurboLogic::CancelExportProjectRequest request;
  TurboLogic::CancelExportProjectResponse response;
  request.projectId = projectId;
  logic.project()->cancelExportProject(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
}

DEFINITION_ROUTER(ProjectRouter, cancelImportProject) {
  int projectId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/import/cancel", &projectId);
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

  schema["properties"]["filePath"] = schemaOfItem("string");
  schema["required"] = {"filePath"};
  if (!jsonParseAndSchemaCheck(body, schema, req, res, true)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  TurboLogic::CancelImportProjectRequest request;
  TurboLogic::CancelImportProjectResponse response;
  request.filePath = req["filePath"];
  logic.project()->cancelImportProject(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
}

DEFINITION_ROUTER(ProjectRouter, getExportProjectProgress) {
  int projectId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/export/progress", &projectId);
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

  TurboLogic::GetExportProjectProgressRequest request;
  TurboLogic::GetExportProjectProgressResponse response;
  request.projectId = projectId;
  logic.project()->getExportProjectProgress(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  res["data"] = {{"errCode", response.errCode},
                 {"errMsg", response.errMsg},
                 {"progress", response.progress},
                 {"status", response.status}};
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
}

DEFINITION_ROUTER(ProjectRouter, getImportProjectProgress) {
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::GetImportProjectProgressRequest request;
  TurboLogic::GetImportProjectProgressResponse response;

  if (querys.find("filePath") != querys.end()) {
    if (!isString(querys.find("filePath")->second)) {
      res["code"] = TurboLogic::ErrorType::ErrInvalidArg;
      res["msg"] = "filePath type is invalid!";
      return std::make_shared<TurboHttp::resMsg>(res.dump());
    }
    request.filePath = querys.find("filePath")->second.c_str();
  }

  logic.project()->getImportProjectProgress(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  res["data"] = {{"errCode", response.errCode},
                 {"errMsg", response.errMsg},
                 {"progress", response.progress},
                 {"status", response.status}};
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
}

DEFINITION_ROUTER(ProjectRouter, exportLog) {
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  json schema = schemaOfRoot();
  schema["properties"]["exportPath"] = schemaOfItem("string");
  if (!jsonParseAndSchemaCheck(body, schema, req, res)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  TurboLogic::ExportLogRequest request;
  TurboLogic::ExportLogResponse response;

  if (req.contains("exportPath")) {
    request.exportPath = req["exportPath"];
  } else {
    request.exportPath = "";
  }

  logic.project()->exportLog(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  res["data"] = {
      {"errCode", response.errCode},
      {"errMsg", response.errMsg},
      {"status", (int)response.status},
  };
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
}

DEFINITION_ROUTER(ProjectRouter, translateLine2Polygon) {
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  json schema = schemaOfRoot();

  schema["properties"]["imageHeight"] = schemaOfItem("integer");
  schema["properties"]["imageWidth"] = schemaOfItem("integer");
  schema["properties"]["thickness"] = schemaOfItem("integer");
  auto points_on_Schema = schemaOfItem("array");
  points_on_Schema["items"] = schemaOfItem("number");
  points_on_Schema["uniqueItems"] = false;
  schema["properties"]["pointsOn"] = points_on_Schema;
  schema["required"] = {"imageHeight", "imageWidth", "thickness", "pointsOn"};
  if (!jsonParseAndSchemaCheck(body, schema, req, res, true)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  TurboLogic::TranslateLine2PolygonRequest request;
  TurboLogic::TranslateLine2PolygonResponse response;
  request.points_on = req["pointsOn"].get<std::vector<float>>();
  request.thickness = req["thickness"];
  request.imageWidth = req["imageWidth"];
  request.imageHeight = req["imageHeight"];
  logic.project()->translateLine2Polygon(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  res["data"]["annotations"] = response.annoDatas;
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};
