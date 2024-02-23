#include <algorithm>
#include <chrono>
#include <cmath>
#include <future>
#include <iostream>
#include <memory>
#include <queue>
#include <string>
#include <thread>
#include "xgrpc.h"

using namespace BackboneGRPC;
int client(const std::string& server_address) {

  auto guide = newClient();
  guide->Initialize(server_address);
  std::string ret;
  bool run = true;
  int count = 0;
  std::string reqId;
  std::string reqType;
  std::string reqData;
  std::string result;
#if 0
  for (int i = 0; i < 5; i++) {
    reqId = std::to_string(i);
    reqType = "call_func1";
    reqData = "test" + std::to_string(i);
    result = "";
    guide->Call(reqId, reqType, reqData, result);
    std::cout << "Call func1 "
              << " data:" << result << std::endl;
  }

  for (int i = 0; i < 5; i++) {
    reqId = std::to_string(i);
    reqType = "call_func2";
    reqData = "test" + std::to_string(i);
    result = "";

    guide->Call(reqId, reqType, reqData, result);
    std::cout << "Call func2 data:" << result << std::endl;
  }

  reqId = std::to_string(200);
  reqType = "call_func3";
  reqData = "test" + std::to_string(200);
  result = "";
  guide->Call(reqId, reqType, reqData, result);
  std::cout << "Call func3 data:" << result << std::endl;

  for (int i = 0; i < 5; i++) {
    reqId = std::to_string(i);
    reqType = "pull_func1";
    reqData = "test" + std::to_string(i);
    result = "";

    guide->Pull(reqId, reqType, reqData,
                [](const std::string& resId, const std::string& resType,
                   const std::string& resData) -> bool {
                  std::cout << "pull_func1 id:" << resId << " data:" << resData
                            << std::endl;
                  return true;
                });
    reqType = "pull_func2";
    guide->Pull(reqId, reqType, reqData,
                [](const std::string& resId, const std::string& resType,
                   const std::string& resData) -> bool {
                  std::cout << "pull_func2 id:" << resId << " data:" << resData
                            << std::endl;
                  return true;
                });
  }
  reqId = std::to_string(400);
  reqType = "pull_func2";
  reqData = "test" + std::to_string(400);
  result = "";

  guide->Pull(reqId, reqType, reqData,
              [](const std::string& resId, const std::string& resType,
                 const std::string& resData) -> bool {
                std::cout << "pull_func2 id:" << resId << " data:" << resData
                          << std::endl;
                return true;
              });


  std::cout << "----------------------------push_func1-------------------------"
               "------------"
            << std::endl;

  guide->Push(
      [&](std::string& reqId, std::string& reqType,
          std::string& reqData) -> bool {
        if (count > 10) {
          return false;
        }
        reqId = std::to_string(count);
        reqType = "push_func1";
        reqData = "test" + std::to_string(count);
        std::cout << "push_func1 id:" << reqId << " data:" << reqData
                  << std::endl;
        count++;
        return true;
      },
      ret);
  std::cout << "----------------------------push_func2-------------------------"
               "------------"
            << std::endl;
  count = 0;
  guide->Push(
      [&](std::string& reqId, std::string& reqType,
          std::string& reqData) -> bool {
        reqId = std::to_string(count);
        reqType = "push_func2";
        reqData = "test" + std::to_string(count);
        std::cout << "push_func2 id:" << reqId << " data:" << reqData
                  << std::endl;
        count++;
        return true;
      },
      ret);
  std::cout << "----------------------------bidi_func1-------------------------"
               "------------"
            << std::endl;
  guide->Bidi(
      [&](std::string& reqId, std::string& reqType,
          std::string& reqData) -> bool {
        reqId = std::to_string(count);
        reqType = "bidi_func1";
        reqData = "test" + std::to_string(count);
        std::cout << "bidi_func1 id:" << reqId << " data:" << reqData
                  << std::endl;

        count++;
        if (count > 100) {
          return false;
        }
        return true;
      },
      [](const std::string& resId, const std::string& resType,
         const std::string& resData) -> bool {
        std::cout << "bidi_func1 r id:" << resId << " data:" << resData
                  << std::endl;
        return true;
      });
#endif
  std::cout << "----------------------------HeartBeat--------------------------"
               "-----------"
            << std::endl;
  std::thread([&]() {
    int count = 0;
    while (run) {
      reqId = std::to_string(3000);
      reqType = "System";
      reqData = "HeartBeat\n";
      result = "";
      guide->Call(reqId, reqType, reqData, result);
      std::this_thread::sleep_for(std::chrono::seconds(3));
    }
  }).detach();
  std::cout << "----------------------------Subscribe--------------------------"
               "-----------"
            << std::endl;
  std::thread([&]() {
    reqId = std::to_string(200);
    reqType = "topic_a";
    reqData = "test" + std::to_string(200);
    result = "";
    guide->Subscribe(reqId, reqType, reqData,
                     [](const std::string& resId, const std::string& resType,
                        const std::string& resData) -> bool {
                       std::cout << "Subscribe topic_a id:" << resId
                                 << " data:" << resData << std::endl;
                       return true;
                     });
  }).detach();
  std::thread([&]() {
    reqId = std::to_string(200);
    reqType = "topic_b";
    reqData = "test" + std::to_string(200);
    result = "";
    guide->Subscribe(reqId, reqType, reqData,
                     [](const std::string& resId, const std::string& resType,
                        const std::string& resData) -> bool {
                       std::cout << "Subscribe topic_a id:" << resId
                                 << " data:" << resData << std::endl;
                       return true;
                     });
  }).detach();
  std::cout << "----------------------------Publish--1-------------------------"
               "----------"
            << std::endl;
  std::thread([&]() {
    int count = 0;
    guide->Publish(
        [&](std::string& reqId, std::string& reqType,
            std::string& reqData) -> bool {
          reqId = std::to_string(count);
          reqType = "topic_a";
          reqData = "test" + std::to_string(count);
          std::cout << "Publish 3 id:" << reqId << " data:" << reqData
                    << std::endl;

          count++;
          if (count > 10) {
            return false;
          }
          std::this_thread::sleep_for(std::chrono::seconds(1));
          return true;
        },
        ret);
  }).detach();
  reqId = std::to_string(3000);
  reqType = "System";
  reqData = "Quit\n";
  result = "";
  guide->Call(reqId, reqType, reqData, result);
  count = 0;
  std::cout << "----------------------------Publish "
               "2-------------------------------------"
            << std::endl;

  guide->Publish(
      [&](std::string& reqId, std::string& reqType,
          std::string& reqData) -> bool {
        reqId = std::to_string(count);
        reqType = "topic_a";
        reqData = "test" + std::to_string(count);
        std::cout << "Publish 3 id:" << reqId << " data:" << reqData
                  << std::endl;
        count++;
        if (count > 20) {
          return false;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return true;
      },
      ret);

  std::cout << "----------------------------Test "
               "finished-------------------------------------"
            << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(30));

  std::cout << "----------------------------Test1 "
               "finished-------------------------------------"
            << std::endl;
  return 0;
}

#include "backward_wrapper.h"

int main(int argc, char** argv) {
  auto backward = newBackwardWrapper();
  backward->Init("./backward.dumpcore");
  std::string connect_address("localhost:50051");
  client(connect_address);
  return 0;
}
