
#include "db_operate.h"
#include <chrono>
#include <iostream>
#include <odb/schema-catalog.hxx>
#include <thread>
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
    //t.tracer(odb::stderr_full_tracer);
    tryPassword(db_ptr);
    loadExtension(db_ptr);
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
void DBBaseOperate::setSchema(DBPtr const& db) {}
void DBBaseOperate::loadExtension(DBPtr const& db) {}
void DBBaseOperate::installTrigger(DBPtr const& db) {}
void DBBaseOperate::setOptimization(DBPtr const& db) {
  db->connection()->execute("PRAGMA journal_mode=WAL");
  db->connection()->execute("PRAGMA cache_size=8000");
  db->connection()->execute("PRAGMA synchronous=off");
  db->connection()->execute("PRAGMA temp_store=memory");
  db->connection()->execute("PRAGMA mmap_size=256000000");
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
