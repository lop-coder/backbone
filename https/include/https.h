#ifndef _HTTPS_H_
#define _HTTPS_H_
#include <stdio.h>
#include <iostream>
#include <memory>
//////////////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <map>
#include <functional>
//////////////////////////////////////////////////////////////////////
namespace Https {

struct resMsg {
  resMsg(std::string d, std::string type = "application/json")
      : type(type), data(d){};
  std::string type;
  std::string data;
};

typedef std::function<std::shared_ptr<resMsg>(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::vector<std::string> matches, std::map<std::string, std::string> form,
    std::string body)>
    routerFunc;

class HttpSrv {
 public:
  HttpSrv(){};
  virtual ~HttpSrv(){};

  virtual bool start(const std::string& host, int port) = 0;
  virtual void stop() = 0;
  virtual void addRouters(std::string method, std::string path, std::function<std::shared_ptr<resMsg>(
                              std::string method, std::string host, std::string proto, std::string path,
                              std::map<std::string, std::string> querys,
                              std::map<std::string, std::string> headers,
                              std::vector<std::string> matches, std::map<std::string, std::string> form,
                              std::string body)> fun,
                          bool wlist = true) = 0;
  virtual bool status() = 0;
  virtual void authentication(bool verifyStatus) = 0;
};

std::shared_ptr<HttpSrv> newHttpServer();
std::shared_ptr<HttpSrv> defaultHttpServer();
void destoryDefaultHttpServer();
void routerTrace(std::string method, std::string host, std::string proto,
                 std::string path, std::map<std::string, std::string> querys,
                 std::map<std::string, std::string> headers,
                 std::vector<std::string> matches,
                 std::map<std::string, std::string> form, std::string body);

}  // namespace Https

#define REGISTER_ROUTER(METHOD, PATH, FUNC)                                \
  Https::defaultHttpServer()->addRouters(                              \
      METHOD, PATH,                                                        \
      std::bind(&FUNC, this, std::placeholders::_1, std::placeholders::_2, \
                std::placeholders::_3, std::placeholders::_4,              \
                std::placeholders::_5, std::placeholders::_6,              \
                std::placeholders::_7, std::placeholders::_8,              \
                std::placeholders::_9))

#define REGISTER_ROUTEREX(METHOD, PATH, FUNC, WHITELIST)                   \
  Https::defaultHttpServer()->addRouters(                              \
      METHOD, PATH,                                                        \
      std::bind(&FUNC, this, std::placeholders::_1, std::placeholders::_2, \
                std::placeholders::_3, std::placeholders::_4,              \
                std::placeholders::_5, std::placeholders::_6,              \
                std::placeholders::_7, std::placeholders::_8,              \
                std::placeholders::_9),                                    \
      WHITELIST)

#define OUTSIDE_REGISTER_ROUTER(OBJECT, METHOD, PATH, FUNC)                  \
  Https::defaultHttpServer()->addRouters(                                \
      METHOD, PATH,                                                          \
      std::bind(&FUNC, OBJECT, std::placeholders::_1, std::placeholders::_2, \
                std::placeholders::_3, std::placeholders::_4,                \
                std::placeholders::_5, std::placeholders::_6,                \
                std::placeholders::_7, std::placeholders::_8,                \
                std::placeholders::_9))

#define OUTSIDE_REGISTER_ROUTEREX(OBJECT, METHOD, PATH, FUNC, WHITELIST)     \
  Https::defaultHttpServer()->addRouters(                                \
      METHOD, PATH,                                                          \
      std::bind(&FUNC, OBJECT, std::placeholders::_1, std::placeholders::_2, \
                std::placeholders::_3, std::placeholders::_4,                \
                std::placeholders::_5, std::placeholders::_6,                \
                std::placeholders::_7, std::placeholders::_8,                \
                std::placeholders::_9),                                      \
      WHITELIST)

#define DECLARE_ROUTER(NAME)                                       \
  std::shared_ptr<Https::resMsg> NAME(                         \
      std::string method, std::string host, std::string proto,     \
      std::string path, std::map<std::string, std::string> querys, \
      std::map<std::string, std::string> headers,                  \
      std::vector<std::string> matches,                            \
      std::map<std::string, std::string> form, std::string body)

#define DEFINITION_ROUTER(CLASS, FUNC)                             \
  std::shared_ptr<Https::resMsg> CLASS::FUNC(                  \
      std::string method, std::string host, std::string proto,     \
      std::string path, std::map<std::string, std::string> querys, \
      std::map<std::string, std::string> headers,                  \
      std::vector<std::string> matches,                            \
      std::map<std::string, std::string> form, std::string body)
#endif
