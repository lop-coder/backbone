#pragma once
#include <stdint.h>
#include <functional>

#include <iostream>
#include <memory>
#include <odb/callback.hxx>
#include <odb/core.hxx>  // (1)
#include <stdexcept>
#include <string>
namespace ODBORM {

#pragma db object polymorphic table("DBTarget") callback(dbevent)
class DBTarget {
 public:
  static std::string Schema() { return "DBTarget"; }
  static unsigned long _idCnt;
  DBTarget(std::string const& value) : value_(value) {
    std::cout << "_idCnt:" << _idCnt << std::endl;
    id_ = ++_idCnt;
    std::cout << "id_:" << _idCnt << std::endl;
  };
  DBTarget(unsigned long id, std::string const& value)
      : id_(id), value_(value) {
    if (id_ > _idCnt) {
      _idCnt = id_;
    }
  };
  unsigned long id() { return id_; }
  std::string value() { return value_; }
  void value(std::string const& value) { value_ = value; }
  static std::function<void(odb::callback_event e,
                            std::shared_ptr<ODBORM::DBTarget> target)>
      _eventFunc;
  virtual void dbevent(odb::callback_event e, odb::database& db) {
    if (DBTarget::_eventFunc) {
      try {
        if (e != odb::callback_event::post_erase) {
          DBTarget::_eventFunc(e,
                               std::make_shared<ODBORM::DBTarget>(id_, value_));
        }

      } catch (...) {}
    }
  }
  virtual void dbevent(odb::callback_event e, odb::database& db) const {
    if (DBTarget::_eventFunc) {
      try {
        if (e != odb::callback_event::post_erase) {
          DBTarget::_eventFunc(e,
                               std::make_shared<ODBORM::DBTarget>(id_, value_));
        }
      } catch (...) {}
    }
  }

 protected:
#pragma db id unique
  unsigned long id_;
  friend class odb::access;
  DBTarget() {}
  //#pragma db unique
  std::string value_;
};

}  // namespace ODBORM
