#pragma once
#ifndef _ROUTER_PIPELINE_H_
#define _ROUTER_PIPELINE_H_

#include <string>

#include "annotation/annotationschema.h"
#include "dataset/datasetschema.h"
#include "filter/filterschema.h"
#include "http.h"
#include "image/imageschema.h"
#include "log/logschema.h"
#include "model/modelschema.h"
#include "nlohmann/json.hpp"
#include "project/projectschema.h"
#include "tag/tagschema.h"
using nlohmann::json;

class PipelineRouter {
 public:
  PipelineRouter();
  ~PipelineRouter();

  DECLARE_ROUTER(componentList);
  DECLARE_ROUTER(createProjectPipeline);
  DECLARE_ROUTER(listProjectPipelines);
  DECLARE_ROUTER(get);
  DECLARE_ROUTER(update);
  DECLARE_ROUTER(deletePipeline);
  DECLARE_ROUTER(createPipelineNode);
  DECLARE_ROUTER(getPipelineNode);
  DECLARE_ROUTER(updatePipelineNode);
  DECLARE_ROUTER(getPipelineNodeResult);
  DECLARE_ROUTER(getPipelineResult);
  DECLARE_ROUTER(runPipeline);
  DECLARE_ROUTER(createPipelineSdk);
  DECLARE_ROUTER(stopPipelineRun);
  DECLARE_ROUTER(singleRunPipeline);
  DECLARE_ROUTER(checkPipeline);
};

#endif  //_ROUTER_PIPELINE_H_
