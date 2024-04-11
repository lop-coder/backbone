#include <stdio.h>

#include <iostream>
#include <memory>

#include "httplib.h"
#include "https.h"

#include <nlohmann/json.hpp>
//////////////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>
#include <stdexcept>
//////////////////////////////////////////////////////////////////////
using nlohmann::json;

namespace Https {
class Router {
 public:
  Router(std::string m, std::string p, routerFunc fn, uint32_t auth = 0)
      : method(std::move(m)),
        path(std::move(p)),
        fn(std::move(fn)),
        m_authCode(auth){};
  ~Router(){};

  std::string method = "";
  std::string path = "";
  routerFunc fn = nullptr;
  uint32_t m_authCode;
};

class HttpServerImpl : public HttpSrv {
 public:
  HttpServerImpl();
  virtual ~HttpServerImpl();

  virtual bool start(const std::string& host, int port) override;
  virtual void stop() override;
  virtual void addRouters(std::string method, std::string path, routerFunc fun,
                          uint32_t auth = 0) override;
  virtual bool status() override;
  virtual void authentication(uint32_t auth) override;

 protected:
  httplib::Server svr;
  void Handler(std::shared_ptr<Router> router, const httplib::Request& req,
               httplib::Response& res);
  void HandlerMultipartFormData(std::shared_ptr<Router> router,
                                const httplib::Request& req,
                                httplib::Response& res,
                                const httplib::ContentReader& content_reader);
  uint32_t m_auth;
  std::unordered_map<std::string, std::shared_ptr<Router>> m_routers;
};

HttpServerImpl::HttpServerImpl() : m_auth(0) {
  svr.set_keep_alive_timeout(10);
  svr.set_read_timeout(5, 0);   // 5 seconds
  svr.set_write_timeout(5, 0);  // 5 seconds
  svr.set_idle_interval(0, 100000);
  svr.set_payload_max_length(1024 * 1024 * 512);
  svr.set_exception_handler(
      [](const auto& req, auto& res, std::exception_ptr ep) {
        std::string result;
        try {
          std::rethrow_exception(ep);
        } catch (std::exception& e) {
          result =
              "{\"code\": -99,"
              "\"msg\": \"method[" +
              std::string(req.method) + "],path[" + std::string(req.path) +
              "], error[" + std::string(e.what()) + "]\", \"body\": \"" +
              req.body + "\" }";

        } catch (...) {  // See the following NOTE
          result =
              "{\"code\": -99,"
              "\"msg\": \"method[" +
              std::string(req.method) + "],path[" + std::string(req.path) +
              "], error[" + "Unknown Exception" + "]\", \"body\": \"" +
              req.body + "\" }";
        }
        return res.set_content(result, "application/json");
      });

  svr.set_error_handler([](const auto& req, auto& res) {
    std::string result =
        "{\"code\": -99,"
        "\"msg\": \"method[" +
        std::string(req.method) + "],path[" + std::string(req.path) +
        "], status[" + std::to_string(res.status) + "]\", \"body\": \"" +
        req.body + "\" }";
    return res.set_content(result, "application/json");
  });

  svr.set_pre_routing_handler(
      [](const httplib::Request& req, httplib::Response& res) {
        return httplib::Server::HandlerResponse::Unhandled;
      });
  svr.set_logger([](const auto& req, const auto& res) {});
}
HttpServerImpl::~HttpServerImpl() {
  stop();
}
bool HttpServerImpl::status() {
  return svr.is_running();
}
void HttpServerImpl::addRouters(std::string method, std::string path,
                                routerFunc fun, uint32_t auth) {
  if (method == "" || path == "" || fun == nullptr) {
    //LogWarn(" Add routers [{} {}] fail!!", method, path);
    return;
  }
  std::shared_ptr<Router> router =
      std::make_shared<Router>(method, path, fun, auth);
  std::string key = method + "-" + path;
  if (m_routers.find(key) != m_routers.end()) {
    //LogWarn(" Add routers [{} {}] fail!!", method, path);
    return;
  }
  /*
  LogTrace("######################register###########################");
  LogTrace("method:{}", router->method);
  LogTrace("whitelist:{}", router->whitelist);
  LogTrace("path:{}", router->path);
  LogTrace("#######################################################");
  */

  std::cout << "######################register###########################"
            << std::endl;
  std::cout << "method:" << router->method << std::endl;
  std::cout << "required auth:" << router->m_authCode << std::endl;
  std::cout << "path:" << router->path << std::endl;
  std::cout << "#######################################################"
            << std::endl;
  if (router->method == "GET" || router->method == "HEAD") {
    svr.Get(router->path, [router, this](const httplib::Request& req,
                                         httplib::Response& res) {
      Handler(router, req, res);
    });
  } else if (router->method == "POST") {
    svr.Post(router->path,
             [router, this](const httplib::Request& req, httplib::Response& res,
                            const httplib::ContentReader& content_reader) {
               HandlerMultipartFormData(router, req, res, content_reader);
             });
  } else if (router->method == "PUT") {
    svr.Put(router->path, [router, this](const httplib::Request& req,
                                         httplib::Response& res) {
      Handler(router, req, res);
    });
  } else if (router->method == "DELETE") {
    svr.Delete(router->path, [router, this](const httplib::Request& req,
                                            httplib::Response& res) {
      Handler(router, req, res);
    });
  } else if (router->method == "OPTIONS") {
    svr.Options(router->path, [router, this](const httplib::Request& req,
                                             httplib::Response& res) {
      Handler(router, req, res);
    });
  } else if (router->method == "PATCH") {
    svr.Patch(router->path, [router, this](const httplib::Request& req,
                                           httplib::Response& res) {
      Handler(router, req, res);
    });
  } else {
    return;
  }
  m_routers[key] = router;
}
bool HttpServerImpl::start(const std::string& host, int port) {
  return svr.listen(host, port);
}

void HttpServerImpl::stop() {
  svr.stop();
  m_routers.clear();
}

void HttpServerImpl::Handler(std::shared_ptr<Router> router,
                             const httplib::Request& req,
                             httplib::Response& res) {
  std::map<std::string, std::string> querys;
  std::map<std::string, std::string> headers;
  std::map<std::string, std::string> form;
  std::vector<std::string> matches;
  try {
    for (auto iter = req.headers.begin(); iter != req.headers.end(); ++iter) {
      headers[iter->first] = iter->second;
    }
    for (auto iter = req.params.begin(); iter != req.params.end(); ++iter) {
      querys[iter->first] = iter->second;
    }
    for (auto iter = req.matches.begin(); iter != req.matches.end(); ++iter) {
      matches.push_back((*iter));
    }
    routerTrace(req.method, req.remote_addr, req.version, req.path, querys,
                headers, matches, form, req.body);

    bool authenticationFlag = false;
    bool canHandler = false;

    if (router->m_authCode == 0) {
      canHandler = true;
    } else {
      if (router->m_authCode & m_auth) {
        canHandler = true;
      } else {
        authenticationFlag = true;
      }
    }
    if (canHandler) {
      auto result =
          router->fn(req.method, req.remote_addr, req.version, req.path, querys,
                     headers, matches, form, req.body);
      res.set_content(result->data, result->type);
      /*
      LogTrace("#######################response##########################");
      LogTrace(result->data);
      LogTrace("##########################end############################");
      */
    } else {
      json resJson;
      std::string message = "path not found:";
      if (authenticationFlag) {
        message = "Unauthorized access";
        resJson["code"] = -3;
      } else {
        resJson["code"] = -1;
      }
      message += req.method;
      message += " ";
      message += req.path;

      resJson["data"] = json::object();

      resJson["msg"] = message;
      res.set_content(resJson.dump().data(), "application/json");
      /*
      LogTrace("#######################response##########################");
      LogTrace(resJson.dump().data());
      LogTrace("##########################end############################");
      */
    }
  } catch (const std::exception& e) {
    std::string result =
        "{\"code\": -99,"
        "\"msg\": \"method[" +
        std::string(req.method) + "],path[" + std::string(req.path) +
        "],catch error[" + std::string(e.what()) + "]\", \"body\": \"" +
        req.body + "\" }";
    return res.set_content(result, "application/json");
  }
}

void HttpServerImpl::HandlerMultipartFormData(
    std::shared_ptr<Router> router, const httplib::Request& req,
    httplib::Response& res, const httplib::ContentReader& content_reader) {
  std::map<std::string, std::string> querys;
  std::map<std::string, std::string> headers;
  std::map<std::string, std::string> form;
  std::vector<std::string> matches;

  std::string body;
  try {
    for (auto iter = req.headers.begin(); iter != req.headers.end(); ++iter) {
      headers[iter->first] = iter->second;
    }
    for (auto iter = req.params.begin(); iter != req.params.end(); ++iter) {
      querys[iter->first] = iter->second;
    }
    for (auto iter = req.matches.begin(); iter != req.matches.end(); ++iter) {
      matches.push_back((*iter));
    }

    if (req.is_multipart_form_data()) {
      httplib::MultipartFormDataItems files;
      content_reader(
          [&](const httplib::MultipartFormData& file) {
            files.push_back(file);
            return true;
          },
          [&](const char* data, size_t data_length) {
            files.back().content.append(data, data_length);
            return true;
          });
      for (auto iter = files.begin(); iter != files.end(); ++iter) {
        querys[iter->filename] = iter->content;
      }
    } else {
      content_reader([&](const char* data, size_t data_length) {
        body.append(data, data_length);
        return true;
      });
    }

    routerTrace(req.method, req.remote_addr, req.version, req.path, querys,
                headers, matches, form, body);
    auto result = router->fn(req.method, req.remote_addr, req.version, req.path,
                             querys, headers, matches, form, body);
    res.set_content(result->data, result->type);
    /*
    LogTrace("#######################response##########################");
    LogTrace(result->data);
    LogTrace("##########################end############################");
    */
  } catch (const std::exception& e) {
    std::string result =
        "{\"code\": -99,"
        "\"msg\": \"method[" +
        std::string(req.method) + "],path[" + std::string(req.path) +
        "],catch error[" + std::string(e.what()) + "]\", \"body\": \"" + body +
        "\" }";
    return res.set_content(result, "application/json");
  }
}

void HttpServerImpl::authentication(uint32_t auth) {
  m_auth = auth;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<HttpSrv> newHttpServer() {
  return std::make_shared<HttpServerImpl>();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static std::shared_ptr<HttpSrv> pDefaultHttpServer = nullptr;
static std::mutex mutx;

std::shared_ptr<HttpSrv> defaultHttpServer() {
  if (pDefaultHttpServer == nullptr) {
    std::lock_guard<std::mutex> lck(mutx);
    if (pDefaultHttpServer == nullptr) {
      pDefaultHttpServer = std::make_shared<HttpServerImpl>();
    }
  }
  return pDefaultHttpServer;
}
void destoryDefaultHttpServer() {
  if (pDefaultHttpServer != nullptr) {
    pDefaultHttpServer = nullptr;
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void routerTrace(std::string method, std::string host, std::string proto,
                 std::string path, std::map<std::string, std::string> querys,
                 std::map<std::string, std::string> headers,
                 std::vector<std::string> matches,
                 std::map<std::string, std::string> form, std::string body) {
  /*
  LogTrace("######################request###########################");
  LogTrace("method:{}", method);
  LogTrace("host:{}", host);
  LogTrace("proto:{}", proto);
  LogTrace("path:{}", path);

  LogTrace("querys");
  */
  for (auto iter = querys.begin(); iter != querys.end(); iter++) {
    //LogTrace("  {}{}{}", iter->first, "=", iter->second);
  }
  //LogTrace("header");
  for (auto iter = headers.begin(); iter != headers.end(); iter++) {
    // LogTrace("  {}{}{}", iter->first, ":", iter->second);
  }
  //LogTrace("matche");
  for (auto iter = matches.begin(); iter != matches.end(); iter++) {
    std::cout << (*iter) << std::endl;
  }
  //LogTrace("form");
  for (auto iter = form.begin(); iter != form.end(); iter++) {
    //LogTrace("  {}{}{}", iter->first, ":", iter->second);
  }
  //LogTrace("body:{}", body);
  //LogTrace("#######################end##########################");
}
}  // namespace Https
