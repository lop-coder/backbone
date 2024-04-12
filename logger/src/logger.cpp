#include "logger.h"

#include <spdlog/details/null_mutex.h>
#include <spdlog/details/synchronous_factory.h>
#include <spdlog/sinks/base_sink.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>

#include <sstream>
#include <string>

#include "spdlog/cfg/env.h"
#include "spdlog/fmt/ostr.h"
#include "spdlog/pattern_formatter.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/udp_sink.h"

using namespace std;

namespace backbone {
///////////////////////////////////////////////////////////////////////////////////////////////////////

Logger* Logger::getInstance() {
  static Logger* _instance = nullptr;
  if (_instance == NULL) {
    static std::mutex m_mtx;
    std::lock_guard<std::mutex> lock{m_mtx};
    _instance = new Logger();
  }
  return _instance;
}

Logger* Logger::operator->() const {
  return getInstance();
}

json Logger::getCfg(const std::string rootPath, const std::string loggerName,
                    const std::string configName, bool configSave) {
  std::cout << "configSave:" << configSave << std::endl;
  json config;
  cfgRead(config, rootPath + "/" + configName);
  if (cfgCheck(config, rootPath, loggerName)) {
    if (configSave) {
      cfgWrite(config, rootPath + "/" + configName);
    }
  }
  return config[loggerName];
}

std::shared_ptr<spdlog::logger> Logger::create(json& config,
                                               const std::string& loggerName) {
  std::vector<spdlog::sink_ptr> sink_list;

  if (config["stdout"].value("enable", true)) {
    try {
      auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>(
          spdlog::color_mode::always);
      sink_list.push_back(stdout_sink);
      std::cout << "stdout_sink init success" << std::endl;
    } catch (std::exception& e) {
      std::cerr << "stdout_sink init fail!!" << e.what() << std::endl;
    }
  }

  if (config["udp"].value("enable", false)) {
    try {
      auto udp_sink = std::make_shared<spdlog::sinks::udp_sink_mt>(
          spdlog::sinks::udp_sink_config(
              config["udp"].value("host", "127.0.0.1"),
              config["udp"].value("port", 8888)));
      sink_list.push_back(udp_sink);
      std::cout << "udp_sink_mt init success" << std::endl;
    } catch (std::exception& e) {
      std::cerr << "udp_sink_mt init fail!!" << e.what() << std::endl;
    }
  }

  if (config["file"].value("enable", true)) {
    try {
      // rotate-on-open set to false
      auto rotating_sink =
          std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
              config["file"]["path"], config["file"].value("size", 1048576 * 5),
              config["file"].value("files", 1), false);
      sink_list.push_back(rotating_sink);
      std::cout << "rotating_file_sink_mt init success" << std::endl;
    } catch (std::exception& e) {
      std::cerr << "rotating_file_sink_mt init fail!!" << e.what() << std::endl;
    }
  }
  try {
    auto logger = std::make_shared<spdlog::logger>(
        loggerName, sink_list.begin(), sink_list.end());

    logger->set_level(from_int(config["system"].value("level", 2)));
    // 设置当触发err或者更加严重的错误时立即刷新日志
    logger->set_pattern(config["system"].value(
        "pattern", "%Y-%m-%d %H:%M:%S [%l] [%t %P] - [%s %# %!] %v"));

    logger->flush_on(from_int(config["system"].value("flush_on", 2)));
    // 日期时间 [等级] [线程 进程] - [文件 行 函数名] 内容
    //  spdlog::set_pattern("%Y-%m-%d %H:%M:%S [%l] [%t %P] - [%s %# %!] %v")
    spdlog::flush_every(
        std::chrono::seconds(config["system"].value("flush_every", 2)));
    spdlog::register_logger(logger);

    auto msg = config.dump(2);
    logger->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION},
                spdlog::level::info, msg);
    return spdlog::get(loggerName);
  } catch (std::exception& e) {
    std::cerr << "logger init fail!!" << e.what() << std::endl;
  }
  return nullptr;
}
void Logger::setDefaultLogger(std::shared_ptr<spdlog::logger> logger) {
  spdlog::set_default_logger(logger);
  spdlog::dump_backtrace();
}

spdlog::level::level_enum Logger::from_int(int level) {
  if (level == 0) {
    return spdlog::level::trace;
  } else if (level == 1) {
    return spdlog::level::debug;
  } else if (level == 2) {
    return spdlog::level::info;
  } else if (level == 3) {
    return spdlog::level::warn;
  } else if (level == 4) {
    return spdlog::level::err;
  } else if (level == 5) {
    return spdlog::level::critical;
  } else {
    return spdlog::level::trace;
  }
}
std::string Logger::level_to_string(int level) {
  if (level == 0) {
    return "trace";
  } else if (level == 1) {
    return "debug";
  } else if (level == 2) {
    return "info";
  } else if (level == 3) {
    return "warn";
  } else if (level == 4) {
    return "err";
  } else if (level == 5) {
    return "critical";
  } else {
    return "unkown";
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
#define LOGGER_CONFIG_VERSION "v2.1.0"

void Logger::cfgRead(json& config, const std::string configFile) {
  if (!std::filesystem::exists(configFile)) {
    config = json::object();
    return;
  }
  try {
    ifstream jfile(configFile);
    jfile >> config;
  } catch (const nlohmann::detail::exception& e) {
    std::cout << "logger config parse fail:" << e.what() << " -->" << configFile
              << std::endl;
    config = json::object();
  } catch (const std::exception& e) {
    std::cout << "logger config load fail:" << e.what() << std::endl;
    config = json::object();
  }
}
bool Logger::cfgCheck(json& config, const std::string rootPath,
                      const std::string loggerName) {
  bool changed = false;
  if (!config.contains("version") ||
      !(config["version"].get<std::string>() == LOGGER_CONFIG_VERSION) ||
      !config.contains(loggerName) || !config[loggerName].contains("file") ||
      !config[loggerName]["file"].contains("path") ||
      config[loggerName]["file"]["path"] == "") {
    config["version"] = LOGGER_CONFIG_VERSION;
    config[loggerName] = json::object();
    auto file = json::object();
    file["size"] = 1048576 * 5;
    file["files"] = 1;
    file["enable"] = true;
    file["path"] = rootPath + "/" + loggerName + ".log";
    config[loggerName]["file"] = file;

    auto console = json::object();
    console["enable"] = true;
    config[loggerName]["stdout"] = console;

    auto udp = json::object();
    udp["enable"] = false;
    udp["host"] = "127.0.0.1";
    udp["port"] = 8888;
    config[loggerName]["udp"] = udp;

    auto system = json::object();
    system["level"] = 2;
    system["pattern"] = "%Y-%m-%d %H:%M:%S [%l] [%t %P] - [%s %# %!] %v";
    system["flush_on"] = 2;
    system["flush_every"] = 2;
    system["note"] =
        "level information\n 0:trace \n1:debug \n 2:info\n 3:warn\n 4:err\n "
        "5:critical\n";
    config[loggerName]["system"] = system;
    changed = true;
  }
  return changed;
}
void Logger::cfgWrite(json& config, const std::string configFile) {
  std::ofstream fout(configFile, std::ios::out | std::ios::binary);
  fout << std::setw(4) << config << std::endl;
}
}  // namespace backbone
///////////////////////////////////////////////////////////////////////////////////////////////////////
