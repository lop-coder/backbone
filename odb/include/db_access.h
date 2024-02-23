#pragma once
#include "db_access_type.h"

namespace DBAccess {

class BaseAccess {
 public:
  /*
    * @brief: 执行sql命令
    *
    * @param[in]: sql sql命令
    */
  void execute(DBPtr const& db, std::string const& sql) { db->execute(sql); }

  /*
    * @brief: 执行sql命令
    *
    * @param[in]: sql sql命令
    * @param[out]: out 命令执行结果
    */
  template <typename ODB, typename RESULT>
  void querySQL(DBPtr const& db, std::string const& sql, RESULT& out);
};

template <typename ODB, typename RESULT>
void BaseAccess::querySQL(DBPtr const& db, std::string const& sql,
                          RESULT& out) {
  out = db->query<ODB>(sql);
}

}  // namespace DBAccess
