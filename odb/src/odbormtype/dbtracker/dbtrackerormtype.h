#pragma once
#include <stdint.h>

#include <iostream>
#include <odb/core.hxx>  // (1)
#include <stdexcept>
#include <string>

namespace ODBORM {

#pragma db object table("DBTracker")
class DBTracker {
 public:
  static std::string Schema() { return "DBTracker"; }
  DBTracker(unsigned long targetId, std::string value)
      : value_(value), targetId_(targetId) {
    std::cout << "targetId_:" << targetId_ << " value_:" << value_ << std::endl;
  };
  unsigned long id() { return id_; }
  unsigned long targetId() { return targetId_; }
  std::string value() { return value_; }
  void value(std::string const& value) { value_ = value; }

 private:
  friend class odb::access;
  DBTracker() {}

#pragma db id auto
  unsigned long id_;

#pragma db unique
  unsigned long targetId_;

  std::string value_;
};
}  // namespace ODBORM
