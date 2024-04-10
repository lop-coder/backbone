#pragma once
#ifndef _ROUTER_SYSTEMINFO_H_
#define _ROUTER_SYSTEMINFO_H_

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

class SystemInfoRouter {
 public:
  SystemInfoRouter();
  ~SystemInfoRouter();

  DECLARE_ROUTER(hardware);
  DECLARE_ROUTER(systemMemory);
};

#endif  //_ROUTER_PIPELINE_H_
