#include <algorithm>
#include <chrono>
#include <cmath>
#include <future>
#include <iostream>
#include <memory>
#include <queue>
#include <string>
#include <thread>
#include "db_access.h"
#include "db_operate.h"
#include "src/odbormtype/dbtarget/dbtargetormtype-odb.hxx"
#include "src/odbormtype/dbtracker/dbtrackerormtype-odb.hxx"
using namespace DBAccess;
/////////////////////////////////////////////////////////////////////////////////////////////////////
#define DEFINE_SINGLETON_WITH_CONSTRUCTOR(T) \
 public:                                     \
  static T* _get() {                         \
    static T self;                           \
    return &self;                            \
  }                                          \
  T* operator->() const {                    \
    return _get();                           \
  }                                          \
  T(T& other) = delete;                      \
  void operator=(const T&) = delete;         \
  T() {}

#define DEFINE_SINGLETON(T) \
 public:                    \
  static T* _get() {        \
    static T self;          \
    return &self;           \
  }                         \
  T* operator->() const {   \
    return _get();          \
  }                         \
  T(T& other) = delete;     \
  void operator=(const T&) = delete;
/////////////////////////////////////////////////////////////////////////////////////////////////////
DBAccess::DBPtrHub dbHub;
/////////////////////////////////////////////////////////////////////////////////////////////////////
class DBTargetOperate : public DBBaseOperate {
 protected:
  virtual void setSchema(DBPtr const& db) final override {
    odb::schema_catalog::create_schema(*db, ODBORM::DBTarget::Schema(), false);
  }
  virtual void installTrigger(DBPtr const& db) final override {
#if 0
        try {
          std::string DBTargetInsertTrackerCmd = std::string(R"(
            CREATE TRIGGER "DBTargetInsertTracker"
            AFTER INSERT
            ON "DBTarget"
            BEGIN
              REPLACE INTO DBTracker(id, value) VALUES (NEW.id, NEW.value);
            END;)");
          db->execute(DBTargetInsertTrackerCmd);

          std::string DBTargetDeleteTrackerCmd = std::string(R"(
           CREATE TRIGGER "DBTargetDeleteTracker"
           AFTER DELETE
           ON "DBTarget"
           FOR EACH ROW
           BEGIN
               DELETE FROM DBTracker WHERE targetId = OLD.id;
           END)");
          db->execute(DBTargetDeleteTrackerCmd);

          std::string DBTargetUpdateTrackerCmd = std::string(R"(
          CREATE TRIGGER "DBTargetUpdateTriger"
          AFTER UPDATE
          ON "DBTarget"
          BEGIN
            UPDATE DBTracker Set value=NEW.value WHERE targetId==OLD.id;
          END;)");
          db->execute(DBTargetUpdateTrackerCmd);

        } catch (const std::exception& e) {
          std::cerr << "DBTarget::CreateTriger error:" << e.what()
                    << std::endl;
        }
#endif
  }

 private:
  void enableTriger(std::shared_ptr<odb::database> db, std::string trigerName,
                    bool enable) {
    try {
      std::string enableStr = "ENABLE";
      if (!enable) {
        enableStr = "DISABLED ";
      }
      std::string enableTrigerCmd =
          "ALTER TRIGGER " + trigerName + " " + enableStr;
      db->execute(enableTrigerCmd);
    } catch (const std::exception& e) {
      std::cerr << "DatasetsTracker::EnableTriger:" << e.what() << std::endl;
    }
  }
  DEFINE_SINGLETON_WITH_CONSTRUCTOR(DBTargetOperate);
};
class DBTrackerOperate : public DBBaseOperate {
 protected:
  virtual void setSchema(DBPtr const& db) final override {
    odb::schema_catalog::create_schema(*db, ODBORM::DBTracker::Schema(), false);
  }
  virtual void installTrigger(DBPtr const& db) final override {}
  DEFINE_SINGLETON_WITH_CONSTRUCTOR(DBTrackerOperate);
};
/////////////////////////////////////////////////////////////////////////////////////////////////////
class DBTargetAccess : public BaseAccess {
 public:
  unsigned long insert(const DBPtr& db, const std::string& value) {
    ODBORM::DBTarget target(value);
    return db->persist(target);
  }
  unsigned long insert(const DBPtr& db, const ODBORM::DBTarget& target) {
    //ODBORM::DBTarget target(value);
    return db->persist(target);
  }
  void update(const DBPtr& db, const unsigned long id,
              const std::string& value) {
    auto target = db->load<ODBORM::DBTarget>(id);
    target->value(value);
    db->update(*target);
  }

  void query(const DBPtr& db, const unsigned long id,
             std::shared_ptr<ODBORM::DBTarget>& result) {
    auto rs =
        db->query<ODBORM::DBTarget>(odb::query<ODBORM::DBTarget>::id == id);
    for (auto iter = rs.begin(); iter != rs.end(); iter++) {
      std::cout << "=1111=>>value:" << iter->value() << std::endl;
      result.reset(&(*iter));
    }
  }

  void query(const DBPtr& db, const std::vector<unsigned long>& ids,
             std::vector<ODBORM::DBTarget>& result) {
    result.clear();

    auto rs = db->query<ODBORM::DBTarget>(
        odb::query<ODBORM::DBTarget>::id.in_range(ids.begin(), ids.end()));
    for (auto iter = rs.begin(); iter != rs.end(); iter++) {
      result.push_back(*iter);
    }
  }
  void query(const DBPtr& db, const std::string conditions,
             std::vector<ODBORM::DBTarget>& result) {
    result.clear();

    odb::result<ODBORM::DBTarget> rs;
    if (conditions == "") {
      rs = db->query<ODBORM::DBTarget>();
    } else {
      rs = db->query<ODBORM::DBTarget>(conditions);
    }
    for (auto iter = rs.begin(); iter != rs.end(); iter++) {
      result.push_back(*iter);
    }
  }

  void erase(const DBPtr& db, const unsigned long id) {
    auto rs =
        db->query<ODBORM::DBTarget>(odb::query<ODBORM::DBTarget>::id == id);
    for (auto iter = rs.begin(); iter != rs.end(); iter++) {
      db->erase((*iter));
    }
  }
  void erase(const DBPtr& db, const std::vector<unsigned long>& ids) {
    auto rs = db->query<ODBORM::DBTarget>(
        odb::query<ODBORM::DBTarget>::id.in_range(ids.begin(), ids.end()));
    for (auto iter = rs.begin(); iter != rs.end(); iter++) {
      db->erase((*iter));
    }
  }

  DEFINE_SINGLETON_WITH_CONSTRUCTOR(DBTargetAccess);
};
class DBTrackerAccess : public BaseAccess {
 public:
  void insert(const DBPtr& db, unsigned long targetId,
              std::string const& value) {
    ODBORM::DBTracker tracker(targetId, value);
    db->persist(tracker);
  }

  void update(const DBPtr& db, unsigned long targetId,
              std::string const& value) {
    auto rs = db->query<ODBORM::DBTracker>(
        odb::query<ODBORM::DBTracker>::targetId == targetId);
    if (!rs.empty()) {
      auto tracker = rs.begin();
      tracker->value(value);
      db->update(*tracker);
    }
  }

  void erase(const DBPtr& db, unsigned long targetId) {
    auto rs = db->query<ODBORM::DBTracker>(
        odb::query<ODBORM::DBTracker>::targetId == targetId);
    for (auto iter = rs.begin(); iter != rs.end(); iter++) {
      db->erase((*iter));
    }
  }
  void query(const DBPtr& db, const std::string conditions,
             std::vector<ODBORM::DBTarget>& result) {
    result.clear();

    odb::result<ODBORM::DBTracker> rs;
    if (conditions == "") {
      rs = db->query<ODBORM::DBTracker>();
    } else {
      rs = db->query<ODBORM::DBTracker>(conditions);
    }
    for (auto iter = rs.begin(); iter != rs.end(); iter++) {
      result.emplace_back(ODBORM::DBTarget{iter->targetId(), iter->value()});
    }
  }
  DEFINE_SINGLETON_WITH_CONSTRUCTOR(DBTrackerAccess);
};
/////////////////////////////////////////////////////////////////////////////////////////////////////
static void Tracker(odb::callback_event e,
                    std::shared_ptr<ODBORM::DBTarget> target) {
  unsigned long id = target->id();
  std::string value = target->value();
  if (e == odb::callback_event::post_persist) {
    std::cout << " post_persist " << target->id()
              << " value:" << target->value() << std::endl;
    std::thread([id, value]() {
      try {
        odb::transaction t1(dbHub.getDBPtr("tracker")->begin());
        DBTrackerAccess()->insert(dbHub.getDBPtr("tracker"), id, value);
        t1.commit();
      } catch (const std::exception& e) {
        std::cerr << "trackerDB persist:" << e.what() << std::endl;
      }
    }).detach();
  } else if (e == odb::callback_event::post_update) {
    std::cout << " post_update " << target->id() << " value:" << target->value()
              << std::endl;
    std::thread([id, value]() {
      try {
        odb::transaction t1(dbHub.getDBPtr("tracker")->begin());
        DBTrackerAccess()->update(dbHub.getDBPtr("tracker"), id, value);
        t1.commit();
      } catch (const std::exception& e) {
        std::cerr << "trackerDB update:" << e.what() << std::endl;
      }
    }).detach();
  } else if (e == odb::callback_event::pre_erase) {
    std::cout << " pre_erase " << target->id() << " value:" << target->value()
              << std::endl;
    std::thread([id]() {
      try {
        odb::transaction t1(dbHub.getDBPtr("tracker")->begin());
        DBTrackerAccess()->erase(dbHub.getDBPtr("tracker"), id);
        t1.commit();
      } catch (const std::exception& e) {
        std::cerr << "trackerDB erase:" << e.what() << std::endl;
      }
    }).detach();
  }
};

static void Initialization() {
  DBPtr targetDB;
  DBPtr trackerDB;
  try {
    //DBTargetOperate()->create(":memory:", targetDB);
    DBTargetOperate()->create("target.db", targetDB);
  } catch (...) {
    //DBTargetOperate().load(":memory:", targetDB);
    DBTargetOperate().load("target.db", targetDB);
  }

  try {
    DBTrackerOperate()->create("tracker.db", trackerDB);
  } catch (...) {
    DBTrackerOperate().load("tracker.db", trackerDB);
  }
  dbHub.addDBPtr("target", targetDB);
  dbHub.addDBPtr("tracker", trackerDB);
}
static void TargetSyncFromTracker() {
  std::vector<unsigned long> ids;
  std::vector<ODBORM::DBTarget> results;

  auto trackerDB = dbHub.getDBPtr("tracker");
  auto targetDB = dbHub.getDBPtr("target");

  std::cout << "trackerDB query " << std::endl;
  DBBaseOperateHandler(
      trackerDB,
      [&](DBPtr const& db, odb::transaction& t) {
        DBTrackerAccess()->query(db, "", results);
        for (auto item : results) {
          std::cout << "-->id: " << item.id() << " value:" << item.value()
                    << std::endl;
        }
      },
      20);
  std::cout << "targetDB sync " << std::endl;
  ODBORM::DBTarget::_eventFunc = nullptr;
  DBBaseOperateHandler(
      targetDB,
      [&](DBPtr const& db, odb::transaction& t) {
        for (auto item : results) {
          std::cout << "-->id: " << item.id() << " value:" << item.value()
                    << std::endl;
          ids.push_back(DBTargetAccess()->insert(db, item));
        }
      },
      20);
  ODBORM::DBTarget::_eventFunc = Tracker;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv) {
  try {
    Initialization();
    TargetSyncFromTracker();

    auto trackerDB = dbHub.getDBPtr("tracker");
    auto targetDB = dbHub.getDBPtr("target");

    std::vector<unsigned long> ids;
    std::vector<ODBORM::DBTarget> results;
    unsigned long maxId = 1;
    std::cout << "targetDB query " << std::endl;
    DBBaseOperateHandler(
        targetDB,
        [&](DBPtr const& db, odb::transaction& t) {
          DBTargetAccess()->query(db, "", results);
          for (auto item : results) {
            std::cout << "-->id: " << item.id() << " value:" << item.value()
                      << std::endl;
            if (item.id() > maxId) {
              maxId = item.id();
            }
          }
        },
        20);
    maxId++;

    std::cout << "targetDB operate " << std::endl;
    DBBaseOperateHandler(
        targetDB,
        [&](DBPtr const& db, odb::transaction& t) {
          ids.push_back(DBTargetAccess()->insert(db, std::to_string(maxId)));
          maxId++;
        },
        20);
    DBBaseOperateHandler(
        targetDB,
        [&](DBPtr const& db, odb::transaction& t) {
          ids.push_back(DBTargetAccess()->insert(db, std::to_string(maxId)));
          maxId++;
        },
        20);
    DBBaseOperateHandler(
        targetDB,
        [&](DBPtr const& db, odb::transaction& t) {
          ids.push_back(DBTargetAccess()->insert(db, std::to_string(maxId)));
          maxId++;
        },
        20);
    DBBaseOperateHandler(
        targetDB,
        [&](DBPtr const& db, odb::transaction& t) {
          //  DBTargetAccess()->erase(db,ids[maxId%2]);
        },
        20);
    std::cout << "targetDB query " << std::endl;
    results.clear();
    DBBaseOperateHandler(
        targetDB,
        [&](DBPtr const& db, odb::transaction& t) {
          DBTargetAccess()->query(db, ids, results);
          for (auto item : results) {
            std::cout << "-->id: " << item.id() << " value:" << item.value()
                      << std::endl;
          }
        },
        20);
    std::this_thread::sleep_for(std::chrono::seconds(3));
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return false;
  };
  return 0;
}
