#include "tag.h"

#include <iostream>
#include <unordered_map>

#include "common.h"
#include "logic.h"

using namespace TurboHttp;
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
TagRouter::TagRouter() {
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/models/(\d+)/list_tags)",
                  TagRouter::listTrainModelTags);
  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/tags)", TagRouter::listTags);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/tags)", TagRouter::create);
  REGISTER_ROUTER("GET", R"(/v1/projects/(\d+)/tags/(\d+))", TagRouter::get);
  REGISTER_ROUTER("PUT", R"(/v1/projects/(\d+)/tags/(\d+))", TagRouter::update);
  REGISTER_ROUTER("DELETE", R"(/v1/projects/(\d+)/tags/(\d+))",
                  TagRouter::deleteTag);
  REGISTER_ROUTER("GET", "/v1/tag_colors", TagRouter::listTagsColor);
  REGISTER_ROUTER("POST", R"(/v1/projects/(\d+)/tags/(\d+)/merge_to)",
                  TagRouter::merge);
}
TagRouter::~TagRouter() {}
////////////////////////////////////////////////////////////////////////////////////////////////////
/*
std::shared_ptr<resMsg> TagRouter::listTrainModelTags(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,std::vector<std::string> matches,
    std::map<std::string, std::string> form, std::string body)
*/
DEFINITION_ROUTER(TagRouter, listTrainModelTags) {
  int projectId = 0;
  int modelId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/models/%d/list_tags",
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

  TurboLogic::GetDatasetTagsRequest getDatasetTagRequest;
  TurboLogic::GetDatasetTagsResponse getDatasetTagResponse;
  // map avoid duplicate
  std::unordered_map<int, int> tagIds;
  res["data"]["tags"] = json::array();

  for (auto datasetId : response.trainingDatasetIds) {
    getDatasetTagRequest.datasetId = datasetId;
    getDatasetTagRequest.projectId = projectId;
    logic.dataset()->getDatasetTags(getDatasetTagRequest,
                                    getDatasetTagResponse);
    if (getDatasetTagResponse.error.type != TurboLogic::ErrorType::Success) {
      return std::make_shared<resMsg>(res.dump());
    }
    for (TurboLogic::TagItem tag : getDatasetTagResponse.tags) {
      if (tagIds.find(tag.id) != tagIds.end()) {
        tagIds[tag.id] = 1;
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
    }
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
std::shared_ptr<resMsg> TagRouter::listTags(std::string method, std::string host,
                            std::string proto, std::string path,
                            std::map<std::string, std::string> querys,
                            std::map<std::string, std::string> headers,
                            std::vector<std::string> matches,
                            std::map<std::string, std::string> form,
                            std::string body)
*/
DEFINITION_ROUTER(TagRouter, listTags) {
  int projectId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/tags", &projectId);
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

  TurboLogic::ListTagsRequest request;
  TurboLogic::ListTagsResponse response;
  if (querys.find("name") != querys.end()) {
    if (!isString(querys.find("name")->second)) {
      res["code"] = TurboLogic::ErrorType::ErrInvalidArg;
      res["msg"] = "name type is invalid!";
      return std::make_shared<resMsg>(res.dump());
    }
    request.name = querys.find("name")->second;
  }

  request.projectId = projectId;
  logic.tag()->listTags(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  res["data"]["tags"] = json::array();
  for (TurboLogic::TagItem tag : response.items) {
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
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
std::shared_ptr<resMsg> TagRouter::create(std::string method, std::string host,
                          std::string proto, std::string path,
                          std::map<std::string, std::string> querys,
                          std::map<std::string, std::string> headers,
                          std::vector<std::string> matches,
                          std::map<std::string, std::string> form,
                          std::string body)
*/
DEFINITION_ROUTER(TagRouter, create) {
  int projectId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/tags", &projectId);
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

  TurboLogic::CreateTagRequest request;
  TurboLogic::CreateTagResponse response;

  if (req.contains("tagDescription")) {
    request.tagDescription = req["tagDescription"];
  } else {
    request.tagDescription = "";
  }
  request.projectId = projectId;
  request.tagName = req["tagName"];
  if (req.contains("tagColor")) {
    request.tagColor = req["tagColor"];
  } else {
    request.tagColor = "";
  }

  logic.tag()->createTag(request, response);
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
std::shared_ptr<resMsg> TagRouter::get(std::string method, std::string host, std::string proto,
                       std::string path,
                       std::map<std::string, std::string> querys,
                       std::map<std::string, std::string> headers,
                       std::vector<std::string> matches,
                       std::map<std::string, std::string> form,
                       std::string body)
*/
DEFINITION_ROUTER(TagRouter, get) {
  int projectId = 0;
  int tagId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/tags/%d", &projectId, &tagId);
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

  TurboLogic::GetTagInfoRequest request;
  TurboLogic::GetTagInfoResponse response;

  request.projectId = projectId;
  request.tagId = tagId;
  logic.tag()->getTagInfo(request, response);
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
std::shared_ptr<resMsg> TagRouter::update(std::string method, std::string host,
                          std::string proto, std::string path,
                          std::map<std::string, std::string> querys,
                          std::map<std::string, std::string> headers,
                          std::vector<std::string> matches,
                          std::map<std::string, std::string> form,
                          std::string body)
*/
DEFINITION_ROUTER(TagRouter, update) {
  int projectId = 0;
  int tagId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/tags/%d", &projectId, &tagId);
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
  TurboLogic::GetTagInfoRequest getTagInfoRequest;
  TurboLogic::GetTagInfoResponse getTagInfoResponse;

  getTagInfoRequest.projectId = projectId;
  getTagInfoRequest.tagId = tagId;
  logic.tag()->getTagInfo(getTagInfoRequest, getTagInfoResponse);
  res["code"] = getTagInfoResponse.error.type;
  res["msg"] = getTagInfoResponse.error.message;
  if (getTagInfoResponse.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  TurboLogic::UpdateTagInfoRequest request;
  TurboLogic::UpdateTagInfoResponse response;

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
  request.tagId = tagId;
  request.tagDescription = getTagInfoResponse.tagDescription;
  logic.tag()->updateTagInfo(request, response);
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
std::shared_ptr<resMsg> TagRouter::deleteTag(std::string method, std::string host,
                             std::string proto, std::string path,
                             std::map<std::string, std::string> querys,
                             std::map<std::string, std::string> headers,
                             std::vector<std::string> matches,
                             std::map<std::string, std::string> form,
                             std::string body)
*/
DEFINITION_ROUTER(TagRouter, deleteTag) {
  int projectId = 0;
  int tagId = 0;
  int converted =
      sscanf(path.c_str(), "/v1/projects/%d/tags/%d", &projectId, &tagId);
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

  TurboLogic::DeleteTagRequest request;
  TurboLogic::DeleteTagResponse response;

  request.projectId = projectId;
  request.tagId = tagId;
  logic.tag()->deleteTag(request, response);
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
std::shared_ptr<resMsg> TagRouter::listTagsColor(std::string method, std::string host,
                                 std::string proto, std::string path,
                                 std::map<std::string, std::string> querys,
                                 std::map<std::string, std::string> headers,
                                 std::vector<std::string> matches,
                                 std::map<std::string, std::string> form,
                                 std::string body)
*/
DEFINITION_ROUTER(TagRouter, listTagsColor) {
  int converted = sscanf(path.c_str(), "/v1/tag_colors");
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

  res["data"]["colors"] = json::object();

  auto IndexColorMap = TurboLogic::getIndexColorMap();
  for (auto firstIter = IndexColorMap.begin(); firstIter != IndexColorMap.end();
       firstIter++) {
    auto sencondMap = firstIter->second;
    for (auto sencondIter = sencondMap.begin(); sencondIter != sencondMap.end();
         sencondIter++)
      res["data"]["colors"][sencondIter->first] = sencondIter->second;
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};

/*
std::shared_ptr<resMsg> TagRouter::merge(std::string method, std::string host,
                         std::string proto, std::string path,
                         std::map<std::string, std::string> querys,
                         std::map<std::string, std::string> headers,
                         std::vector<std::string> matches,
                         std::map<std::string, std::string> form,
                         std::string body)
*/
DEFINITION_ROUTER(TagRouter, merge) {
  int projectId = 0;
  int tagId = 0;
  int converted = sscanf(path.c_str(), "/v1/projects/%d/tags/%d/merge_to",
                         &projectId, &tagId);
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
  TurboLogic::MergeTagRequest request;
  TurboLogic::MergeTagResponse response;

  request.projectId = projectId;
  request.tagId = tagId;
  request.mergeToTagId = req["tagId"].get<int>();
  logic.tag()->mergeTag(request, response);
  res["code"] = response.error.type;
  res["msg"] = response.error.message;
  if (response.error.type != TurboLogic::ErrorType::Success) {
    return std::make_shared<resMsg>(res.dump());
  }

  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<resMsg>(res.dump());
};
