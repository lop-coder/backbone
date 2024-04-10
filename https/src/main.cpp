#include <stdio.h>

#include <iostream>
#include <memory>

#include "logger.h"
#include "logic.h"

#include "message.h"
#include "wrapper.h"

#include "common.h"
#include "http.h"
#include "router/dataset.h"
#include "router/model.h"
#include "router/pipeline.h"
#include "router/project.h"
#include "router/sam.h"
#include "router/sdk.h"
#include "router/systeminfo.h"
#include "router/tag.h"
#include "router/task.h"
#if ISDEBUG
#ifdef LEAK_DETECTOR
#include <vld.h>
#endif
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool parseParameter(size_t argc, char* argv[], std::string name,
                           std::string& word) {
  std::string work_tmp = "";

  for (size_t i = 0; i < argc; i++) {
    // '-'或'--'开头为参数标识
    if ("-" + name == argv[i] || "--" + name == argv[i]) {
      // 参数后没有内容，则返回空字符
      if (argc < i + 1) {
        if (work_tmp != "") {
          word = work_tmp;
        }
        return true;
      }
      // 从参数位的后一位开始取值
      for (size_t j = i + 1; j < argc; j++) {
        std::string curr = argv[j];
        // 直到遇到'-'开头则停止
        if ("-" == curr.substr(0, 1)) {
          if (work_tmp != "") {
            word = work_tmp;
          }
          return true;
        }
        // 拼接空格
        if (j != i + 1) {
          work_tmp += " ";
        }
        // 拼接当前数据
        work_tmp += argv[j];
      }
      if (work_tmp != "") {
        word = work_tmp;
      }
      return true;
    }
  }
  if (work_tmp != "") {
    word = work_tmp;
  }
  return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "config.h"
int main(int argc, char* argv[]) {
  std::cout << "TurboTask Run argv:" << argc << std::endl;

  std::string local_ip = "";
  std::string http_port = "8079";
  std::string websocket_port = "8081";
  {
    if (parseParameter(argc, argv, "http_port", http_port)) {
      std::cout << "http_port is " << http_port << std::endl;
      ;
    }
    if (parseParameter(argc, argv, "websocket_port", websocket_port)) {
      std::cout << "websocket_port is " << websocket_port << std::endl;
    }
    if (parseParameter(argc, argv, "local_ip", local_ip)) {
      std::cout << "local_ip is " << local_ip << std::endl;
    }

    if (http_port == "") {
      http_port = "8080";
    }

    if (websocket_port == "") {
      websocket_port = "8081";
    }

    if (local_ip == "") {
      local_ip = "";
    }

    std::cout << "http_port is " << http_port << std::endl;
    std::cout << "websocket_port is " << websocket_port << std::endl;
  }

  auto logger = TurboLogger::getLogger("logicTest");
  if (logger == nullptr) {
    logger = TurboLogger::newLogger(".", "logicTest", "logic.conf");
    TurboLogger::setDefaultLogger("logicTest");
  }
  LogInfo("======================================================");
  LogInfo("PROJECT_NAME:{}", PROJECT_NAME);
  LogInfo("COMPILE_TIME:{}", COMPILE_TIME);
  LogInfo("COMPILE_TYPE:{}", COMPILE_TYPE);
  LogInfo("TARGET_OS:{}", TARGET_OS);
  LogInfo("SOFTWARE_VERSION:{}", SOFTWARE_VERSION);
  LogInfo("======================================================");
  LogInfo("http_port:{}", http_port);
#if ISDEBUG
  auto coredump = TurboLogger::newLoggerCoreDump();
#ifndef LEAK_DETECTOR
  coredump->Init("logicTest.coredump");
#else
  LogInfo("LEAK_DETECTOR:{}", LEAK_DETECTOR);
#endif
#endif

  TurboLogic::Logic& handle =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);

  int errCode = 0;
  // std::string msg = "";
  // Init logger
  logger = TurboLogger::getLogger("logic");
  if (logger == nullptr) {
    logger = TurboLogger::newLogger(
        TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local)
            .systemInfo()
            ->SystemPath(),
        "logic", "logic.conf");
    TurboLogger::setDefaultLogger("logic");
  }
  LogInfo("======================================================");
  LogInfo("PROJECT_NAME:{}", PROJECT_NAME);
  LogInfo("COMPILE_TIME:{}", COMPILE_TIME);
  LogInfo("COMPILE_TYPE:{}", COMPILE_TYPE);
  LogInfo("TARGET_OS:{}", TARGET_OS);
  LogInfo("SOFTWARE_VERSION:{}", SOFTWARE_VERSION);
  LogInfo("======================================================");
  LogInfo("http_port:{}", http_port);
#if ISDEBUG
  auto coredump = TurboLogger::newLoggerCoreDump();
#ifndef LEAK_DETECTOR
  coredump->Init(TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local)
                     .systemInfo()
                     ->SystemPath() +
                 "/" + "logic.coredump");
#else
  LogInfo("LEAK_DETECTOR:{}", LEAK_DETECTOR);
#endif
#endif

  std::shared_ptr<ProjectRouter> projectRouter =
      std::make_shared<ProjectRouter>();

  std::shared_ptr<TagRouter> tagRouter = std::make_shared<TagRouter>();
  std::shared_ptr<DatasetRouter> datasetRouter =
      std::make_shared<DatasetRouter>();
  std::shared_ptr<ModelRouter> modelRouter = std::make_shared<ModelRouter>();
  std::shared_ptr<PipelineRouter> pipelineRouter =
      std::make_shared<PipelineRouter>();

  std::shared_ptr<SDKRouter> sdkRouter = std::make_shared<SDKRouter>();
  std::shared_ptr<SystemInfoRouter> systemInfoRouter =
      std::make_shared<SystemInfoRouter>();
  std::shared_ptr<TaskRouter> taskRouter = std::make_shared<TaskRouter>();
  std::shared_ptr<SAMRouter> samRouter = std::make_shared<SAMRouter>();
  bool status = true;
  int port = std::stoi(http_port);

  std::thread([&] {
    while (status) {
      if (TurboHttp::defaultHttpServer()->status()) {
        g_status = "online";
      } else {
        g_status = "offline";
      }
      //LogInfo(packHeartBeat());
      std::cout << packHeartBeat() << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(3));
    }
  }).detach();
  TurboLogic::ActivateResponse res;
  handle.project()->getActivateInfo(res);
  TurboHttp::defaultHttpServer()->authentication(
      checkVerifyStatus(res.authorizationStatus));
#ifdef LEAK_DETECTOR
  int check_time = LEAK_DETECTOR;
  std::thread([check_time]() {
    std::this_thread::sleep_for(std::chrono::seconds(check_time));
    TurboHttp::defaultHttpServer()->stop();
  }).detach();
#endif
  int tryCount = 6;
  while (tryCount >= 0) {
    g_http_port = port;

    if (TurboHttp::defaultHttpServer()->start(local_ip, port)) {
      break;
    }
    std::cerr << "http server [" << std::to_string(port) << "] start error!!"
              << std::endl;
    port++;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    tryCount--;
  }
  status = false;
  LogWarn("turbohttp exit!!");
  std::this_thread::sleep_for(std::chrono::seconds(5));
  TurboHttp::destoryDefaultHttpServer();
  return 0;
}
