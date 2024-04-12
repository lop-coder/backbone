#include <stdio.h>

#include <iostream>
#include <memory>

#include <gtest/gtest.h>


#ifdef LOGGER_DLL_EXPORTS
#undef LOGGER_DLL_EXPORTS
#endif
#include "../include/logger.h"
#include "../src/logger.cpp"
// Demonstrate some basic assertions.
TEST(LoggerInit, Base) {
  // Init logger
  // Expect equality.
  auto loggerCfg = Logger::getInstance()->getCfg("", "logger", "logger.conf");
  auto logger = Logger::getInstance()->create(loggerCfg, "logger");
  EXPECT_NE(logger, nullptr);
  Logger::getInstance()->setDefaultLogger(logger);
}
int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
