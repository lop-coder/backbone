#pragma once
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include "extension.h"

enum ErrorType {
  Success = 0,
  ErrOutOfMemory = 2000,
  ErrBadTypeid = 2001,
  ErrUnknown = 5000,
};

class Exception : public std::exception {
 public:
  Exception();
  Exception(int code);
  Exception(int code, std::string str);
  uint32_t errorCode();
  ~Exception() throw();
  virtual const char* what() const throw();

  std::string _errorMsg;
  uint32_t _errorCode = 0;

  static std::map<int, std::string> _errorCode2WhatMap;
  static std::string errorCode2What(const uint32_t errorCode,
                                    const std::string errorMessage);
  static bool addErrorCode(const uint32_t errorCode,
                           const std::string errorMessage);
};
class TryCatchWarpper {
 public:
  TryCatchWarpper(std::exception const& exc);
  Exception _except;
  bool addTryCatchWarpperHandler(
      const std::string& catchName,
      std::function<bool(std::exception const&, Exception&)> func);

 private:
  static std::map<std::string,
                  std::function<bool(std::exception const&, Exception&)>>
      _tryCatchWarpperHandler;
};
