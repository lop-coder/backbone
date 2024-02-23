#include <stdio.h>

#include <iostream>
#include <memory>

#include <gtest/gtest.h>
#include <thread>
#include <nlohmann/json.hpp>
//非常重要
#ifdef EXTENSION_DLL_EXPORTS
#undef EXTENSION_DLL_EXPORTS
#endif
#include "../../extensions/process/extension-process.h"
#include "backbone.h"
using nlohmann::json;
TEST(ProcessTaskTest, Base) {
  EXPECT_NO_THROW(Backbone::Hub::load("./", "process", "ProcessTask"));
  EXPECT_NE(Backbone::Hub::create<ProcessTask>("ProcessTask", ""), nullptr);
  EXPECT_NO_THROW(Backbone::Hub::create<ProcessTask>("ProcessTask", ""));
  EXPECT_NO_THROW(Backbone::Hub::create<ProcessTask>("ProcessTask", ""));

  auto processTask = Backbone::Hub::create<ProcessTask>("ProcessTask", "");

  int _exitCode = 0;
  std::string _stdoutMsg = "";
  std::string _stderrMsg = "";

  #ifdef _WIN32
  const std::string cmd = "echoWorker.exe";
  #else
  const std::string cmd = "../bin/echoWorker";
  #endif
  auto initializeFunc=[&](const std::string &cmd) {

  };
  auto releaseFunc = [&](int exitCode) {
    _exitCode = exitCode;
  };
  auto stdoutFunc = [&](const std::string &msg) {
    _stdoutMsg += msg;
  };
  auto stderrFunc = [&](const std::string &msg) {
    _stderrMsg += msg;
  };

  processTask->start(cmd, initializeFunc,releaseFunc, stdoutFunc, stderrFunc);
  EXPECT_TRUE(processTask->status());
  _stdoutMsg = "";
  processTask->toStdin("a\n");
  std::this_thread::sleep_for(std::chrono::seconds(1));
  EXPECT_EQ(_stdoutMsg, "a");
  processTask->stop();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  EXPECT_NE(_exitCode, 0);
  EXPECT_FALSE(processTask->status());

  _exitCode = 0;
  _stdoutMsg = "";
  _stderrMsg = "";
  processTask->start(cmd, initializeFunc,releaseFunc, stdoutFunc, stderrFunc);
  EXPECT_TRUE(processTask->status());

  processTask->toStdin("quit\n");
  std::this_thread::sleep_for(std::chrono::seconds(1));
  EXPECT_EQ(_stderrMsg, "quit");
  std::this_thread::sleep_for(std::chrono::seconds(1));
  EXPECT_EQ(_exitCode, 0);
  EXPECT_FALSE(processTask->status());

  processTask.reset();
  EXPECT_NO_THROW(Backbone::Hub::unload("ProcessTask"));
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
