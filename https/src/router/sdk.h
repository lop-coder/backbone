#ifndef _ROUTER_SDK_H_
#define _ROUTER_SDK_H_

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

class SDKRouter {
 public:
  SDKRouter();
  ~SDKRouter();

  DECLARE_ROUTER(getSdks);
  DECLARE_ROUTER(setSdks);
  DECLARE_ROUTER(getSdk);
  DECLARE_ROUTER(deleteSdk);
  DECLARE_ROUTER(cancelGenerateSdk);
  DECLARE_ROUTER(getSdkLog);
  DECLARE_ROUTER(getSdkProgress);
  DECLARE_ROUTER(checkSdks);
};

#endif  //_ROUTER_TAG_H_
