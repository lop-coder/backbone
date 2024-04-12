#ifndef _TURBO_LOGGER_H_
#define _TURBO_LOGGER_H_

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <string>

#ifndef SPDLOG_FMT_EXTERNAL
#define SPDLOG_FMT_EXTERNAL
#endif
#include <nlohmann/json.hpp>
#include "spdlog/spdlog.h"

#ifndef _WIN32
#define LOGGER_API
#elif defined LOGGER_DLL_EXPORTS
#define LOGGER_API __declspec(dllexport)
#else
#define LOGGER_API __declspec(dllimport)
#endif

using json = nlohmann::json;
namespace backbone {

#define LOGGER_CONFIG_VERSION "v2.1.0"

class LOGGER_API Logger {
 public:
  static Logger* getInstance();
  json getCfg(const std::string rootPath, const std::string loggerName,
              const std::string configName, bool configSave = false);

  std::shared_ptr<spdlog::logger> create(json& config,
                                         const std::string& loggerName);
  void setDefaultLogger(std::shared_ptr<spdlog::logger> logger);

  Logger* operator->() const;

 private:
  spdlog::level::level_enum from_int(int level);
  std::string level_to_string(int level);
  void cfgRead(json& config, const std::string configFile);
  bool cfgCheck(json& config, const std::string rootPath,
                const std::string loggerName);
  void cfgWrite(json& config, const std::string configFile);

 private:
  Logger(){};
  ~Logger(){};
  Logger(const Logger&);
  Logger& operator=(const Logger&);
};

class LOGGER_API LoggerCoreDump {
 public:
  LoggerCoreDump(){};
  virtual ~LoggerCoreDump(){};
  virtual void Init(std::string dumpfilepath){};
};
std::shared_ptr<LoggerCoreDump> LOGGER_API newLoggerCoreDump();
}  // namespace backbone

#define LOGGER_CALL_(logger, level, ...)                                      \
  logger->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, level, \
              __VA_ARGS__)

#ifdef NOLOGGER

#define LogTrace(...)
#define LogDebug(...)
#define LogInfo(...)
#define LogWarn(...)
#define LogError(...)
#define LogCritical(...)
#define LogCriticalIf(b, ...)
#else
#define LogTrace(...) \
  LOGGER_CALL_(spdlog::default_logger(), spdlog::level::trace, __VA_ARGS__)
#define LogDebug(...) \
  LOGGER_CALL_(spdlog::default_logger(), spdlog::level::debug, __VA_ARGS__)
#define LogInfo(...) \
  LOGGER_CALL_(spdlog::default_logger(), spdlog::level::info, __VA_ARGS__)
#define LogWarn(...) \
  LOGGER_CALL_(spdlog::default_logger(), spdlog::level::warn, __VA_ARGS__)
#define LogError(...) \
  LOGGER_CALL_(spdlog::default_logger(), spdlog::level::err, __VA_ARGS__)
#define LogCritical(...) \
  LOGGER_CALL_(spdlog::default_logger(), spdlog::level::critical, __VA_ARGS__)
#define LogCriticalIf(b, ...)                                         \
  do {                                                                \
    if (b) {                                                          \
      LOGGER_CALL_(spdlog::default_logger(), spdlog::level::critical, \
                   __VA_ARGS__);                                      \
    }                                                                 \
  } while (0)
#endif

#endif
