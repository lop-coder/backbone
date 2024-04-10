#ifndef _ROUTER_PROJECT_H_
#define _ROUTER_PROJECT_H_

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

class ProjectRouter {
 public:
  ProjectRouter();
  ~ProjectRouter();

  DECLARE_ROUTER(close);
  DECLARE_ROUTER(open);
  DECLARE_ROUTER(list);
  DECLARE_ROUTER(create);
  DECLARE_ROUTER(get);
  DECLARE_ROUTER(update);
  DECLARE_ROUTER(deleteProject);
  DECLARE_ROUTER(getTurboConfigs);
  DECLARE_ROUTER(updateTurboConfigs);
  DECLARE_ROUTER(getPathStorage);
  DECLARE_ROUTER(closePreCheck);
  DECLARE_ROUTER(checkProjectObject);
  DECLARE_ROUTER(checkProjectStatus);
  DECLARE_ROUTER(activate);
  DECLARE_ROUTER(getUECode);
  DECLARE_ROUTER(getActivationInfo);
  DECLARE_ROUTER(cropReiszeDatasetImagesPreview);
  DECLARE_ROUTER(cancelDatasetImageSampleQuery);
  DECLARE_ROUTER(getImportProjectProgress);
  DECLARE_ROUTER(cancelImportProject);
  DECLARE_ROUTER(getExportProjectProgress);
  DECLARE_ROUTER(cancelExportProject);
  DECLARE_ROUTER(importProject);
  DECLARE_ROUTER(exportProject);
  DECLARE_ROUTER(exportLog);
  DECLARE_ROUTER(translateLine2Polygon);
};

#endif  //_ROUTER_PROJECT_H_
