#pragma once
#include <stdio.h>

#include <chrono>
#include <iostream>
#include <memory>

#include "extension.h"

class BACKBONE_PLUGIN_API TimeMeasurement {
 public:
  virtual void start();
  //纳秒
  virtual double nanoseconds();
  //微秒
  virtual double microseconds();
  //毫秒
  virtual double milliseconds();
  //秒
  virtual double seconds();

 private:
  std::chrono::steady_clock::time_point _start;
};

#define TIMEMEASUREMENT_IDENTITY "TimeMeasurement"
#define TIMEMEASUREMENT_VERSION "v1.0.0"
#define TIMEMEASUREMENT_DESCRIPTION "TimeMeasurement"
