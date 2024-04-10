#ifndef _ROUTER_COMMON_H_
#define _ROUTER_COMMON_H_
#include <type_traits>

#include "logic.h"
#include "logictype.h"
#include "nlohmann/json.hpp"
using nlohmann::json;
std::string getLogLevel(int level);
bool parseBody(const std::string& body, json& request, json& response,
               const std::vector<std::string>& needEntryList);

bool dumpResponse(json& response, std::string& dumpData);

template <typename E>
constexpr auto to_underlying(E e) noexcept {
  return static_cast<std::underlying_type_t<E>>(e);
}

template <class T>
bool isDigits(const T& str) {
  try {
    if ((std::is_same<std::string, T>::value)) {
      return true;
    }
  } catch (const std::exception& e) {
    return false;
  }
}

template <>
inline bool isDigits(const std::string& str) {
  return str.find_first_not_of("0123456789") == std::string::npos;
}

template <class T>
bool isInteger(const T& n) {
  if ((std::is_same<int, T>::value) || (std::is_same<long, T>::value) ||
      (std::is_same<unsigned long, T>::value) ||
      (std::is_same<unsigned int, T>::value) ||
      (std::is_same<TurboLogic::AlgorithmType, T>::value) ||
      (std::is_same<TurboLogic::ProcessStatus, T>::value)) {
    return true;
  }
  return false;
}

template <>
inline bool isInteger(const json& n) {
  return (n.is_number_integer()) || (n.is_number_unsigned());
}

template <class T>
bool isString(const T& n) {
  if ((std::is_same<std::string, T>::value) || (std::is_same<char, T>::value)) {
    return true;
  }
  return false;
}

template <>
inline bool isString(const json& n) {
  return n.is_string();
}

template <typename T>
bool checkAlgorithmTypeArg(const T& value) {
  if (isInteger(value)) {
    return true;
  }
  return false;
}

template <>
inline bool checkAlgorithmTypeArg(const TurboLogic::AlgorithmType& value) {
  if ((TurboLogic::AlgorithmType::SingleLabelClassification <= value) &&
      (value <= TurboLogic::AlgorithmType::UnsupervisedSegmentation)) {
    printf("------ AlgorithmType ------ \n");
    return true;
  }
  return false;
}

template <typename T>
bool checkProcessStatusArg(const T& value) {
  if (isInteger(value)) {
    return true;
  }
  return false;
}

template <>
inline bool checkProcessStatusArg(const TurboLogic::ProcessStatus& value) {
  if ((TurboLogic::ProcessStatus::Cancelling <= value) &&
      (value <= TurboLogic::ProcessStatus::Finished)) {
    printf("------ ProcessStatus ------ \n");
    return true;
  }
  return false;
}

template <typename T>
bool checkTaskTypeArg(const T& value) {
  if (isInteger(value)) {
    return true;
  }
  return false;
}

template <>
inline bool checkTaskTypeArg(const TurboLogic::TaskType& value) {
  if ((TurboLogic::TaskType::ModelTrain <= value) &&
      (value <= TurboLogic::TaskType::GenerateSdk)) {
    printf("------ TaskType ------ \n");
    return true;
  }
  return false;
}

template <class T>
bool checkArg(const T& value, const std::string& Type, json& response) {
  try {
    if (Type == "AlgorithmType") {
      if (checkAlgorithmTypeArg(value)) {
        return true;
      }
    } else if (Type == "ProcessStatus") {
      if (checkProcessStatusArg(value)) {
        return true;
      }
    } else if (Type == "TaskType") {
      if (checkTaskTypeArg(value)) {
        return true;
      }
    } else if (Type == "DigitalStr") {
      if (isDigits(value)) {
        printf("------ DigitalStr ------ \n");
        return true;
      }
    } else if (Type == "Integer") {
      if (isInteger(value)) {
        printf("------ Integer ------ \n");
        return true;
      }
    } else if (Type == "String") {
      if (isString(value)) {
        printf("------ String ------ \n");
        return true;
      }
    }
    throw std::exception("The request item arg has invalid type!");
  } catch (const std::exception& e) {
    response["msg"] = std::string(e.what());
    response["code"] = TurboLogic::ErrorType::ErrInvalidArg;
    return false;
  }
};

void checkJson(const json& valueCheck, const json& valueExample);

bool checkRequest(json& request, json& valueExample, json& response);

std::string getFileBaseName(std::string path);

std::string GB2312ToUTF8(std::string const& strGb2312);
bool checkVerifyStatus(int authorizationStatus);
#endif  //_ROUTER_COMMON_H_
