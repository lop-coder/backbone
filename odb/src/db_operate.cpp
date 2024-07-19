
#include "db_operate.h"
#include <chrono>
#include <iostream>
#include <odb/schema-catalog.hxx>
#include <thread>
#include "../util/utils.h"
using namespace DBAccess;
///////////////////////////////////////////////////////////////////////////////////////////
const std::string DB_PASSWORD = "db_@_pwd";
///////////////////////////////////////////////////////////////////////////////////////////
DBBaseOperate::DBBaseOperate() {}

DBBaseOperate::~DBBaseOperate(){};

void DBBaseOperate::create(const std::string& dbPath, DBPtr& db,
                           int createFlag) {
  try {
    db.reset();
    auto db_ptr = std::make_shared<odb::sqlite::database>(dbPath, createFlag);
    if (!db_ptr) {
      auto msg = "create DB[" + dbPath + "] failed!";
      throw std::exception(msg.data());
    }
    odb::core::transaction t(db_ptr->begin());
    //t.tracer(odb::stderr_full_tracer);
    setPassword(db_ptr);
    setSchema(db_ptr);
    loadExtension(db_ptr);
    installTrigger(db_ptr);
    t.commit();
    setOptimization(db_ptr);
    db = db_ptr;
  } catch (const std::exception& e) {
    auto msg = "create DB[" + dbPath + "] error:{}" + e.what();
    throw std::exception(msg.data());
  }
}
void DBBaseOperate::load(const std::string dbPath, DBPtr& db, int loadFlag) {
  try {
    auto db_ptr = std::make_shared<odb::sqlite::database>(dbPath, loadFlag);
    if (!db_ptr) {
      auto msg = "load DB[" + dbPath + "] failed!";
      throw std::exception(msg.data());
    }
    odb::core::transaction t(db_ptr->begin());
    t.tracer(odb::stderr_full_tracer);
    tryPassword(db_ptr);
    migrate(db_ptr);
    loadExtension(db_ptr);
    t.tracer(odb::stderr_tracer);
    t.commit();
    db = db_ptr;
  } catch (const std::exception& e) {
    auto msg = "load DB[" + dbPath + "] error:{}" + e.what();
    throw std::exception(msg.data());
  }
}
void DBBaseOperate::unload(DBPtr& db) {
  if (db) {
    db.reset();
    db = nullptr;
  }
}
int DBBaseOperate::handler(
    DBPtr const& projectDb,
    std::function<void(DBPtr const& projectDb, odb::transaction& t)> func,
    std::string file, int line, int max_retries) {
  // Error error;
  for (unsigned short retry_count(0);; retry_count++) {
    try {
      odb::transaction t(projectDb->begin());
      { func(projectDb, t); }
      t.commit();
      return 0;
    } catch (const odb::recoverable& e) {
      if (retry_count > max_retries) {
        std::cerr << "ODB handler error:" << e.what() << std::endl;
        return -1;
      } else {
        continue;
      }
    } catch (const odb::exception& e) {
      std::cerr << "ODB handler error:" << e.what() << std::endl;
      return -1;
    } catch (const std::exception& e) {
      std::cerr << "ODB handler error:" << e.what() << std::endl;
      throw;
    }
  }
  return 0;
}

void DBBaseOperate::setPassword(DBPtr const& db) {
  db->execute("pragma key = '" + DB_PASSWORD + "';");
}
void DBBaseOperate::tryPassword(DBPtr const& db) {
  try {
    db->execute("pragma optimize;");
  } catch (const std::exception& e) {
    std::string errMsg = e.what();
    if (errMsg == "26: file is not a database") {
      db->execute("pragma key = '" + DB_PASSWORD + "';");
    }
  }
}
void DBBaseOperate::setSchema(DBPtr const& db) {
#if _ENABLE_EXAMPLE_
  //odb::schema_catalog::create_schema(*db,false);
  // --schema-format embedded
  // drop other schema
  //odb::schema_catalog::drop_schema(*db,ODBORM::ProjectManager::SCHEMA());
#endif
}
void DBBaseOperate::checkTable(DBPtr const& db) {
#if _ENABLE_EXAMPLE_
  try {
    std::string createDatasetAdditionalTableCmd = std::string(R"(
                    CREATE TABLE IF NOT EXISTS DatasetAdditional(id INTEGER AUTO_INCREMENT,datasetId INTEGER NOT NULL,createTime TEXT  NOT NULL,modifiedTime TEXT  NOT NULL,data TEXT  NOT NULL,PRIMARY KEY(id));)");
    auto ret = db->execute(createDatasetAdditionalTableCmd);
  } catch (const std::exception& e) {
    std::cerr << "DatasetsTracker::CreateTable error:" << e.what() << std::endl;
  }
#endif
}
void DBBaseOperate::loadExtension(DBPtr const& db) {
#if _ENABLE_EXAMPLE_
  //download sqlite extension from https://github.com/nalgeon/sqlean
  std::string rootPath =
      pathAppend(std::filesystem::current_path().string(), "turbologic");

  std::string regexpLibPath = pathAppend(rootPath, "regexp.dll");
  if (!fileExisted(regexpLibPath)) {
    regexpLibPath =
        pathAppend(std::filesystem::current_path().string(), "regexp.dll");
  }
  if (fileExisted(regexpLibPath)) {
    db->execute("select load_extension('" + regexpLibPath + "');");
  } else {
    std::cerr << " SQLite extension regexp dll not found!" << std::endl;
  }
  std::string uuidLibPath = pathAppend(rootPath, "uuid.dll");
  if (!fileExisted(uuidLibPath)) {
    uuidLibPath =
        pathAppend(std::filesystem::current_path().string(), "uuid.dll");
  }
  if (fileExisted(uuidLibPath)) {
    db->execute("select load_extension('" + uuidLibPath + "');");
  } else {
    std::cerr << " SQLite extension uuid dll not found!" << std::endl;
  }
#endif
}
void DBBaseOperate::installTrigger(DBPtr const& db) {
#if _ENABLE_EXAMPLE_
  try {
    std::string datasetTagsInsertTrigerCmd = std::string(R"(
        CREATE TRIGGER "DatasetTagsInsertTriger"
        AFTER INSERT
        ON "DatasetTags"
        BEGIN
          REPLACE INTO DatasetsTracker(datasetId, uuid) VALUES (new.datasetId, strftime('%Y%m%d%H%M%f','now','localtime'));
        END;)");
    db->execute(datasetTagsInsertTrigerCmd);

    std::string datasetTagsDeleteTrigerCmd = std::string(R"(
       CREATE TRIGGER "DatasetTagsDeleteTriger"
       AFTER DELETE
       ON "DatasetTags"
       BEGIN
         REPLACE INTO DatasetsTracker(datasetId, uuid) VALUES (old.datasetId, strftime('%Y%m%d%H%M%f','now','localtime'));
       END;)");
    db->execute(datasetTagsDeleteTrigerCmd);

    std::string datasetTagsUpdateTrigerCmd = std::string(R"(
      CREATE TRIGGER "DatasetTagsUpdateTriger"
      AFTER UPDATE
      ON "DatasetTags"
      BEGIN
        UPDATE DatasetsTracker Set uuid=strftime('%Y%m%d%H%M%f','now','localtime')  WHERE datasetId==old.datasetId AND new.name!=old.name;
      END;)");
    db->execute(datasetTagsUpdateTrigerCmd);

  } catch (const std::exception& e) {
    std::cerr << "DatasetsTracker::CreateTriger error:" << e.what();
  }
#endif
}
void DBBaseOperate::setOptimization(DBPtr const& db) {
  db->connection()->execute("PRAGMA journal_mode=WAL");
  db->connection()->execute("PRAGMA cache_size=8000");
  db->connection()->execute("PRAGMA synchronous=off");
  db->connection()->execute("PRAGMA temp_store=memory");
  db->connection()->execute("PRAGMA mmap_size=256000000");
}
void DBBaseOperate::migrate(DBPtr const& db) {
  try {
    odb::schema_version v(db->schema_version());
    odb::schema_version bv(odb::schema_catalog::base_version(*db));
    odb::schema_version cv(odb::schema_catalog::current_version(*db));
    if (v == 0) {
      //No schema in the database. Create the schema and
      //initialize the database.
      odb::core::transaction t(db->begin());
      //odb::schema_catalog::create_schema(*db);
      setSchema(db);
      // Populate the database with initial data, if any.
      t.commit();
    } else if (v < cv) {
      std::cout << "Old schema (and data) in the database, migrate them."
                << std::endl;
      if (v < bv) {
        std::cerr
            << "Error: migration from this version is no longer supported."
            << std::endl;
      } else {
        for (v = odb::schema_catalog::next_version(*db, v); v <= cv;
             v = odb::schema_catalog::next_version(*db, v)) {
          std::cout << "Data migration." << std::endl;
          odb::core::transaction t(db->begin());
          //odb::schema_catalog::migrate_schema_pre(*db, v);
          // Data migration goes here.
          odb::schema_catalog::migrate(*db);
          //odb::schema_catalog::migrate_schema_post(*db, v);
          t.commit();
        }
      }
    } else if (v > cv) {
      std::cerr << "Error: old application trying to access new database."
                << std::endl;
    }
  } catch (const std::exception& e) {
    std::cerr << "Migrate error:" << e.what() << std::endl;
  }
}
///////////////////////////////////////////////////////////////////////////////////////////
DBPtrHub::DBPtrHub() {}
DBPtrHub::~DBPtrHub() {
  std::unique_lock<std::mutex> lck(_mtx);
  _dbs.clear();
}

DBPtr DBPtrHub::getDBPtr(std::string path) {
  std::unique_lock<std::mutex> lck(_mtx);
  auto iter = _dbs.find(path);
  if (iter != _dbs.end()) {
    return iter->second;
  }
  return nullptr;
}
void DBPtrHub::delDBPtr(std::string path) {
  std::unique_lock<std::mutex> lck(_mtx);
  auto iter = _dbs.find(path);
  if (iter != _dbs.end()) {
    _dbs.erase(iter);
  }
}
bool DBPtrHub::addDBPtr(std::string path, DBPtr db) {
  if (!db) {
    return false;
  }
  std::unique_lock<std::mutex> lck(_mtx);
  auto iter = _dbs.find(path);
  if (iter != _dbs.end()) {
    return false;
  }
  _dbs[path] = db;
  return true;
}
