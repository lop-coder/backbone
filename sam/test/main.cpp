#include <stdio.h>

#include <iostream>
#include <memory>

#include <gtest/gtest.h>
#include "../include/turborpc.h"

// Demonstrate some basic assertions.
TEST(LoggerInit, Base) {
  // Init logger
  /*
  auto logger = TurboLogger::getLogger("logger");
  // Expect equality.
  EXPECT_EQ(logger, nullptr);
  logger = TurboLogger::newLogger("", "logger", "logger.conf");
  TurboLogger::setDefaultLogger("logger");
  EXPECT_NE(logger, nullptr);
  */
}
int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
