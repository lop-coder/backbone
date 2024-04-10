#ifndef _ROUTER_SAM_H_
#define _ROUTER_SAM_H_

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

class SAMRouter {
 public:
  SAMRouter();
  ~SAMRouter();

  DECLARE_ROUTER(start);
  DECLARE_ROUTER(stop);
  DECLARE_ROUTER(embedding);
  DECLARE_ROUTER(segment);
};

#endif  //_ROUTER_TAG_H_
