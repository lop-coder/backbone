#include <stdio.h>

#include <iostream>
#include <memory>

#include <filesystem>
#include <iostream>
#include "projectTestCase.h"
////////////////////////////////////////////////////////////////////////////////////
TEST_F(ProjectTestCase, ProjectTest) {
  EXPECT_NE(_project_hub, nullptr)
      << "rootPath: " << _project_hub->rootPath() << std::endl
      << "workspacePath: " << _project_hub->workspacePath() << std::endl;
  EXPECT_NE(_project_hub->getDBPtr(_project_hub->projectManagerKey()), nullptr);

  //get projects db
  auto projects_db = _project_hub->getDBPtr(_project_hub->projectManagerKey());

  //add project
  auto addFunc = [](DBPtr const& db, odb::transaction& t) -> Error {
    std::shared_ptr<ODBORM::ProjectManager> prjItem;
    for (int i = 0; i < 5; i++) {
      ProjectManager()->add(db, std::to_string(i), "", prjItem);
      ProjectManager()->update(db, prjItem->id(), std::to_string(i),
                               _project_hub->projectDir(prjItem->id()));
    }
    return Error();
  };
  EXPECT_EQ(DBBaseOperateHandler(projects_db, addFunc, 2).type, Success);

  //get all project
  std::vector<ODBORM::ProjectManager> result;
  auto queryFunc = [&](DBPtr const& db, odb::transaction& t) -> Error {
    ProjectManager()->query(db, "", result);
    return Error();
  };
  EXPECT_EQ(DBBaseOperateHandler(projects_db, queryFunc, 2).type, Success);
  EXPECT_EQ(result.size(), 5);

  //create project db
  for (auto& item : result) {
    _project_hub->createDir(_project_hub->projectDir(item.id()));

    auto projectDBPath = TurboUtils::pathAppend(
        _project_hub->projectDir(item.id()), "project.db");

    std::shared_ptr<odb::sqlite::database> prjDB;
    prjDB.reset();
    EXPECT_NO_THROW(DBProjectOperate()->create(projectDBPath, prjDB));
    EXPECT_TRUE(
        _project_hub->addDBPtr(_project_hub->projectKey(item.id()), prjDB));
  }

  //load project db
  for (auto& item : result) {
    auto projectDBPath = TurboUtils::pathAppend(
        _project_hub->projectDir(item.id()), "project.db");
    std::shared_ptr<odb::sqlite::database> prjDB;
    EXPECT_NO_THROW(DBProjectOperate()->load(projectDBPath, prjDB));
    EXPECT_NE(_project_hub->getDBPtr(_project_hub->projectKey(item.id())),
              nullptr);
  }
  //erase project db
  for (auto& item : result) {
    auto projectDBPath = TurboUtils::pathAppend(
        _project_hub->projectDir(item.id()), "project.db");

    _project_hub->delDBPtr(_project_hub->projectKey(item.id()));
    EXPECT_EQ(_project_hub->getDBPtr(_project_hub->projectKey(item.id())),
              nullptr);
  }

  //get projects db
  EXPECT_NE(_project_hub->getDBPtr(_project_hub->projectManagerKey()), nullptr);

  //del project
  auto delFunc = [&](DBPtr const& db, odb::transaction& t) -> Error {
    std::shared_ptr<ODBORM::ProjectManager> prjItem;
    for (auto& item : result) {
      ProjectManager()->erase(db, item.id());
    }
    return Error();
  };
  EXPECT_EQ(DBBaseOperateHandler(projects_db, delFunc, 2).type, Success);
  EXPECT_EQ(DBBaseOperateHandler(projects_db, queryFunc, 2).type, Success);
  EXPECT_EQ(result.size(), 0);
  //get projects db
  EXPECT_NE(_project_hub->getDBPtr(_project_hub->projectManagerKey()), nullptr);
}
