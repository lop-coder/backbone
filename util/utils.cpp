#include "utils.h"
#include <exception>
#include <filesystem>
#include <fstream>
#ifdef _WIN32
#include <direct.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <windows.h>
#endif
#include <wchar.h>
#include <codecvt>
#include <locale>
#include <regex>
#include "md5.h"
bool exec(const std::string& cmd, std::string& result) {
#ifdef WIN32
  FILE* fp = _popen(cmd.c_str(), "r");
#else
  FILE* fp = popen(cmd.c_str(), "r");
#endif

  if (!fp) {
    return false;
  }
  char buf[1024];
  while (fgets(buf, 1024, fp) != NULL) {
    result += buf;
  }

#ifdef WIN32
  _pclose(fp);
#else
  pclose(fp);
#endif
  return true;
}

std::wstring transformString2Wstring(const std::string& s) {
  setlocale(LC_CTYPE, "en_US.UTF-8");
  const char* _Source = s.c_str();
  size_t len = strlen(_Source) + 1;
  size_t converted = 0;
  wchar_t* wStr = new wchar_t[len];
  mbstowcs_s(&converted, wStr, len, _Source, _TRUNCATE);
  std::wstring result(wStr);
  delete[] wStr;
  return result;
}
std::filesystem::path getPath(const std::string& p) {
#if 0
   try {
     return std::filesystem::u8path(p);
   } catch (const std::exception &e) {
     std::cerr << "not utf8 string:" << e.what() << std::endl;
   }
    // usually gbk string come to here
   return std::filesystem::path(p);
#else
  std::wstring wdirname = transformString2Wstring(p);
  // 使用 std::filesystem 和 std::wstring 来创建目录
  std::filesystem::path path(wdirname);
  return path;
#endif
}

std::string getMd5(const std::string& str) {
  std::string md5str;
  MD5 md5;
  md5str = md5(str);
  return md5str;
}
std::string bigFileMd5(const std::string& filePath) {
  std::string md5str;
  try {
    // chunk size read
    std::ifstream inputFile(filePath, std::ios::binary);
    if (!inputFile.is_open()) {
      std::cerr << "Failed to open file:" << filePath << std::endl;
      md5str = "";
    }
    //char buffer[4096];
    char buffer[4096];
    MD5 md5;
    int n = 0;
    while (inputFile.read(buffer, sizeof(buffer))) {
      //md5.add(&buffer, sizeof(buffer));
      md5.add(reinterpret_cast<void*>(buffer), inputFile.gcount());
      n += 1;
    }
    md5str = md5.getHash();
  } catch (const std::exception& e) {
    std::cerr << "get md5 failed!" << e.what() << std::endl;
  }
  return md5str;
}
std::string fileMd5(const std::string& filePath) {
  std::string md5str;
  try {
    std::ifstream ifs(filePath, std::ios::binary);
    std::string fileStr((std::istreambuf_iterator<char>(ifs)),
                        std::istreambuf_iterator<char>());
    ifs.close();
    md5str = getMd5(fileStr);
  } catch (const std::exception& e) {
    std::cerr << "get md5 failed!" << e.what() << std::endl;
  }
  return md5str;
}
bool regexMatch(const std::string& s, const std::wstring& pattern) {
  std::wstring _pattern;
  // first pattern limit
  // std::wstring pattern{
  //  L"^[\u4E00-\u9FA5A-Za-z0-9][\u4E00-\u9FA5A-Za-z0-9_\\-\\.]{0,49}$"};
  // first pattern no limit
  std::wstring patternDefault{L"[\u4E00-\u9FA5A-Za-z0-9_\\-\\.]{0,50}$"};
  if (wcscmp(pattern, L"") == 0) {
    _pattern = patternDefault;
  }
  std::wregex re(_pattern);
  bool ret = std::regex_match(transformString2Wstring(s), re);
  return ret;
}
bool fileExisted(const std::string& file) {
  return std::filesystem::exists(getPath(file));
}

int makeDir(const std::string& dir) {
  std::wstring wdirname = transformString2Wstring(dir);

  // 使用 std::filesystem 和 std::wstring 来创建目录
  std::filesystem::path path(wdirname);
  if (std::filesystem::create_directories(path) || fileExisted(dir)) {
    std::cout << "create directories success: " << path.string() << std::endl;
  } else {
    std::cout << "create directories failed: " << path.string() << std::endl;
  }
  return 0;
}
std::string pathAppend(const std::string& p1, const std::string& p2) {
  auto full = getPath(p1) / getPath(p2);
  return full.u8string();
}

std::string absolutePath(const std::string& path) {
  std::filesystem::path p = getPath(path);
  return std::filesystem::absolute(p).u8string();
}
std::string genUUID() {
#ifdef WIN32
  UUID uuid;
  UuidCreate(&uuid);
  unsigned char* str;
  UuidToStringA(&uuid, &str);
  std::string s((char*)str);
  RpcStringFreeA(&str);
#else
  uuid_t uuid;
  uuid_generate_random(uuid);
  char s[37];
  uuid_unparse(uuid, s);
#endif
  return s;
}
std::string getEnv(const std::string& envName) {
  char* buf = nullptr;
  size_t sz = 0;
  if (_dupenv_s(&buf, &sz, envName.c_str()) == 0 && buf != nullptr) {
    // std::cout << "EnvVarName: " << envName;
    // std::cout << " " << buf << std::endl;
    auto envValue = new std::string(buf, buf + sz);
    free(buf);
    std::string envValueCopy = envValue->c_str();
    delete envValue;
    return envValueCopy;
  }
  return "";
}
bool setEnv(const std::string& envName, const std::string& value) {

  if (_putenv_s(envName.c_str(), value.c_str())) {
    std::cerr << " envName:" << envName << " = " << value << std::endl;
    return false;
  }
  return true;
}
void transformToLowerOrUpper(std::string& s, bool toLower) {
  if (toLower) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
  } else {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::toupper(c); });
  }
}
std::vector<std::string> splitstr(const std::string& str, char tag) {
  std::vector<std::string> li;
  std::string subStr;
  for (size_t i = 0; i < str.length(); i++) {
    if (tag == str[i]) {
      if (!subStr.empty()) {
        li.push_back(subStr);
        subStr.clear();
      }
    } else {
      subStr.push_back(str[i]);
    }
  }
  if (!subStr.empty()) {
    li.push_back(subStr);
  }
  return li;
}

void ltrim(std::string& s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                  [](int ch) { return !std::isspace(ch); }));
}
std::string trim(const std::string& s) {
  std::string::const_iterator it = s.begin();
  while (it != s.end() && isspace(*it)) {
    it++;
  }

  std::string::const_reverse_iterator rit = s.rbegin();
  while (rit.base() != it && isspace(*rit)) {
    rit++;
  }

  return std::string(it, rit.base());
}
void mSleep(int milliseconds) {
  std::chrono::steady_clock::time_point start =
      std::chrono::steady_clock::now();
  double durationMilliseconds = 0;
  while (true) {
    std::chrono::steady_clock::time_point end =
        std::chrono::steady_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    if (milliseconds > duration.count()) {
      std::this_thread::yield();
    } else {
      break;
    }
  }
}
std::string gb2312ToUTF8(std::string strGb2312) {
  try {
    std::filesystem::u8path(strGb2312);
    return strGb2312;
  } catch (const std::exception& e) {
    std::cerr << "not utf8 string:" << e.what() << std::endl;
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
