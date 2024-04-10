#include "common.h"

#include <nlohmann/json-schema.hpp>
#include <nlohmann/json.hpp>

#include "logger.h"
#include "logic.h"
#include "logictype.h"
using nlohmann::json;
using nlohmann::json_schema::json_validator;

std::string getLogLevel(int level) {
  if (level == 1) {
    return "debug";
  } else if (level == 2) {
    return "info";
  } else if (level == 3) {
    return "warning";
  } else if (level == 4) {
    return "error";
  } else if (level == 5) {
    return "critical";
  } else {
    return "";
  }
}

bool parseBody(const std::string& body, json& request, json& response,
               const std::vector<std::string>& needEntryList) {
  // ����json body
  try {
    printf("body: %s\n", body.data());
    request = json::parse(body);
    for (std::string entry : needEntryList) {
      printf("entry: %d\n", request.contains(entry));
      if (!request.contains(entry)) {
        response["msg"] = "lack of necessary args: " + entry;
        response["code"] = TurboLogic::ErrorType::ErrInvalidArg;
        return false;
      }
    }
    return true;
  } catch (const nlohmann::detail::parse_error& e) {
    response["msg"] = "parse request body fail";
    response["code"] = TurboLogic::ErrorType::ErrInvalidArg;
    return false;
  }
}

bool dumpResponse(json& response, std::string& dumpData) {
  try {
    dumpData = response.dump();
    return true;
  } catch (const std::exception& e) {
    response["data"] = json::object();
    response["msg"] = "pack response fail:[" + std::string(e.what()) + "]";
    LogError(response["msg"].get<std::string>());
    response["code"] = TurboLogic::ErrorType::ErrUnknown;
    dumpData = response.dump();

    return false;
  };
}

bool checkEnumArg(const int value, const std::string EnumType, json& response) {
  try {
    if (EnumType == "AlgorithmType") {
      if ((TurboLogic::AlgorithmType::SingleLabelClassification <= value) &&
          (value <= TurboLogic::AlgorithmType::OCR)) {
        printf("------ AlgorithmType ------ \n");
        return true;
      }
    } else if (EnumType == "ProcessStatus") {
      if ((TurboLogic::ProcessStatus::Cancelling <= value) &&
          (value <= TurboLogic::ProcessStatus::Finished)) {
        printf("------ ProcessStatus ------ \n");
        return true;
      }
    }
    throw std::exception("The enum value range out of all values!");
  } catch (const std::exception& e) {
    response["msg"] = std::string(e.what());
    response["code"] = TurboLogic::ErrorType::ErrInvalidArg;
    return false;
  }
}

void checkJson(const json& valueCheck, const json& valueExample) {
  if (valueCheck.type_name() == valueExample.type_name()) {
    if (valueCheck.is_array()) {
      if (valueCheck.size()) {
        checkJson(valueCheck[0], valueExample[0]);
      }
    } else if (valueCheck.is_object()) {
      for (auto& checkData : valueCheck.items()) {
        auto checkKey = checkData.key();
        if (valueExample.contains(checkKey)) {
          checkJson(checkData.value(), valueExample[checkKey]);
        }
      }
    }
  } else {
    throw std::exception("The request args has invalid type!");
  }
}

bool checkRequest(json& request, json& valueExample, json& response) {
  try {
    checkJson(request, valueExample);
    return true;
  } catch (const std::exception& e) {
    response["msg"] = std::string(e.what());
    response["code"] = TurboLogic::ErrorType::ErrInvalidArg;
    return false;
  };
}

bool is_digits(const std::string& str) {
  return str.find_first_not_of("0123456789") == std::string::npos;
}
std::string getFileBaseName(std::string path) {
  int l = path.length();
  int i;
  for (i = l - 1; i >= 0; i--) {
    if (path[i] == '/' || path[i] == '\\') {
      break;
    }
  }
  return path.substr(i + 1, l - i - 1);
}

#include <codecvt>
#include <locale>
#include <vector>
std::string GB2312ToUTF8(std::string const& strGb2312) {
  try {
    std::filesystem::u8path(strGb2312);
    return strGb2312;
  } catch (const std::exception& e) {
    // std::cerr << "not utf8 string:" << e.what() << std::endl;
  }
  std::vector<wchar_t> buff(strGb2312.size());
#ifdef _MSC_VER
  std::locale loc("zh-CN");
#else
  std::locale loc("zh_CN.GB18030");
#endif
  wchar_t* pwszNext = nullptr;
  const char* pszNext = nullptr;
  mbstate_t state = {};
  int res = std::use_facet<std::codecvt<wchar_t, char, mbstate_t>>(loc).in(
      state, strGb2312.data(), strGb2312.data() + strGb2312.size(), pszNext,
      buff.data(), buff.data() + buff.size(), pwszNext);

  if (std::codecvt_base::ok == res) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> cutf8;
    return cutf8.to_bytes(std::wstring(buff.data(), pwszNext));
  }
  return "";
}

bool checkVerifyStatus(int authorizationStatus) {
  if (authorizationStatus == 0) {
    return true;
  }
  return false;
}
