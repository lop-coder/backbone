#include <stdio.h>

#include <iostream>
#include <memory>

#include "extension-timemeasurement.h"

void TimeMeasurement::start() {
  _start = std::chrono::steady_clock::now();
}

//纳秒
double TimeMeasurement::nanoseconds() {
  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::steady_clock::now() - _start);
  return double(duration.count()) * std::chrono::nanoseconds::period::num /
         std::chrono::nanoseconds::period::den;
}
//微秒
double TimeMeasurement::microseconds() {
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
      std::chrono::steady_clock::now() - _start);
  return double(duration.count()) * std::chrono::microseconds::period::num /
         std::chrono::microseconds::period::den;
}
//毫秒
double TimeMeasurement::milliseconds() {
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::steady_clock::now() - _start);
  return double(duration.count()) * std::chrono::milliseconds::period::num /
         std::chrono::milliseconds::period::den;
}
//秒
double TimeMeasurement::seconds() {
  auto duration = std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::steady_clock::now() - _start);
  return double(duration.count()) * std::chrono::seconds::period::num /
         std::chrono::seconds::period::den;
}

#ifdef __cplusplus
extern "C" {
#endif

std::string identity(void) {
  return TIMEMEASUREMENT_IDENTITY;
}
std::string version(void) {
  return TIMEMEASUREMENT_VERSION;
}
std::string description(void) {
  return TIMEMEASUREMENT_DESCRIPTION;
}
void* create(const std::string& cfg) {
  return new TimeMeasurement();
}
void destroy(void* p) {
  if (p) {
    auto tp = reinterpret_cast<TimeMeasurement*>(p);
    delete tp;
  }
}
#ifdef __cplusplus
}
#endif
