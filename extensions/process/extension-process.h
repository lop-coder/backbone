#pragma once
#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <memory>
#include <optional>
#include "extension.h"

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
*/
#include <iostream>
inline std::string getMsg() {
  std::string cmd = "";
  while (true) {
    char c = -1;
    c = std::cin.get();
    if (c == 13 || c == 10) {
      break;
    }
    cmd += c;
  }
  return cmd;
}

class BACKBONE_PLUGIN_API ProcessTask {
 public:
  ProcessTask(){};
  virtual ~ProcessTask() = default;
  virtual void start(const std::string& cmd,
                     std::function<void(int exitCode)> finishFunc,
                     std::function<void(std::string msg)> stdoutFunc,
                     std::function<void(std::string msg)> stderrFunc) = 0;
  virtual void stop() = 0;
  virtual bool status() = 0;
  virtual void toStdin(const std::string& msg) = 0;
};

#define PROCESSTASK_IDENTITY "ProcessTask"
#define PROCESSTASK_VERSION "v1.0.0"
#define PROCESSTASK_DESCRIPTION "ProcessTask"
#endif  //_PROCESS_H_
