#include "message.h"

#include <iostream>

#include "logic.h"
#include "nlohmann/json.hpp"
#include "wrapper.h"

using nlohmann::json;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string g_status = "offline";
int g_http_port = 8080;
static std::string wrapper_version = WRAPPER_VERSION;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static std::string GetCompileDateTime() {
  const char* pMonth[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  char Mmm[4] = "Jan";

  int tm_sec;      // seconds after the minute - [0, 60] including leap second
  int tm_min;      // minutes after the hour - [0, 59]
  int tm_hour;     // hours since midnight - [0, 23]
  int tm_mday;     // day of the month - [1, 31]
  int tm_mon = 0;  // months since January - [0, 11]
  int tm_year;     // years since 1900
  int tm_wday;     // days since Sunday - [0, 6]
  int tm_yday;     // days since January 1 - [0, 365]
  int tm_isdst;    // daylight savings time flag

  sscanf(__TIME__, "%d:%d:%d", &tm_hour, &tm_min, &tm_sec);
  sscanf(__DATE__, "%3s %d %d", &Mmm, &tm_mday, &tm_year);
  // tm_year -= 1900;

  for (int i = 0; i < 12; i++) {
    if (memcmp(Mmm, pMonth[i], 3) == 0) {
      tm_mon = i + 1;
      break;
    }
  }

  tm_isdst = tm_wday = tm_yday = 0;
  std::string dataTime = std::to_string(tm_year) + std::to_string(tm_mon) +
                         std::to_string(tm_mday) + std::to_string(tm_hour) +
                         std::to_string(tm_min) + std::to_string(tm_sec);
  return dataTime;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string packHeartBeat() {
  json heart_beat = {
      {"version",
       wrapper_version + "-" + GetCompileDateTime() + " " +
           TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local)
               .version()},
      {"http_port", g_http_port},
      {"status", g_status}};
  json res;
  res["heart_beat"] = heart_beat;
  std::string msg = res.dump().data() + std::string("\n");
  return msg;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
void handleRecvMessage() {
  char* msg = getRecvMsg();
  try {
    auto request = json::parse(std::string(msg));
    if (request.contains("heart_beat")) {
      if (request["heart_beat"].contains("http_port")) {
        if (request["heart_beat"]["http_port"].is_number_integer()) {
          //  g_http_port = request["heart_beat"]["http_port"].get<int>();
        }
      }
      if (request["heart_beat"].contains("status")) {
        if (request["heart_beat"]["status"].is_string()) {
          //g_status = request["heart_beat"]["status"].get<std::string>();
        }
      }
      if (request["heart_beat"].contains("version")) {
        if (request["heart_beat"]["version"].is_string()) {
          // version=request["heart_beat"]["version"].get<std::string>();
        }
      }
      sendMsg(dupString(packHeartBeat().data()));
    }
  } catch (const nlohmann::detail::parse_error& e) {
    printf("json parse: %s\n", e.what());
  }
}*/
