#include <stdio.h>

#include <iostream>
#include <memory>

#include <gtest/gtest.h>

//非常重要
#ifdef EXTENSION_DLL_EXPORTS
#undef EXTENSION_DLL_EXPORTS
#endif
#include "../../extensions/timemeasurement/extension-timemeasurement.h"
#include "backbone.h"

TEST(TimeMeasurementTest, Base) {
  EXPECT_NO_THROW(
      Backbone::Hub::load("./", "timemeasurement", "TimeMeasurement"));
  EXPECT_NE(Backbone::Hub::create<TimeMeasurement>("TimeMeasurement", ""),
            nullptr);
  EXPECT_NO_THROW(
      Backbone::Hub::create<TimeMeasurement>("TimeMeasurement", ""));
  EXPECT_NO_THROW(
      Backbone::Hub::create<TimeMeasurement>("TimeMeasurement", ""));
  //EXPECT_EQ(Backbone::Hub::create<TimeMeasurement>("TimeMeasurement","")->name(),"TimeMeasurement");

  auto timeMeasurement =
      Backbone::Hub::create<TimeMeasurement>("TimeMeasurement", "");
  EXPECT_NO_THROW(timeMeasurement->start());
  EXPECT_NO_THROW(timeMeasurement->nanoseconds());
  EXPECT_NO_THROW(timeMeasurement->microseconds());
  EXPECT_NO_THROW(timeMeasurement->milliseconds());
  EXPECT_NO_THROW(timeMeasurement->seconds());

  timeMeasurement.reset();
  EXPECT_NO_THROW(Backbone::Hub::unload("TimeMeasurement"));
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
