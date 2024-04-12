
#include <stdio.h>
#include <iostream>
#include <memory>

#if ISDEBUG
#ifdef LEAK_DETECTOR
#include <vld.h>
#endif
#endif

#ifdef LOGGER_DLL_EXPORTS
#undef LOGGER_DLL_EXPORTS
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "config.h"
#include "logger.h"
int main(int argc, char* argv[]) {
  auto loggerCfg =
      backbone::Logger::getInstance()->getCfg("", "logicTest", "logger.conf");
  auto logger = backbone::Logger::getInstance()->create(loggerCfg, "logicTest");
  backbone::Logger::getInstance()->setDefaultLogger(logger);
  LogInfo("======================================================");
  LogInfo("PROJECT_NAME:{}", PROJECT_NAME);
  LogInfo("COMPILE_TIME:{}", COMPILE_TIME);
  LogInfo("COMPILE_TYPE:{}", COMPILE_TYPE);
  LogInfo("TARGET_OS:{}", TARGET_OS);
  LogInfo("SOFTWARE_VERSION:{}", SOFTWARE_VERSION);
  LogInfo("======================================================");
#if ISDEBUG
  auto coredump = backbone::newLoggerCoreDump();
#ifndef LEAK_DETECTOR
  coredump->Init("logicTest.coredump");
#else
  LogInfo("LEAK_DETECTOR:{}", LEAK_DETECTOR);
#endif
#endif
}
