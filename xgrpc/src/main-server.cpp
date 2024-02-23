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

void RunServer(const std::string& server_address) {
  std::map<std::string, int> counts;

  auto turboRPCService = newServer();
  turboRPCService->RegisterCallHandler(
      "call_func1",
      [](const std::string& reqId, const std::string& reqType,
         const std::string& reqData, std::string& result) -> bool {
        result = reqData + "-func1";

        return true;
      });

  turboRPCService->RegisterCallHandler(
      "call_func2",
      [](const std::string& reqId, const std::string& reqType,
         const std::string& reqData, std::string& result) -> bool {
        result = reqData + "-func2";
        return true;
      });
  turboRPCService->RegisterCallHandler(
      "call_func3",
      [](const std::string& reqId, const std::string& reqType,
         const std::string& reqData, std::string& result) -> bool {
        result = reqData + "-func3";
        return true;
      });

  turboRPCService->RegisterPullHandler(
      "pull_func1",
      [&counts](const std::string& reqId, const std::string& reqType,
                const std::string& reqData, std::string& result) -> bool {
        std::string id = reqId;
        std::string data = reqData;
        if (counts.find(id) == counts.end()) {
          counts[id] = 10;
        }
        if (counts[id] > 0) {
          counts[id] = --counts[id];
        }
        //std::cout<<"pull server pull_func1 4 "<<std::endl;
        if (counts[id] <= 0) {
          //std::cout<<"pull server pull_func1 false id:"<<id<<" count:"<<counts.at(id)<<std::endl;
          return false;
        }
        result = data + "-func1-" + std::to_string(counts[id]);
        //std::cout<<"pull server pull_func1 true id:"<<id<<" count:"<<counts.at(id)<<std::endl;
        return true;
      });
  counts.clear();

  turboRPCService->RegisterPullHandler(
      "pull_func2",
      [&counts](const std::string& reqId, const std::string& reqType,
                const std::string& reqData, std::string& result) -> bool {
        std::string id = reqId;
        std::string data = reqData;
        if (counts.find(id) == counts.end()) {
          counts[id] = 10;
        }

        if (counts[id] > 0) {
          counts[id] = --counts[id];
        }
        if (counts[id] <= 0) {
          //std::cout<<"pull server pull_func2 false id:"<<id<<" count:"<<counts.at(id)<<std::endl;
          return false;
        }

        result = data + "-func2-" + std::to_string(counts[id]);
        //std::cout<<"pull server pull_func2 id:"<<id<<" count:"<<counts.at(id)<<std::endl;
        return true;
      });
  counts.clear();

  turboRPCService->RegisterPushHandler(
      "push_func1",
      [](const std::string& reqId, const std::string& reqType,
         const std::string& reqData, std::string& result) -> bool {
        std::string id = reqId;
        std::string data = reqData;
        //std::cout<<"push server push_func1 id:"<<id<<" count:"<<data<<std::endl;
        return true;
      });
  turboRPCService->RegisterPushHandler(
      "push_func2",
      [](const std::string& reqId, const std::string& reqType,
         const std::string& reqData, std::string& result) -> bool {
        std::string id = reqId;
        std::string data = reqData;

        //std::cout<<"push server push_func2 id:"<<id<<" count:"<<data<<std::endl;
        result = "Just for Test";
        return false;
      });

  turboRPCService->RegisterBidiHandler(
      "bidi_func1",
      [&counts](const std::string& reqId, const std::string& reqType,
                const std::string& reqData, std::string& result) -> bool {
        std::string id = reqId;
        std::string data = reqData;

        if (counts.find(id) == counts.end()) {
          counts[id] = 10;
        }
        if (counts[id] > 0) {
          counts[id] = --counts[id];
        }
        result = data + "-func1-" + std::to_string(counts[id]);
        if (counts[id] <= 0) {
          //std::cout<<"bidi server bidi_func1 false id:"<<id<<" count:"<<counts.at(id)<<std::endl;
          return false;
        }
        //std::cout<<"bidi server read bidi_func1 id:"<<id<<" count:"<<data<<std::endl;
        return true;
      },
      [](const std::string& reqId, const std::string& reqType,
         const std::string& reqData, std::string& result) -> bool {
        std::string id = reqId;
        std::string data = reqData;
        //std::cout<<"bidi server write bidi_func1 id:"<<id<<" count:"<<data<<std::endl;
        result = "Just for Test";
        return true;
      });
  auto serverRun = newServerRun();
  serverRun->Install(turboRPCService.get());
  serverRun->Run(server_address);
}

#include "backward_wrapper.h"

int main(int argc, char** argv) {
  auto backward = newBackwardWrapper();
  backward->Init("./backward.dumpcore");

  std::string server_address("0.0.0.0:50051");
  try {
    RunServer(server_address);
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
