#ifndef _ROUTER_TAG_H_
#define _ROUTER_TAG_H_

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

class TagRouter {
 public:
  TagRouter();
  ~TagRouter();

  DECLARE_ROUTER(listTrainModelTags);
  DECLARE_ROUTER(listTags);
  DECLARE_ROUTER(create);
  DECLARE_ROUTER(get);
  DECLARE_ROUTER(update);
  DECLARE_ROUTER(deleteTag);
  DECLARE_ROUTER(listTagsColor);
  DECLARE_ROUTER(merge);
};

#endif  //_ROUTER_TAG_H_
