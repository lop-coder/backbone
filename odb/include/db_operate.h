#pragma once
#include <odb/schema-catalog.hxx>
#include "db_access_type.h"
namespace DBAccess {

class DBBaseOperate {
 public:
  DBBaseOperate();
  virtual ~DBBaseOperate();
  void create(const std::string& dbPath, DBPtr& db,
              int createFlag = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE |
                               SQLITE_OPEN_WAL);
  void load(const std::string dbPath, DBPtr& db,
            int loadFlag = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE |
                           SQLITE_OPEN_WAL);
  void unload(DBPtr& db);
  static int handler(
      DBPtr const& projectDb,
      std::function<void(DBPtr const& projectDb, odb::transaction& t)> func,
      std::string file, int line, int max_retries = 50);

 protected:
  virtual void setPassword(DBPtr const& db);
  virtual void tryPassword(DBPtr const& db);
  virtual void setSchema(DBPtr const& db);
  virtual void checkTable(DBPtr const& db);
  virtual void loadExtension(DBPtr const& db);
  virtual void installTrigger(DBPtr const& db);
  virtual void setOptimization(DBPtr const& db);
  virtual void migrate(DBPtr const& db);
};
}  // namespace DBAccess

#define DBBaseOperateHandler(projectDb, func, max_retries) \
  DBBaseOperate::handler(projectDb, func, __FILE__, __LINE__, max_retries)
namespace DBAccess {

class DBPtrHub {
 public:
  DBPtrHub();
  ~DBPtrHub();
  DBPtr getDBPtr(std::string key);
  void delDBPtr(std::string key);
  bool addDBPtr(std::string key, DBPtr db);

 private:
  std::string _root_path;
  std::mutex _mtx;
  std::map<std::string, DBPtr> _dbs;
};

}  // namespace DBAccess
