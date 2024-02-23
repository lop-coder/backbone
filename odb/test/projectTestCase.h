#pragma once

#include <gtest/gtest.h>

#include "src/local/access/db_access_type.h"
#include "src/local/access/db_base_operate.h"
#include "src/local/access/db_log_access.h"
#include "src/local/access/db_model_operate.h"
#include "src/local/access/db_model_pred_access.h"
#include "src/local/access/db_model_trainboard_access.h"
#include "src/local/access/db_model_val_access.h"
#include "src/local/access/db_project_operate.h"
#include "src/local/access/db_projectmanager_access.h"
#include "src/local/access/db_projectmanager_operate.h"
#include "src/local/access/project_hub.h"
using namespace DBAccess;
class ProjectTestCase : public testing::Test {
 protected:
  static void SetUpTestCase();
  static void TearDownTestCase();
  static ProjectHub* _project_hub;
};
