#pragma once
#include "extension-process.h"
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <future>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <queue>
#include <regex>
#include <sstream>
#include <thread>
#include "libtinyprocess/process.hpp"

#include <chrono>
static void mSleep(int milliseconds) {
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

using nlohmann::json;
class ProcessTaskImpl : public ProcessTask,
                        public std::enable_shared_from_this<ProcessTaskImpl> {
 public:
  ProcessTaskImpl();
  virtual ~ProcessTaskImpl();
  virtual void start(
      const std::string& cmd,
      std::function<void(const std::string& cmd)> initializeFunc,
      std::function<void(int exitCode)> finishFunc,
      std::function<void(const std::string& msg)> stdoutFunc,
      std::function<void(const std::string& msg)> stderrFunc) final override;
  virtual void stop() final override;
  virtual bool status() final override;

  virtual void toStdin(const std::string& msg);

 private:
  std::atomic<bool> _isRun = false;
  std::mutex _mtx;
  std::shared_ptr<TinyProcessLib::Process> _processHandler;

  std::function<void(const std::string& cmd)> _initializeFunc;
  std::function<void(int exitCode)> _releaseFunc;
  std::function<void(const std::string& msg)> _stdoutFunc;
  std::function<void(const std::string& msg)> _stderrFunc;
};

ProcessTaskImpl::ProcessTaskImpl() : _isRun(false), _processHandler(nullptr) {}

ProcessTaskImpl::~ProcessTaskImpl() {
  while (_isRun) {
    mSleep(1000);
    std::cerr << "error process never exit!!!!" << std::endl;
  }
};

void ProcessTaskImpl::start(
    const std::string& cmd,
    std::function<void(const std::string& cmd)> initializeFunc,
    std::function<void(int exitCode)> releaseFunc,
    std::function<void(const std::string& msg)> stdoutFunc,
    std::function<void(const std::string& msg)> stderrFunc) {
  std::unique_lock<std::mutex> lck(_mtx);
  if (!_isRun) {
    _initializeFunc = initializeFunc;
    _releaseFunc = releaseFunc;
    _stdoutFunc = stdoutFunc;
    _stderrFunc = stderrFunc;

    bool openStdin = true;
    TinyProcessLib::Process::string_type pCmd = cmd;
    TinyProcessLib::Process::string_type path = "";
    _processHandler = std::make_shared<TinyProcessLib::Process>(
        cmd, path,
        [&](const char* bytes, size_t n) {
          auto msg = std::string(bytes, n);
          if (_stdoutFunc) {
            _stdoutFunc(msg);
          }
        },

        [&](const char* bytes, size_t n) {
          auto msg = std::string(bytes, n);
          if (_stderrFunc) {
            _stderrFunc(msg);
          }
        },
        openStdin);
    if (!_processHandler) {
      return;
    }
    _isRun = true;
    auto MonitorFunc = [this]() {
      int exit_status;
      exit_status = _processHandler->get_exit_status();
      {
        std::unique_lock<std::mutex> lck(_mtx);
        if (_releaseFunc) {
          _releaseFunc(exit_status);
        }
        _isRun = false;
      }
    };
    std::thread Mthread(MonitorFunc);
    Mthread.detach();

    if (_initializeFunc) {
      _initializeFunc(cmd);
    }
  }
}

void ProcessTaskImpl::stop() {
  if (_isRun && _processHandler) {
    _processHandler->kill();
  }
}

bool ProcessTaskImpl::status() {
  return _isRun;
}
void ProcessTaskImpl::toStdin(const std::string& msg) {
  if (_isRun && _processHandler) {
    _processHandler->write(msg);
  }
}
#ifdef __cplusplus
extern "C" {
#endif
std::string identity(void) {
  return PROCESSTASK_IDENTITY;
}
std::string version(void) {
  return PROCESSTASK_VERSION;
}
std::string description(void) {
  return PROCESSTASK_DESCRIPTION;
}
void* create(const std::string& cfg) {
  return new ProcessTaskImpl();
}
void destroy(void* p) {
  if (p) {
    auto tp = reinterpret_cast<ProcessTaskImpl*>(p);
    delete tp;
  }
}
#ifdef __cplusplus
}
#endif

/*

class BACKBONE_PLUGIN_API ProcessMessager {
 public:
  ProcessMessager() = default;
  ~ProcessMessager();
  void start();
  void stop();

  std::string getLine();
  bool status();

 private:
  std::mutex mtx;
  std::queue<std::string> msgs;
  std::atomic<bool> _status = false;
  std::future<bool> threadResult;
};

ProcessMessager::~ProcessMessager() {
  stop();
}
bool ProcessMessager::status() {
  return _status;
}
void ProcessMessager::start() {
  if (!_status) {
    _status = true;
    threadResult = std::async(std::launch::async, [&]() -> bool {
      std::string cmd = "";
      while (_status) {
        while (_status) {
          char c = -1;
          c = std::cin.get();
          if (!_status) {
            return true;
          }
          if (c == 13 || c == 10) {
            break;
          }
          cmd += c;
        }
        if (cmd != "") {
          std::lock_guard<std::mutex> lck(mtx);
          // LogDebug("cmd:{}", cmd);
          //LogTrace("cmd:" + cmd);
          fflush(stdout);
          msgs.push(cmd);
          if (cmd == "Quit") {
            _status = false;
            return true;
          }
        }
        cmd.resize(0);
      }
      return true;
    });
  }
}
void ProcessMessager::stop() {
  if (_status) {
    _status = false;
    threadResult.get();
  }
}
std::string ProcessMessager::getLine() {
  std::string message = "";
  if (!msgs.empty()) {
    std::lock_guard<std::mutex> lck(mtx);
    message = msgs.front();
    msgs.pop();
  }
  return message;
}
*/
