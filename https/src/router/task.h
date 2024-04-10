#ifndef _ROUTER_TASK_H_
#define _ROUTER_TASK_H_

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

class TaskRouter {
 public:
  TaskRouter();
  ~TaskRouter();

  DECLARE_ROUTER(listProjectAsyncTasks);
  DECLARE_ROUTER(listAllAsyncTasks);
  DECLARE_ROUTER(get);
  DECLARE_ROUTER(list);
  DECLARE_ROUTER(listAllTasks);
};

#endif  //_ROUTER_TASK_H_
