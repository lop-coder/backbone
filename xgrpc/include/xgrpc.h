#ifndef _TURBO_RPC_H_
#define _TURBO_RPC_H_
#include <memory>
#include <string>
#include "../../extension/include/extension.h"

#ifndef _WIN32
#define XGRPC_API
#elif defined XGRPC_DLL_EXPORTS
#define XGRPC_API __declspec(dllexport)
#else
#define XGRPC_API __declspec(dllimport)
#endif

namespace BackboneGRPC {
class XGRPC_API Client {
 public:
  explicit Client(){};
  virtual ~Client(){};
  virtual bool Initialize(const std::string& server_address) = 0;
  virtual bool Call(const std::string& reqId, const std::string& reqType,
                    const std::string& reqData, std::string& result) = 0;

  virtual bool Pull(
      const std::string& reqId, const std::string& reqType,
      const std::string& reqData,
      std::function<bool(const std::string& resId, const std::string& resType,
                         const std::string& resData)>
          func) = 0;
  virtual bool Push(std::function<bool(std::string& reqId, std::string& reqType,
                                       std::string& reqData)>
                        func,
                    std::string& result) = 0;
  virtual bool Bidi(
      std::function<bool(std::string& reqId, std::string& reqType,
                         std::string& reqData)>
          wfunc,
      std::function<bool(const std::string& resId, const std::string& resType,
                         const std::string& resData)>
          rfunc) = 0;

  virtual bool Subscribe(
      const std::string& reqId, const std::string& reqType,
      const std::string& reqData,
      std::function<bool(const std::string& resId, const std::string& resType,
                         const std::string& resData)>
          func) = 0;
  virtual bool Publish(
      std::function<bool(std::string& reqId, std::string& reqType,
                         std::string& reqData)>
          func,
      std::string& result) = 0;
  virtual bool Publish(const std::string& reqId, const std::string& reqType,
                       const std::string& reqData, std::string& result) = 0;
};

class XGRPC_API Server {
 public:
  Server(){};
  virtual ~Server(){};
  virtual bool RegisterCallHandler(
      const std::string& type,
      std::function<bool(const std::string& reqId, const std::string& reqType,
                         const std::string& reqData, std::string& result)>
          func) = 0;
  virtual bool RegisterPullHandler(
      const std::string& type,
      std::function<bool(const std::string& reqId, const std::string& reqType,
                         const std::string& reqData, std::string& result)>
          func) = 0;
  virtual bool RegisterPushHandler(
      const std::string& type,
      std::function<bool(const std::string& reqId, const std::string& reqType,
                         const std::string& reqData, std::string& result)>
          func) = 0;
  virtual bool RegisterBidiHandler(
      const std::string& type,
      std::function<bool(const std::string& reqId, const std::string& reqType,
                         const std::string& reqData, std::string& result)>
          rfunc,
      std::function<bool(const std::string& resId, const std::string& reqsType,
                         const std::string& resData, std::string& result)>
          wfunc) = 0;
  virtual bool Subscribe(
      const std::string& reqId, const std::string& reqType,
      std::function<bool(const std::string& resId, const std::string& resType,
                         const std::string& resData)>
          func) = 0;
  virtual bool Publish(const std::string& reqType,
                       const std::string& reqData) = 0;
};

class XGRPC_API ServerRun {
 public:
  ServerRun(){};
  virtual ~ServerRun(){};
  virtual bool Install(Server* ser) = 0;
  virtual bool Run(const std::string& server_address) = 0;
};

XGRPC_API std::shared_ptr<Client> newClient();
XGRPC_API std::shared_ptr<Server> newServer();
XGRPC_API std::shared_ptr<ServerRun> newServerRun();
}  // namespace BackboneGRPC

#ifdef __cplusplus
extern "C" {
#endif
#define XGRPC_IDENTITY "XgRPC"
#define XGRPC_VERSION "v1.0.0"
#define XGRPC_DESCRIPTION "XgRPC"
#ifdef __cplusplus
}
#endif

#endif
