#pragma once
#include <mutex>
#include <odb/database.hxx>
#include <odb/sqlite/database.hxx>
#include <odb/transaction.hxx>
#include <string>
namespace DBAccess {
using DBPtr = std::shared_ptr<odb::sqlite::database>;

}  // namespace DBAccess
