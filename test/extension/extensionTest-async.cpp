#include <stdio.h>

#include <iostream>
#include <memory>

#include <gtest/gtest.h>
#include <thread>
#include <nlohmann/json.hpp>
//非常重要
#ifdef DLL_EXPORTS
#undef DLL_EXPORTS
#endif
#include "../../extensions/async/extension-async.h"
#include "backbone.h"
using nlohmann::json;
TEST(AsyncTaskTest, Base) {
  EXPECT_NO_THROW(Backbone::Hub::load("./", "async", "Async"));
  EXPECT_NE(Backbone::Hub::create<Async>("Async", ""), nullptr);
  EXPECT_NO_THROW(Backbone::Hub::create<Async>("Async", ""));
  EXPECT_NO_THROW(Backbone::Hub::create<Async>("Async", ""));

  auto asyncAsync = Backbone::Hub::create<Async>("Async", "");
  asyncAsync->run(
      [](std::shared_ptr<Async> async, Status& status,
         const std::string& config,
         const std::string& runconfig) -> std::string {
        while (!async->peekCommand()) {
          auto cmd = async->getCommand();
          if (cmd.has_value()) {
            try {
              auto cmdJson = json::parse(cmd.value());
              if ("Q" == cmdJson["command"]) {
                break;
              } else {
                async->sendCommand(cmdJson["command"]);
              }
            } catch (const std::exception& e) {
              async->sendError("error command");
              break;
            }
          }
          std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        status = Status::RUNNING;
        async->setProgress(0.5);
        while (auto msg = async->getMessage()) {
          if (msg.has_value()) {
            try {
              auto msgJson = json::parse(msg.value());
              if ("Q" == msgJson["message"]) {
                break;
              } else {
                async->sendMessage(msgJson["message"]);
              }
            } catch (const std::exception& e) {
              async->sendError("error message");
              break;
            }
          }
          std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        async->setProgress(1);
        return "ok";
      },
      "cfg");

  //check status
  EXPECT_EQ(asyncAsync->getStatus(), Status::STARTING);

  asyncAsync->sendCommand("A");
  std::this_thread::sleep_for(std::chrono::seconds(2));
  auto cmd = asyncAsync->getCommand();
  auto cmdJson = json::parse(cmd.value());
  EXPECT_EQ(cmdJson["command"], "A");

  asyncAsync->sendCommand("B");
  std::this_thread::sleep_for(std::chrono::seconds(2));
  cmd = asyncAsync->getCommand();
  cmdJson = json::parse(cmd.value());
  EXPECT_EQ(cmdJson["command"], "B");

  asyncAsync->sendCommand("Q");
  std::this_thread::sleep_for(std::chrono::seconds(2));
  EXPECT_EQ(asyncAsync->getStatus(), Status::RUNNING);
  EXPECT_EQ(asyncAsync->getProgress(), 0.5);

  std::this_thread::sleep_for(std::chrono::seconds(2));
  while (asyncAsync->getMessagesSize() > 0) {
    asyncAsync->getMessage();
  }
  asyncAsync->sendMessage("A");
  std::this_thread::sleep_for(std::chrono::seconds(2));
  auto msg = asyncAsync->getMessage();
  auto msgJson = json::parse(msg.value());
  EXPECT_EQ(msgJson["message"], "A");

  asyncAsync->sendMessage("Q");
  std::this_thread::sleep_for(std::chrono::seconds(2));
  EXPECT_EQ(asyncAsync->getProgress(), 1);
  std::this_thread::sleep_for(std::chrono::seconds(2));
  EXPECT_EQ(asyncAsync->getStatus(), Status::FINISHED);
  EXPECT_EQ(asyncAsync->waitResult(), "ok");

  asyncAsync.reset();
  EXPECT_NO_THROW(Backbone::Hub::unload("Async"));
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
