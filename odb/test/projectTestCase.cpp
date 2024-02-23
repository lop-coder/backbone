#include "projectTestCase.h"
#include <stdio.h>
#include <filesystem>
#include <iostream>
#include <memory>

void ProjectTestCase::SetUpTestCase() {
  try {
    auto root_path = std::filesystem::u8path(
        std::filesystem::current_path().string() + "/project");

    std::filesystem::remove_all(root_path);
    std::filesystem::create_directory(root_path);

    _project_hub = new ProjectHub(root_path.string());
    auto projects_db_path =
        std::filesystem::u8path(_project_hub->rootPath() + "/projects.db");
    std::shared_ptr<odb::sqlite::database> projects_db;
    if (!std::filesystem::exists(projects_db_path)) {
      DBProjectManagerOperate()->create(projects_db_path.string(), projects_db);
    } else {
      DBProjectManagerOperate()->load(projects_db_path.string(), projects_db);
    }
    auto workspace_path =
        std::filesystem::u8path(_project_hub->workspacePath());
    if (!std::filesystem::is_directory(workspace_path)) {
      std::filesystem::create_directory(workspace_path);
    }
    _project_hub->addDBPtr(_project_hub->projectManagerKey(), projects_db);
  } catch (const std::exception& e) {
    std::cerr << "ProjectTestCase::SetUpTestCase error:" << e.what()
              << std::endl;
  }
}
void ProjectTestCase::TearDownTestCase() {
  if (_project_hub) {
    delete _project_hub;
    _project_hub = nullptr;
  }
}
ProjectHub* ProjectTestCase::_project_hub = nullptr;
