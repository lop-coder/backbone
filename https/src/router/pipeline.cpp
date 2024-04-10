#include "pipeline.h"

#include <filesystem>
#include <iostream>
#include <unordered_map>

#include "common.h"
#include "logic.h"
#include "schema.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
PipelineRouter::PipelineRouter() {
  REGISTER_ROUTER("GET", "/v1/component/list", PipelineRouter::componentList);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/pipelines)",
                  PipelineRouter::createProjectPipeline);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/pipelines/list)",
                  PipelineRouter::listProjectPipelines);
  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/pipelines/(\d+))",
                  PipelineRouter::get);
  REGISTER_ROUTER("PUT", R"(/v1/projects/(\d+)/pipelines/(\d+))",
                  PipelineRouter::update);
  REGISTER_ROUTER("DELETE", R"(/v1/projects/(\d+)/pipelines/(\d+))",
                  PipelineRouter::deletePipeline);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/pipelines/(\d+)/nodes)",
                  PipelineRouter::createPipelineNode);
  REGISTER_ROUTER("POST",
                  R"(/v1/projects/(\d+)/pipelines/(\d+)/nodes/"
                  "(\d+)/results)",
                  PipelineRouter::getPipelineNodeResult);

  REGISTER_ROUTER("GET",
                  R"(/v1/projects/(\d+)/pipelines/(\d+)/nodes/"
                  "(\d+))",
                  PipelineRouter::getPipelineNode);

  REGISTER_ROUTER("PUT",
                  R"(/v1/projects/(\d+)/pipelines/(\d+)/nodes/"
                  "(\d+))",
                  PipelineRouter::updatePipelineNode);

  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/pipelines/(\d+)/results)",
                  PipelineRouter::getPipelineResult);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/pipelines/(\d+)/run)",
                  PipelineRouter::runPipeline);
  REGISTER_ROUTER("POST", R"(/v1​/projects​/(\d+)/pipelines​/(\d+)/sdks)",
                  PipelineRouter::createPipelineSdk);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/pipelines/(\d+)/stop)",
                  PipelineRouter::stopPipelineRun);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/pipelines/(\d+)/single_run)",
                  PipelineRouter::singleRunPipeline);
  REGISTER_ROUTEREX("GET", R"(/v1/projects/(\d+)/pipelines/(\d+)/check)",
                    PipelineRouter::checkPipeline, false);
}
PipelineRouter::~PipelineRouter() {}
////////////////////////////////////////////////////////////////////////////////////////////////////

/*
resMsg* PipelineRouter::componentList(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/

DEFINITION_ROUTER(PipelineRouter, componentList) {
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";

  TurboLogic::ListComponentsResponse response;
  logic.pipeline()->listComponents(response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  res["data"]["components"] = json::array();
  for (auto taskInfo : response.components) {
    json detail = {
        {"filename", taskInfo.fileName},
        {"status", taskInfo.status},
        {"name", taskInfo.json["name"]},
        {"category", taskInfo.json["category"]},
        {"type", taskInfo.json["type"]},
        {"data", taskInfo.status ? taskInfo.json.dump() : taskInfo.e.what()},
    };
    res["data"]["components"].push_back(detail);
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* PipelineRouter::createProjectPipeline(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(PipelineRouter, createProjectPipeline) {
  int projectId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/pipelines", &projectId);
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

  schema["properties"]["pipelineDescription"] = schemaOfDescription();
  schema["properties"]["pipelineName"] = schemaOfItem("string");
  schema["required"].push_back("pipelineName");

  if (!jsonParseAndSchemaCheck(body, schema, req, res)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  TurboLogic::CreatePipelineRequest request;
  TurboLogic::CreatePipelineResponse response;

  if (req.contains("pipelineDescription")) {
    request.pipelineDescription = req["pipelineDescription"];
  } else {
    request.pipelineDescription = "";
  }
  request.pipelineName = req["pipelineName"];

  request.projectId = projectId;
  logic.pipeline()->createProjectPipeline(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  res["data"] = {
      {"pipelineConfig", response.pipelineInfo.pipelineConfig},
      {"pipelineCreateTime", response.pipelineInfo.pipelineCreateTime},
      {"pipelineDescription", response.pipelineInfo.pipelineDescription},
      {"pipelineEditable", response.pipelineInfo.pipelineEditable},
      {"pipelineId", response.pipelineInfo.pipelineId},
      {"pipelineIsReady", response.pipelineInfo.pipelineIsReady},
      {"pipelineModifiedTime", response.pipelineInfo.pipelineModifiedTime},
      {"pipelineName", response.pipelineInfo.pipelineName},
      {"pipelineStatus", response.pipelineInfo.pipelineStatus},
  };
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* PipelineRouter::listProjectPipelines(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(PipelineRouter, listProjectPipelines) {
  int projectId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/pipelines/list", &projectId);
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

  json reqExample;
  reqExample = {{"limit", 1}, {"pipelineName", "pipelineName"}, {"offset", 1}};

  std::vector<std::string> needEntryList;
  // needEntryList.push_back("pipelineName");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  TurboLogic::ListPipelinesRequest request;
  TurboLogic::ListPipelinesResponse response;

  if (req.contains("pipelineName")) {
    request.pipelineName = req["pipelineName"];
  } else {
    request.pipelineName = "";
  }
  if (req.contains("pageNum")) {
    request.pageNum = req["pageNum"];
  } else {
    request.pageNum = 1;
  }
  if (req.contains("pageSize")) {
    request.pageSize = req["pageSize"];
  } else {
    request.pageSize = 20;
  }

  request.projectId = projectId;
  logic.pipeline()->listProjectPipelines(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  res["data"]["pipelines"] = json::array();
  for (auto pipelineInfo : response.pipelines) {
    json detail = {
        {"pipelineConfig", pipelineInfo.pipelineConfig},
        {"pipelineCreateTime", pipelineInfo.pipelineCreateTime},
        {"pipelineDescription", pipelineInfo.pipelineDescription},
        {"pipelineEditable", pipelineInfo.pipelineEditable},
        {"pipelineId", pipelineInfo.pipelineId},
        {"pipelineIsReady", pipelineInfo.pipelineIsReady},
        {"pipelineModifiedTime", pipelineInfo.pipelineModifiedTime},
        {"pipelineName", pipelineInfo.pipelineName},
        {"pipelineStatus", pipelineInfo.pipelineStatus},
    };
    res["data"]["pipelines"].push_back(detail);
  }
  res["data"]["total"] = response.total;

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* PipelineRouter::get(std::string method, std::string host,
                            std::string proto, std::string path,
                            std::map<std::string, std::string> querys,
                            std::map<std::string, std::string> headers,
                            std::map<std::string, std::string> form,
                            std::string body)
 */
DEFINITION_ROUTER(PipelineRouter, get) {
  int projectId = 0;
  int pipelineId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/pipelines/%d",
                         &projectId, &pipelineId);
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

  TurboLogic::GetPipelineInfoRequest request;
  TurboLogic::GetPipelineInfoResponse response;
  request.projectId = projectId;
  request.pipelineId = pipelineId;
  logic.pipeline()->getPipelineInfo(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  std::cout << "-----pipelineStatus:" << response.pipelineInfo.pipelineStatus
            << std::endl;
  res["data"] = {
      {"pipelineConfig", response.pipelineInfo.pipelineConfig},
      {"pipelineCreateTime", response.pipelineInfo.pipelineCreateTime},
      {"pipelineDescription", response.pipelineInfo.pipelineDescription},
      {"pipelineEditable", response.pipelineInfo.pipelineEditable},
      {"pipelineId", response.pipelineInfo.pipelineId},
      {"pipelineIsReady", response.pipelineInfo.pipelineIsReady},
      {"pipelineModifiedTime", response.pipelineInfo.pipelineModifiedTime},
      {"pipelineName", response.pipelineInfo.pipelineName},
      {"pipelineStatus", response.pipelineInfo.pipelineStatus},
  };

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* PipelineRouter::update(std::string method, std::string host,
                               std::string proto, std::string path,
                               std::map<std::string, std::string> querys,
                               std::map<std::string, std::string> headers,
                               std::map<std::string, std::string> form,
                               std::string body)
*/
DEFINITION_ROUTER(PipelineRouter, update) {
  int projectId = 0;
  int pipelineId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/pipelines/%d",
                         &projectId, &pipelineId);
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

  json reqExample;
  reqExample = {{"pipelineDescription", "pipelineDescription"},
                {"pipelineName", "pipelineName"},
                {"pipelineConfig", "pipelineConfig"}};

  std::vector<std::string> needEntryList;
  // needEntryList.push_back("pipelineName");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  TurboLogic::UpdatePipelineRequest request;
  TurboLogic::UpdatePipelineResponse response;

  TurboLogic::GetPipelineInfoRequest getPipelineInfoRequest;
  TurboLogic::GetPipelineInfoResponse getPipelineInfoResponse;

  getPipelineInfoRequest.pipelineId = pipelineId;
  getPipelineInfoRequest.projectId = projectId;
  logic.pipeline()->getPipelineInfo(getPipelineInfoRequest,
                                    getPipelineInfoResponse);
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  if (req.contains("pipelineConfig")) {
    request.pipelineConfig = req["pipelineConfig"];
  } else {
    request.pipelineConfig =
        getPipelineInfoResponse.pipelineInfo.pipelineConfig;
  }

  if (req.contains("pipelineDescription")) {
    request.pipelineDescription = req["pipelineDescription"];
  } else {
    request.pipelineDescription =
        getPipelineInfoResponse.pipelineInfo.pipelineDescription;
  }

  if (req.contains("pipelineName")) {
    request.pipelineName = req["pipelineName"];
  } else {
    request.pipelineName = getPipelineInfoResponse.pipelineInfo.pipelineName;
  }

  request.projectId = projectId;
  request.pipelineId = pipelineId;
  logic.pipeline()->updatePipelineInfo(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  res["data"] = {
      {"pipelineConfig", response.pipelineInfo.pipelineConfig},
      {"pipelineCreateTime", response.pipelineInfo.pipelineCreateTime},
      {"pipelineDescription", response.pipelineInfo.pipelineDescription},
      {"pipelineEditable", response.pipelineInfo.pipelineEditable},
      {"pipelineId", response.pipelineInfo.pipelineId},
      {"pipelineIsReady", response.pipelineInfo.pipelineIsReady},
      {"pipelineModifiedTime", response.pipelineInfo.pipelineModifiedTime},
      {"pipelineName", response.pipelineInfo.pipelineName},
      {"pipelineStatus", response.pipelineInfo.pipelineStatus},
  };

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* PipelineRouter::deletePipeline(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(PipelineRouter, deletePipeline) {
  int projectId = 0;
  int pipelineId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/pipelines/%d",
                         &projectId, &pipelineId);
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

  TurboLogic::DeletePipelineRequest request;
  TurboLogic::DeletePipelineResponse response;
  request.projectId = projectId;
  request.pipelineId = pipelineId;
  logic.pipeline()->deletePipeline(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* PipelineRouter::createPipelineNode(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(PipelineRouter, createPipelineNode) {
  int projectId = 0;
  int pipelineId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/pipelines/%d/nodes",
                         &projectId, &pipelineId);
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

  TurboLogic::CreatePipelineNodeRequest request;
  TurboLogic::CreatePipelineNodeResponse response;
  request.projectId = projectId;
  request.pipelineId = pipelineId;
  logic.pipeline()->createPipelineNode(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;

  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  res["data"]["pipelineNodeId"] = response.pipelineNodeInfo.pipelineNodeId;

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* PipelineRouter::getPipelineNodeResult(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
 */
DEFINITION_ROUTER(PipelineRouter, getPipelineNodeResult) {
  int projectId = 0;
  int pipelineId = 0;
  int pipelineNodeId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/pipelines/%d/nodes/%d/results",
             &projectId, &pipelineId, &pipelineNodeId);
  if (converted == 1) {
    std::cout << "projectId:" << projectId << std::endl;
  }
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  try {
    res["data"] = json::object();
    res["code"] = TurboLogic::ErrorType::Success;
    res["msg"] = "";

    json reqExample;
    reqExample = {{"imagePath", "imagePath"}};

    std::vector<std::string> needEntryList;
    needEntryList.push_back("imagePath");
    if (!parseBody(body, req, res, needEntryList)) {
      return std::make_shared<TurboHttp::resMsg>(res.dump());
    }

    if (!checkRequest(req, reqExample, res)) {
      return std::make_shared<TurboHttp::resMsg>(res.dump());
    }

    TurboLogic::GetPipelineNodeResultRequest request;
    TurboLogic::GetPipelineNodeResultResponse response;
    request.projectId = projectId;
    request.pipelineId = pipelineId;
    request.pipelineNodeId = pipelineNodeId;
    request.imagePath = req["imagePath"];

    logic.pipeline()->getPipelineNodeResult(request, response);
    res["code"] = response.error.type;
    res["msg"] = response.error.message;

    if (response.error.type != TurboLogic::ErrorType::Success) {
      return std::make_shared<TurboHttp::resMsg>(res.dump());
    }

    res["data"] = json::object();
    res["data"]["imagePaths"] = json::array();
    res["data"]["gts"] = json::array();
    res["data"]["result"] = json::array();
    res["data"]["currentResult"] = json::array();
    res["data"]["predicts"] = json::array();
    res["data"]["visType"] = response.visType;

    res["data"]["imagePaths"].clear();
    for (auto iter : response.imagePaths) {
      res["data"]["imagePaths"].push_back(iter);
    }
    std::string path = req["imagePath"];
    std::string baseName = getFileBaseName(path);
    // res["data"]["gts"].clear();
    for (auto node : response.gts) {
      auto result = json::parse(node);
      if (result.is_object()) {
        result["extra_data"]["image_name"] = baseName;
        res["data"]["gts"].push_back(result);
      }
    }

    for (auto node : response.results) {
      printf("result: %s\n", node.data());
      auto result = json::parse(node);
      if (result.is_object()) {
        res["data"]["result"].push_back(result);
      }
      auto single_cls = json::array();
      if (result.contains("single_cls") && result["single_cls"].is_array()) {
        res["data"]["resultType"] = "single_cls";

        auto predictItem = json::object();
        auto currentResultJson = json::object();

        predictItem["predicts"] = json::array();
        predictItem["extra_data"] = json::object();
        predictItem["version"] = "v1.0";
        predictItem["type"] = "single_cls";
        predictItem["extra_data"]["image_height"] = result["meta"]["height"];
        predictItem["extra_data"]["image_width"] = result["meta"]["width"];

        predictItem["extra_data"]["image_name"] =
            result["uuid"].get<std::string>() + ".jpg";

        currentResultJson["row_name"] = json::array();
        currentResultJson["col_name"] = {"序号", "标签名", "标签ID", "置信度",
                                         "阈值"};
        currentResultJson["data"] = json::array();
        int index = 1;
        for (auto& [key, value] : result["single_cls"].items()) {
          if (value.contains("label") && value.contains("label_name") &&
              value.contains("score")) {
            auto item = json::object();
            item["score"] = value["score"];
            item["tag"] = value["label_name"];
            predictItem["predicts"].push_back(item);

            auto resultItem = json::array();
            resultItem.push_back(index);

            resultItem.push_back(value["label_name"]);
            resultItem.push_back(value["label"]);
            resultItem.push_back(value["score"]);
            resultItem.push_back(value["score_threshod"]);

            currentResultJson["data"].push_back(resultItem);

            index++;
          }
        }

        res["data"]["currentResult"].push_back(currentResultJson);
        res["data"]["predicts"].push_back(predictItem);
      }
    }
  } catch (const nlohmann::detail::parse_error& e) {
    /*
res["msg"] = "parse result fail";
res["code"] = TurboLogic::ErrorType::ErrUnknown;
*/
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* PipelineRouter::getPipelineNode(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(PipelineRouter, getPipelineNode) {
  int projectId = 0;
  int pipelineId = 0;
  int pipelineNodeId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/pipelines/%d/nodes/%d",
                         &projectId, &pipelineId, &pipelineNodeId);
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
  /*
json reqExample;
reqExample = {{"imagePath", "imagePath"}};

std::vector<std::string> needEntryList;
needEntryList.push_back("imagePath");
if (!parseBody(body, req, res, needEntryList)) {
return newResMsg(res["code"], std::string(res["msg"]).data(),
           res.dump(2).data());
}
if (!checkRequest(req, reqExample, res)) {
return newResMsg(res["code"], std::string(res["msg"]).data(),
           res.dump(2).data());
}
*/
  TurboLogic::GetPipelineNodeRequest request;
  TurboLogic::GetPipelineNodeResponse response;

  request.projectId = projectId;
  request.pipelineId = pipelineId;
  request.pipelineNodeId = pipelineNodeId;

  logic.pipeline()->getPipelineNode(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  res["data"] = json::object();
  res["data"]["gtVisSettings"] = response.gtVisSettings;
  res["data"]["visSettings"] = response.visSettings;

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* PipelineRouter::updatePipelineNode(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(PipelineRouter, updatePipelineNode) {
  int projectId = 0;
  int pipelineId = 0;
  int pipelineNodeId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/pipelines/%d/nodes/%d",
                         &projectId, &pipelineId, &pipelineNodeId);
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

  json reqExample;
  reqExample = {{"visSettings", "visSettings"}};

  std::vector<std::string> needEntryList;
  needEntryList.push_back("visSettings");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  if (!checkRequest(req, reqExample, res)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  TurboLogic::UpdatePipelineNodeRequest request;
  TurboLogic::UpdatePipelineNodeResponse response;
  request.projectId = projectId;
  request.pipelineId = pipelineId;
  request.pipelineNodeId = pipelineNodeId;
  request.visSettings = req["visSettings"];

  logic.pipeline()->updatePipelineNode(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  res["data"] = json::object();
  res["data"]["gtVisSettings"] = response.gtVisSettings;
  res["data"]["visSettings"] = response.visSettings;

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};
/*
resMsg* PipelineRouter::getPipelineResult(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(PipelineRouter, getPipelineResult) {
  int projectId = 0;
  int pipelineId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/pipelines/%d/results",
                         &projectId, &pipelineId);
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

  TurboLogic::GetPipelineResultRequest request;
  TurboLogic::GetPipelineResultResponse response;
  request.projectId = projectId;
  request.pipelineId = pipelineId;
  logic.pipeline()->getPipelineResult(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* PipelineRouter::runPipeline(std::string method, std::string host,
                                    std::string proto, std::string path,
                                    std::map<std::string, std::string> querys,
                                    std::map<std::string, std::string> headers,
                                    std::map<std::string, std::string> form,
                                    std::string body)
 */
DEFINITION_ROUTER(PipelineRouter, runPipeline) {
  int projectId = 0;
  int pipelineId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/pipelines/%d/run",
                         &projectId, &pipelineId);
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

  TurboLogic::RunPipelineRequest request;
  TurboLogic::RunPipelineResponse response;
  request.projectId = projectId;
  request.pipelineId = pipelineId;

  logic.pipeline()->runPipeline(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* PipelineRouter::createPipelineSdk(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
 */
DEFINITION_ROUTER(PipelineRouter, createPipelineSdk) {
  int projectId = 0;
  int pipelineId = 0;
  int converted =
      sscanf(path.c_str(), "/v1​/projects​/%d/pipelines​/%d/sdks",
             &projectId, &pipelineId);
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

  TurboLogic::CreatePipelineSDKRequest request;
  TurboLogic::CreatePipelineSDKResponse response;
  request.projectId = projectId;
  request.pipelineId = pipelineId;
  logic.pipeline()->createPipelineSdk(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* PipelineRouter::stopPipelineRun(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
 */
DEFINITION_ROUTER(PipelineRouter, stopPipelineRun) {
  int projectId = 0;
  int pipelineId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/pipelines/%d/stop",
                         &projectId, &pipelineId);
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

  TurboLogic::StopPipelineRunRequest request;
  TurboLogic::StopPipelineRunResponse response;
  request.projectId = projectId;
  logic.pipeline()->stopPipelineRun(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* PipelineRouter::singleRunPipeline(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(PipelineRouter, singleRunPipeline) {
  int projectId = 0;
  int pipelineId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/pipelines/%d/single_run",
             &projectId, &pipelineId);
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
  needEntryList.push_back("imagePath");
  if (!parseBody(body, req, res, needEntryList)) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }

  TurboLogic::SingleRunPipelineRequest request;
  TurboLogic::SingleRunPipelineResponse response;
  request.projectId = projectId;
  request.pipelineId = pipelineId;
  request.imagePath = req["imagePath"];

  logic.pipeline()->singleRunPipeline(request, response);

  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* PipelineRouter::checkPipeline(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(PipelineRouter, checkPipeline) {
  int projectId = 0;
  int pipelineId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/pipelines/%d/check",
                         &projectId, &pipelineId);
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

  TurboLogic::CheckPipelineConfigRequest request;
  TurboLogic::CheckPipelineConfigResponse response;
  request.projectId = projectId;
  request.pipelineId = pipelineId;

  logic.pipeline()->checkPipelineConfig(request, response);

  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<TurboHttp::resMsg>(res.dump());
  }
  res["data"] = json::object();
  res["data"]["results"] = json::array();

  for (auto node : response.status) {
    auto obj = json::object();
    obj["err_msg"] = node.msg;
    obj["node_id"] = node.nodeId;
    res["data"]["results"].push_back(obj);
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};
