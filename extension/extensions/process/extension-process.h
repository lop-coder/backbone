#pragma once
#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <memory>
#include <optional>
#include "extension.h"
//https://github.com/eidheim/tiny-process-library?tab=MIT-1-ov-file
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
  virtual void start(
      const std::string& cmd,
      std::function<void(const std::string& cmd)> initializeFunc,
      std::function<void(int exitCode)> releaseFunc,
      std::function<void(const std::string& msg)> stdoutFunc,
      std::function<void(const std::string& msg)> stderrFunc) = 0;
  virtual void stop() = 0;
  virtual bool status() = 0;
  virtual void toStdin(const std::string& msg) = 0;
};

#define PROCESSTASK_IDENTITY "ProcessTask"
#define PROCESSTASK_VERSION "v1.1.0"
#define PROCESSTASK_DESCRIPTION "ProcessTask"
#endif  //_PROCESS_H_
