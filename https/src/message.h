#ifndef _MESSAGE_H
#define _MESSAGE_H
#include <functional>
#include <map>
#include <mutex>
#include <regex>
#include "util.h"
std::string packHeartBeat();
void handleRecvMessage();
extern int g_http_port;
extern std::string g_status;
#endif  //_MESSAGE_H
