
#include "extension-exception.h"
#include <exception>

std::map<int, std::string> Exception::_errorCode2WhatMap = {
    {Success, "success"},
    {ErrOutOfMemory, "out of memory"},
    {ErrUnknown, "ErrUnknown"},
    {ErrBadTypeid, "bad_typeid"},
};

std::string Exception::errorCode2What(const uint32_t errorCode,
                                      const std::string errorMessage) {
  std::string what = "";
  auto item = _errorCode2WhatMap.find(errorCode);
  if (item != _errorCode2WhatMap.end()) {
    what += _errorCode2WhatMap[errorCode];
  } else {
    what += "Unknown error type!";
  }
  if (errorMessage != "") {
    what += " detail: " + errorMessage;
  }
  return what;
}
bool Exception::addErrorCode(const uint32_t errorCode,
                             const std::string errorMessage) {
  auto item = _errorCode2WhatMap.find(errorCode);
  if (item != _errorCode2WhatMap.end()) {
    std::cerr << " add error code failed: The error code already exists"
              << std::endl;
    return false;
  } else {
    _errorCode2WhatMap[errorCode] = errorMessage;
    return true;
  }
}

Exception::Exception() : _errorMsg("") {}
Exception::Exception(int code) : _errorCode(code) {
  _errorMsg = errorCode2What(_errorCode, "");
}
Exception::Exception(int code, std::string str) : _errorCode(code) {
  _errorMsg = errorCode2What(_errorCode, str);
}
uint32_t Exception::errorCode() {
  return _errorCode;
}
Exception::~Exception() throw() {}
const char* Exception::what() const throw() {
  return _errorMsg.c_str();
}

TryCatchWarpper::TryCatchWarpper(std::exception const& exc) {
  try {
    std::string type_name = typeid(exc).name();
    if (type_name == typeid(std::bad_alloc).name()) {
      _except._errorCode = ErrOutOfMemory;
      _except._errorMsg =
          Exception::errorCode2What(_except._errorCode, exc.what());

    } else if (type_name == typeid(std::bad_typeid).name()) {
      _except._errorCode = ErrBadTypeid;
      _except._errorMsg =
          Exception::errorCode2What(_except._errorCode, exc.what());

    } else {
      auto iter = TryCatchWarpper::_tryCatchWarpperHandler.find(type_name);
      if (iter != TryCatchWarpper::_tryCatchWarpperHandler.end()) {
        if (iter->second) {
          iter->second(exc, _except);
        }
      } else {
        _except._errorCode = ErrUnknown;
        _except._errorMsg =
            Exception::errorCode2What(_except._errorCode, exc.what());
      }
    }
  } catch (std::bad_typeid const& e) {
    _except._errorCode = ErrUnknown;
    _except._errorMsg =
        Exception::errorCode2What(_except._errorCode, exc.what());
  }
}

bool TryCatchWarpper::addTryCatchWarpperHandler(
    const std::string& catchName,
    std::function<bool(std::exception const&, Exception&)> func) {
  auto item = _tryCatchWarpperHandler.find(catchName);
  if (item != _tryCatchWarpperHandler.end()) {
    std::cerr << " add catch name failed: The catch name already exists"
              << std::endl;
    return false;
  } else {
    _tryCatchWarpperHandler[catchName] = func;
    return true;
  }
}

std::map<std::string, std::function<bool(std::exception const&, Exception&)>>
    TryCatchWarpper::_tryCatchWarpperHandler = {};

#if 0
bool TryCatchNlohmannJsonWarpper(std::exception const&exc, Exception&exception){
    try {
      std::string type_name = typeid(exc).name();

      if (type_name == typeid(nlohmann::detail::parse_error).name()) {
        innerType = ErrInvalidArg;
      } else if (type_name == typeid(nlohmann::detail::type_error).name()) {
        innerType = ErrInvalidArg;
      } else {
        innerType = type;
      }
    } catch (...) {
        throw;
    }
}

int exceptionWarpper(std::exception const& exc){
    try {
      std::string what = exc.what();
      std::string type_name = typeid(exc).name();

      const odb::exception* e = dynamic_cast<const odb::exception*>(&exc);
      if (e != nullptr) {  // is odb exception
        if (type_name == typeid(odb::timeout).name()) {
          //innerType = ErrDBTimeout;
          LogWarn("odb time out, file: {}, line: {}", file, line);
          innerType = ErrDBTimeout;
        } else if (type_name == typeid(odb::deadlock).name()) {
          LogWarn("odb deadlock, file: {}, line: {}", file, line);
          innerType = ErrDBTransactionAborted;
        } else if (type_name == typeid(odb::connection_lost).name()) {
          LogWarn("odb connection lost, file: {}, line: {}", file, line);
          innerType = ErrDBConnectionLoss;
        } else if (type_name == typeid(odb::sqlite::database_exception).name() &&
                   isDuplicateError(what)) {  // 捕获重名
          if (what.find("DatasetTags.name") != std::string::npos) {
            LogWarn("dataset tag name duplicate, file: {}, line: {}", file, line);
            innerType = ErrTagNameDuplicate;
          } else if (what.find("Datasets.name") != std::string::npos) {
            LogWarn("dataset name duplicate, file: {}, line: {}", file, line);
            innerType = ErrDatasetNameDuplicate;
          } else if (what.find("Models.name") != std::string::npos) {
            LogWarn("model name duplicate, file: {}, line: {}", file, line);
            innerType = ErrModelNameDuplicate;
          } else if (what.find("Images.name") != std::string::npos) {
            LogWarn("image name duplicate, file: {}, line: {}", file, line);
            innerType = ErrDatasetImageDuplicate;
          } else if (what.find("DatasetAttrs.name") != std::string::npos) {
            LogWarn("dataset attr name duplicate, file: {}, line: {}", file,
                    line);
            innerType = ErrDatasetAttrNameDuplicate;
          } else if (what.find("ProjectManager.name") != std::string::npos) {
            LogWarn("project name duplicate, file: {}, line: {}", file, line);
            innerType = ErrProjNameDuplicate;
          } else {
            innerType = ErrNameDuplicate;
          }
        } else {
          innerType = ErrDBHandle;
        }
      } else {  // not odb exception
        innerType = type;
      }
    } catch (std::bad_typeid const& e) {
      return TurboLogicException(ErrUnknown,
                                 getErrorMessage(ErrUnknown, e.what()));
    } catch (std::bad_cast const& e) {
      return TurboLogicException(ErrUnknown,
                                 getErrorMessage(ErrUnknown, e.what()));
    }
}


TurboLogicException wrapper_json_exception(std::exception const& exc,
                                           const ErrorType type,
                                           std::string const& file,
                                           const int line) {
  ErrorType innerType = ErrUnknown;

  try {
    std::string type_name = typeid(exc).name();

    if (type_name == typeid(nlohmann::detail::parse_error).name()) {
      innerType = ErrInvalidArg;
    } else if (type_name == typeid(nlohmann::detail::type_error).name()) {
      innerType = ErrInvalidArg;
    } else {
      innerType = type;
    }
  } catch (std::bad_typeid const& e) {
    return TurboLogicException(ErrUnknown,
                               getErrorMessage(ErrUnknown, e.what()));
  } catch (std::bad_cast const& e) {
    return TurboLogicException(ErrUnknown,
                               getErrorMessage(ErrUnknown, e.what()));
  }

  return TurboLogicException(innerType, getErrorMessage(innerType, exc.what()));
}

TurboLogicException wrapper_std_exception(std::exception const& exc,
                                          const ErrorType type,
                                          std::string const& file,
                                          const int line) {
  ErrorType innerType = ErrUnknown;

  try {
    std::string type_name = typeid(exc).name();

    if (type_name == typeid(std::bad_alloc).name())  // 优先捕获内存等问题
    {
      innerType = ErrOutOfMemory;
      LogWarn("out of memory, file: {}, line: {}", file, line);
    } else if (type_name == typeid(std::bad_typeid).name()) {
      innerType = ErrUnknown;
    } else {
      innerType = type;
    }
  } catch (std::bad_typeid const& e) {
    return TurboLogicException(ErrUnknown,
                               getErrorMessage(ErrUnknown, e.what()));
  }

  return TurboLogicException(innerType, getErrorMessage(innerType, exc.what()));
}

TurboLogicException wrapper_odb_exception(std::exception const& exc,
                                          const ErrorType type,
                                          std::string const& file,
                                          const int line) {
  ErrorType innerType = ErrUnknown;

  try {
    std::string what = exc.what();
    std::string type_name = typeid(exc).name();

    const odb::exception* e = dynamic_cast<const odb::exception*>(&exc);
    if (e != nullptr) {  // is odb exception
      if (type_name == typeid(odb::timeout).name()) {
        //innerType = ErrDBTimeout;
        LogWarn("odb time out, file: {}, line: {}", file, line);
        innerType = ErrDBTimeout;
      } else if (type_name == typeid(odb::deadlock).name()) {
        LogWarn("odb deadlock, file: {}, line: {}", file, line);
        innerType = ErrDBTransactionAborted;
      } else if (type_name == typeid(odb::connection_lost).name()) {
        LogWarn("odb connection lost, file: {}, line: {}", file, line);
        innerType = ErrDBConnectionLoss;
      } else if (type_name == typeid(odb::sqlite::database_exception).name() &&
                 isDuplicateError(what)) {  // 捕获重名
        if (what.find("DatasetTags.name") != std::string::npos) {
          LogWarn("dataset tag name duplicate, file: {}, line: {}", file, line);
          innerType = ErrTagNameDuplicate;
        } else if (what.find("Datasets.name") != std::string::npos) {
          LogWarn("dataset name duplicate, file: {}, line: {}", file, line);
          innerType = ErrDatasetNameDuplicate;
        } else if (what.find("Models.name") != std::string::npos) {
          LogWarn("model name duplicate, file: {}, line: {}", file, line);
          innerType = ErrModelNameDuplicate;
        } else if (what.find("Images.name") != std::string::npos) {
          LogWarn("image name duplicate, file: {}, line: {}", file, line);
          innerType = ErrDatasetImageDuplicate;
        } else if (what.find("DatasetAttrs.name") != std::string::npos) {
          LogWarn("dataset attr name duplicate, file: {}, line: {}", file,
                  line);
          innerType = ErrDatasetAttrNameDuplicate;
        } else if (what.find("ProjectManager.name") != std::string::npos) {
          LogWarn("project name duplicate, file: {}, line: {}", file, line);
          innerType = ErrProjNameDuplicate;
        } else {
          innerType = ErrNameDuplicate;
        }
      } else {
        innerType = ErrDBHandle;
      }
    } else {  // not odb exception
      innerType = type;
    }
  } catch (std::bad_typeid const& e) {
    return TurboLogicException(ErrUnknown,
                               getErrorMessage(ErrUnknown, e.what()));
  } catch (std::bad_cast const& e) {
    return TurboLogicException(ErrUnknown,
                               getErrorMessage(ErrUnknown, e.what()));
  }

  return TurboLogicException(innerType, getErrorMessage(innerType, exc.what()));
}

TurboLogicException wrapper_exception(std::exception const& exc,
                                      const ErrorType type,
                                      std::string const& file, const int line) {
  try {
    std::string type_name = typeid(exc).name();

    if (type_name.find("odb") != std::string::npos) {
      return wrapper_odb_exception(exc, type, file, line);
    } else if (type_name.find("json") != std::string::npos) {
      return wrapper_json_exception(exc, type, file, line);
    } else {
      return wrapper_std_exception(exc, type, file, line);
    }
  } catch (std::bad_typeid const& e) {
    return TurboLogicException(ErrUnknown,
                               getErrorMessage(ErrUnknown, exc.what()));
  }

  return TurboLogicException(type, exc.what());
}
#endif
