#include "task.h"

#include <iostream>
#include <unordered_map>

#include "common.h"
#include "logic.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
TaskRouter::TaskRouter() {
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/async_tasks)",
                  TaskRouter::listProjectAsyncTasks);
  REGISTER_ROUTER("POST", "/v1/async_tasks", TaskRouter::listAllAsyncTasks);
  REGISTER_ROUTER("GET", R"(/v1/projects/async_tasks/(\d+))", TaskRouter::get);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/tasks)", TaskRouter::list);
  REGISTER_ROUTER("POST", "/v1/tasks", TaskRouter::listAllTasks);
}
TaskRouter::~TaskRouter() {}
////////////////////////////////////////////////////////////////////////////////////////////////////
/*
resMsg* TaskRouter::listProjectAsyncTasks(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(TaskRouter, listProjectAsyncTasks) {
  int projectId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/async_tasks", &projectId);
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

  TurboLogic::GetProjectAsyncTasksRequest request;
  TurboLogic::GetProjectAsyncTasksResponse response;
  request.projectId = projectId;
  logic.project()->getProjectAsyncTasks(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  res["data"]["tasks"] = json::array();
  for (auto taskInfo : response.tasksInfo) {
    json detail = {
        {"id", taskInfo.taskId},
        {"message", taskInfo.message},
        {"progress", taskInfo.progress},
        {"status", taskInfo.status},
    };
    res["data"]["tasks"].push_back(detail);
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};
/*
resMsg* TaskRouter::listAllAsyncTasks(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
    */
DEFINITION_ROUTER(TaskRouter, listAllAsyncTasks) {
  int converted = sscanf(path.c_str(), "/v1/async_tasks");
  if (converted == 1) {
    std::cout << "converted:" << converted << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::GetProjectAsyncTasksRequest request;
  TurboLogic::GetProjectAsyncTasksResponse response;
  request.projectId = -1;
  logic.project()->getProjectAsyncTasks(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  res["data"]["tasks"] = json::array();
  for (auto taskInfo : response.tasksInfo) {
    json detail = {
        {"id", taskInfo.taskId},
        {"message", taskInfo.message},
        {"progress", taskInfo.progress},
        {"status", taskInfo.status},
    };
    res["data"]["tasks"].push_back(detail);
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* TaskRouter::get(std::string method, std::string host, std::string proto,
                        std::string path,
                        std::map<std::string, std::string> querys,
                        std::map<std::string, std::string> headers,
                        std::map<std::string, std::string> form,
                        std::string body)
*/
DEFINITION_ROUTER(TaskRouter, get) {
  char taskId[256];
  if (path.size() > 300) {
    // sreturn
    throw std::exception("url too big ,it is not valid!");
  }
  int converted =
      sscanf_s(path.c_str(), "/v1/projects/async_tasks/%s", &taskId, 256);
  if (converted == 1) {
    std::cout << "taskId:" << taskId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";
  TurboLogic::GetProjectAsyncTasksRequest request;
  TurboLogic::GetProjectAsyncTasksResponse response;
  request.taskId = std::string(taskId);
  request.projectId = -1;
  logic.project()->getProjectAsyncTasks(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  if (response.tasksInfo.size() == 1) {
    auto taskInfo = response.tasksInfo.at(0);
    res["data"] = {
        {"id", taskInfo.taskId},           {"message", taskInfo.message},
        {"progress", taskInfo.progress},   {"status", taskInfo.status},
        {"projectId", taskInfo.projectId},
    };
  } else {
    res["data"]["tasks"] = json::array();
    for (auto taskInfo : response.tasksInfo) {
      json detail = {
          {"id", taskInfo.taskId},
          {"message", taskInfo.message},
          {"progress", taskInfo.progress},
          {"status", taskInfo.status},
      };
      res["data"]["tasks"].push_back(detail);
    }
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* TaskRouter::list(std::string method, std::string host,
                         std::string proto, std::string path,
                         std::map<std::string, std::string> querys,
                         std::map<std::string, std::string> headers,
                         std::map<std::string, std::string> form,
                         std::string body)
*/
DEFINITION_ROUTER(TaskRouter, list) {
  int projectId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/tasks", &projectId);
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

  TurboLogic::GetProjectTasksRequest request;
  TurboLogic::GetProjectTasksResponse response;
  if (!(req.contains("taskType")) || req["taskType"].size() == 0) {
    req["taskType"] = {TurboLogic::TaskType::GenerateSdk,
                       TurboLogic::TaskType::ModelTest,
                       TurboLogic::TaskType::ModelTrain};
  }
  for (auto& element : req["taskType"]) {
    if (!(checkArg(element, "TaskType", res))) {
      return std::make_shared<TurboHttp::resMsg>(res.dump());
    }
    request.tasksType.push_back(element);
  }
  request.projectId = projectId;
  logic.project()->getProjectTasks(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  if (response.tasksInfo.size() == 1) {
    res["data"]["tasks"] = json::array();
    for (auto taskInfo : response.tasksInfo) {
      json detail = {
          {"createTime", taskInfo.createTime},
          {"duration", taskInfo.duration},
          {"endTime", taskInfo.endTime},
          {"gpuNum", taskInfo.gpuNum},
          {"modelId", taskInfo.modelId},
          {"modelName", taskInfo.modelName},
          {"modifiedTime", taskInfo.modifiedTime},
          {"progressRate", taskInfo.progressRate},
          {"projectId", taskInfo.projectId},
          {"projectName", taskInfo.projectName},
          {"remainTime", taskInfo.remainTime},
          {"startTime", taskInfo.startTime},
          {"status", taskInfo.status},
          {"testId", taskInfo.testId},
          {"testVersion", taskInfo.testId},
          {"type", taskInfo.type},
          {"taskType", taskInfo.taskType},
      };
      res["data"]["tasks"].push_back(detail);
    }
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};
/*
resMsg* TaskRouter::listAllTasks(std::string method, std::string host,
                                 std::string proto, std::string path,
                                 std::map<std::string, std::string> querys,
                                 std::map<std::string, std::string> headers,
                                 std::map<std::string, std::string> form,
                                 std::string body)
*/
DEFINITION_ROUTER(TaskRouter, listAllTasks) {
  int converted = sscanf(path.c_str(), "/v1/tasks");
  if (converted == 1) {
    std::cout << "converted:" << converted << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::GetProjectTasksRequest request;
  TurboLogic::GetProjectTasksResponse response;
  if (!(req.contains("taskType")) || req["taskType"].size() == 0) {
    req["taskType"] = {TurboLogic::TaskType::GenerateSdk,
                       TurboLogic::TaskType::ModelTest,
                       TurboLogic::TaskType::ModelTrain};
  }
  for (auto& element : req["taskType"]) {
    if (!(checkArg(element, "TaskType", res))) {
      return std::make_shared<TurboHttp::resMsg>(res.dump());
    }
    request.tasksType.push_back(element);
  }
  request.projectId = -1;
  logic.project()->getProjectTasks(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  if (response.tasksInfo.size() == 1) {
    res["data"]["tasks"] = json::array();
    for (auto taskInfo : response.tasksInfo) {
      json detail = {
          {"createTime", taskInfo.createTime},
          {"duration", taskInfo.duration},
          {"endTime", taskInfo.endTime},
          {"gpuNum", taskInfo.gpuNum},
          {"modelId", taskInfo.modelId},
          {"modelName", taskInfo.modelName},
          {"modifiedTime", taskInfo.modifiedTime},
          {"progressRate", taskInfo.progressRate},
          {"projectId", taskInfo.projectId},
          {"projectName", taskInfo.projectName},
          {"remainTime", taskInfo.remainTime},
          {"startTime", taskInfo.startTime},
          {"status", taskInfo.status},
          {"testId", taskInfo.testId},
          {"testVersion", taskInfo.testId},
          {"type", taskInfo.type},
          {"taskType", taskInfo.taskType},
      };
      res["data"]["tasks"].push_back(detail);
    }
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};
