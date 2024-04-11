#include "xgrpc.h"
#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <future>
#include <iostream>

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include "proto/xgrpc/xgrpc.grpc.pb.h"
using backbonerpc::Request;
using backbonerpc::Response;
using backbonerpc::XgRPC;
using grpc::CallbackServerContext;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::Status;
using grpc::StatusCode;

namespace BackboneGRPC {
class ClientCtx {
 public:
  ClientCtx() : _result(false), _done(false){};
  ~ClientCtx() {}
  void Cancel() {
    std::lock_guard<std::mutex> lock(_mtx);
    if (!_done) {
      _done = true;
      _cv.notify_one();
    }
  }
  bool _result = false;
  std::mutex _mtx;
  std::condition_variable _cv;
  bool _done = false;
  ClientContext _context;

  Request _request;
  Response _response;
};
class ClientImpl : public Client {
 public:
  explicit ClientImpl() : Client(), _stub(nullptr), _channel(nullptr) {}
  ~ClientImpl() { ClearCtx(); };
  virtual bool Initialize(const std::string& server_address) final override {
    grpc::ChannelArguments args;
    args.SetInt(GRPC_ARG_KEEPALIVE_TIME_MS, 20 * 1000);
    args.SetInt(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, 10 * 1000);
    args.SetInt(GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS, 1);
    _channel = grpc::CreateCustomChannel(
        server_address, grpc::InsecureChannelCredentials(), args);
    ;
    _stub = XgRPC::NewStub(_channel);
    if (!_stub) {
      std::cerr << "_stub is nullptr" << std::endl;
      return false;
    }
    return true;
  }
  void AddCtx(std::shared_ptr<ClientCtx>& ctx) {
    std::lock_guard<std::mutex> lock(_mtx);
    std::vector<std::weak_ptr<ClientCtx>> tmp;
    bool changed = false;
    for (auto iter = _ctxs.begin(); iter != _ctxs.end(); iter++) {
      if (iter->expired()) {
        changed = true;
      } else {
        tmp.push_back((*iter));
      }
    }
    if (changed) {
      _ctxs = std::move(tmp);
    }
    _ctxs.push_back(ctx);
  };
  void ClearCtx() {
    std::lock_guard<std::mutex> lock(_mtx);
    for (auto iter = _ctxs.begin(); iter != _ctxs.end(); iter++) {
      if (auto ctx = iter->lock()) {
        std::cout << " Ctx Cancel" << std::endl;
        ctx->Cancel();
      }
    }
    _ctxs.clear();
  };
  virtual bool Call(const std::string& reqId, const std::string& reqType,
                    const std::string& reqData,
                    std::string& result) final override {
    if (!_stub) {
      std::cerr << "Initialization failed or not initialized" << std::endl;
      return false;
    }
    auto _ctx = std::make_shared<ClientCtx>();
    AddCtx(_ctx);

    _ctx->_request.set_id(reqId);
    _ctx->_request.set_type(reqType);
    _ctx->_request.set_data(reqData);
    /*
    ClientContext _context;
    bool _result = false;
    std::mutex _mtx;
    std::condition_variable _cv;
    bool _done = false;
*/
    _stub->async()->Call(&_ctx->_context, &_ctx->_request, &_ctx->_response,
                         [&_ctx, this](Status status) {
                           if (!status.ok()) {
                             std::cout << "Call rpc failed." << std::endl;
                             _ctx->_result = false;
                           } else {
                             _ctx->_result = true;
                           }
                           std::lock_guard<std::mutex> lock(_ctx->_mtx);
                           _ctx->_done = true;
                           _ctx->_cv.notify_one();
                         });
    std::unique_lock<std::mutex> lock(_ctx->_mtx);
    _ctx->_cv.wait(lock, [&_ctx] { return _ctx->_done; });
    result = _ctx->_response.data();
    return _ctx->_result;
  }
  virtual bool Pull(
      const std::string& reqId, const std::string& reqType,
      const std::string& reqData,
      std::function<bool(const std::string& resId, const std::string& resType,
                         const std::string& resData)>
          func) final override {
    if (!_stub) {
      std::cerr << "Initialization failed or not initialized" << std::endl;
      return false;
    }
    if (GRPC_CHANNEL_READY != _channel->GetState(true)) {
      return false;
    }
    class Reader : public grpc::ClientReadReactor<Response> {
     public:
      Reader(XgRPC::Stub* stub, std::shared_ptr<ClientCtx>& ctx,
             const std::string& reqId, const std::string& reqType,
             const std::string& reqData,
             std::function<bool(const std::string& resId,
                                const std::string& resType,
                                const std::string& resData)>
                 rfunc)
          : _rfunc(rfunc), _ctx(ctx) {

        _ctx->_request.set_id(reqId);
        _ctx->_request.set_type(reqType);
        _ctx->_request.set_data(reqData);

        stub->async()->Pull(&_ctx->_context, &_ctx->_request, this);
        StartRead(&_ctx->_response);
        StartCall();
      }
      void OnReadDone(bool ok) override {
        if (ok) {
          if (_rfunc(_ctx->_response.id(), _ctx->_response.type(),
                     _ctx->_response.data())) {
            StartRead(&_ctx->_response);
          }
        } else {
          std::cout << "Pull OnReadDone failed" << std::endl;
          //OnDone(Status(StatusCode::DATA_LOSS,"Read failed"));
        }
      }
      void OnDone(const Status& s) override {
        std::unique_lock<std::mutex> l(_ctx->_mtx);
        _status = s;
        _ctx->_done = true;
        _ctx->_cv.notify_one();
      }
      Status Await() {
        std::unique_lock<std::mutex> l(_ctx->_mtx);
        _ctx->_cv.wait(l, [this] { return _ctx->_done; });
        return std::move(_status);
      }

     private:
      std::function<bool(const std::string& resId, const std::string& resType,
                         const std::string& resData)>
          _rfunc = nullptr;

      /*
      XgRPC::Response _response;
      backbone::Request _request;
      ClientContext _context;
      std::mutex _mtx;
      std::condition_variable _cv;
      bool _done = false;
      */
      Status _status;
      std::shared_ptr<ClientCtx> _ctx;
    };

    auto _ctx = std::make_shared<ClientCtx>();
    AddCtx(_ctx);

    Reader reader(_stub.get(), _ctx, reqId, reqType, reqData, func);
    Status status = reader.Await();
    if (status.ok()) {
      return true;
    } else {
      std::cout << "Pull rpc failed. " << status.error_code() << " "
                << status.error_details() << " " << status.error_message()
                << std::endl;
      return false;
    }
  }
  virtual bool Push(std::function<bool(std::string& reqId, std::string& reqType,
                                       std::string& reqData)>
                        func,
                    std::string& result) final override {
    if (!_stub) {
      std::cerr << "Initialization failed or not initialized" << std::endl;
      return false;
    }
    class Recorder : public grpc::ClientWriteReactor<backbonerpc::Request> {
     public:
      Recorder(XgRPC::Stub* stub, std::shared_ptr<ClientCtx>& ctx,
               std::function<bool(std::string& reqId, std::string& reqType,
                                  std::string& reqData)>
                   wfunc)
          : _wfunc(wfunc), _ctx(ctx) {
        stub->async()->Push(&_ctx->_context, &_ctx->_response, this);
        AddHold();
        NextWrite();
        StartCall();
      }
      void OnWriteDone(bool ok) override {
        if (ok) {
          NextWrite();
        } else {
          std::cout << "Push OnWriteDone failed" << std::endl;
          OnDone(Status(StatusCode::DATA_LOSS, "Write failed"));
        }
      }
      void OnDone(const Status& s) override {
        std::unique_lock<std::mutex> l(_ctx->_mtx);
        _status = s;
        _ctx->_done = true;
        _ctx->_cv.notify_one();
      }
      Status Await(backbonerpc::Response* stats) {
        std::cout << "Push Await " << std::endl;
        std::unique_lock<std::mutex> l(_ctx->_mtx);
        _ctx->_cv.wait(l, [this] { return _ctx->_done; });
        *stats = _ctx->_response;
        return std::move(_status);
      }

     private:
      void NextWrite() {
        std::string reqId;
        std::string reqType;
        std::string reqData;
        if (_wfunc && _wfunc(reqId, reqType, reqData)) {
          _ctx->_request.set_id(std::move(reqId));
          _ctx->_request.set_type(std::move(reqType));
          _ctx->_request.set_data(std::move(reqData));
          StartWrite(&_ctx->_request);
        } else {
          StartWritesDone();
          RemoveHold();
        }
      }
      std::function<bool(std::string& reqId, std::string& reqType,
                         std::string& reqData)>
          _wfunc = nullptr;
      /*
      backbonerpc::Request _request;
      backbonerpc::Response _response;

      ClientContext _context;
      std::mutex _mtx;
      std::condition_variable _cv;
      Status _status;
      bool _done = false;
      */
      Status _status;
      std::shared_ptr<ClientCtx> _ctx;
    };

    auto _ctx = std::make_shared<ClientCtx>();
    AddCtx(_ctx);

    Recorder recorder(_stub.get(), _ctx, func);
    backbonerpc::Response stats;
    Status status = recorder.Await(&stats);
    result = stats.data();
    if (status.ok()) {
      return true;
    } else {
      std::cout << "Push rpc failed. " << status.error_code() << " "
                << status.error_details() << " " << status.error_message()
                << std::endl;
      return false;
    }
  }
  virtual bool Bidi(
      std::function<bool(std::string& reqId, std::string& reqType,
                         std::string& reqData)>
          wfunc,
      std::function<bool(const std::string& resId, const std::string& resType,
                         const std::string& resData)>
          rfunc) final override {
    if (!_stub) {
      std::cerr << "Initialization failed or not initialized" << std::endl;
      return false;
    }
    class Chatter : public grpc::ClientBidiReactor<Request, Response> {
     public:
      explicit Chatter(
          XgRPC::Stub* stub, std::shared_ptr<ClientCtx>& ctx,
          std::function<bool(std::string& reqId, std::string& reqType,
                             std::string& reqData)>
              wfunc,
          std::function<bool(const std::string& resId,
                             const std::string& resType,
                             const std::string& resData)>
              rfunc)
          : _wfunc(wfunc), _rfunc(rfunc), _ctx(ctx) {
        stub->async()->Bidi(&_ctx->_context, this);
        NextWrite();
        StartRead(&_ctx->_response);
        StartCall();
      }
      void OnWriteDone(bool ok) override {
        if (ok) {
          NextWrite();
        } else {
          std::cout << "Bidi OnWriteDone failed" << std::endl;
          std::flush(std::cout);
          OnDone(Status(StatusCode::DATA_LOSS, "Write failed"));
        }
      }
      void OnReadDone(bool ok) override {
        if (ok) {
          if (_rfunc && _rfunc(_ctx->_response.id(), _ctx->_response.type(),
                               _ctx->_response.data())) {
            StartRead(&_ctx->_response);
          }
        } else {
          std::cout << "Bidi OnReadDone failed" << std::endl;
          std::flush(std::cout);
          //OnDone(Status(StatusCode::DATA_LOSS,"Read failed"));
        }
      }
      void OnDone(const Status& s) override {
        std::unique_lock<std::mutex> l(_ctx->_mtx);
        _status = s;
        _ctx->_done = true;
        _ctx->_cv.notify_one();
      }
      Status Await() {
        std::unique_lock<std::mutex> l(_ctx->_mtx);
        _ctx->_cv.wait(l, [this] { return _ctx->_done; });
        return std::move(_status);
      }

     private:
      void NextWrite() {
        std::string reqId;
        std::string reqType;
        std::string reqData;
        if (_wfunc && _wfunc(reqId, reqType, reqData)) {
          _ctx->_request.set_id(std::move(reqId));
          _ctx->_request.set_type(std::move(reqType));
          _ctx->_request.set_data(std::move(reqData));
          StartWrite(&_ctx->_request);
        } else {
          StartWritesDone();
        }
      }

      std::function<bool(std::string& reqId, std::string& reqType,
                         std::string& reqData)>
          _wfunc = nullptr;
      std::function<bool(const std::string& resId, const std::string& resType,
                         const std::string& resData)>
          _rfunc = nullptr;

      /*
      Request _request;
      Response _reponse;

      ClientContext _context;
      std::mutex _mtx;
      std::condition_variable _cv;
      Status _status;
      bool _done = false;
      */
      Status _status;
      std::shared_ptr<ClientCtx> _ctx;
    };
    auto _ctx = std::make_shared<ClientCtx>();
    AddCtx(_ctx);

    Chatter chatter(_stub.get(), _ctx, wfunc, rfunc);
    Status status = chatter.Await();
    if (status.ok()) {
      return true;
    } else {
      std::cout << "Bidi rpc failed. " << status.error_code() << " "
                << status.error_details() << " " << status.error_message()
                << std::endl;
      return false;
    }
  }
  virtual bool Subscribe(
      const std::string& reqId, const std::string& reqType,
      const std::string& reqData,
      std::function<bool(const std::string& resId, const std::string& resType,
                         const std::string& resData)>
          func) final override {
    if (!_stub) {
      std::cerr << "Initialization failed or not initialized" << std::endl;
      return false;
    }
    class Reader : public grpc::ClientReadReactor<Response> {
     public:
      Reader(XgRPC::Stub* stub, std::shared_ptr<ClientCtx>& ctx,
             const std::string& reqId, const std::string& reqType,
             const std::string& reqData,
             std::function<bool(const std::string& resId,
                                const std::string& resType,
                                const std::string& resData)>
                 rfunc)
          : _rfunc(rfunc), _ctx(ctx) {
        _ctx->_request.set_id(reqId);
        _ctx->_request.set_type(reqType);
        _ctx->_request.set_data(reqData);
        std::cout << "Client Subscribe Topic:" << reqType << " " << reqData
                  << std::endl;
        stub->async()->Subscribe(&_ctx->_context, &_ctx->_request, this);
        StartRead(&_ctx->_response);
        StartCall();
      }
      void OnReadDone(bool ok) override {
        if (ok) {
          if (_rfunc(_ctx->_response.id(), _ctx->_response.type(),
                     _ctx->_response.data())) {
            StartRead(&_ctx->_response);
          }
        } else {
          std::cout << "Subscribe OnReadDone failed" << std::endl;
          //OnDone(Status(StatusCode::DATA_LOSS,"Read failed"));
        }
      }
      void OnDone(const Status& s) override {
        std::cout << "UnSubscribe Topic:" << _ctx->_request.type() << std::endl;
        std::unique_lock<std::mutex> l(_ctx->_mtx);
        _status = s;
        _ctx->_done = true;
        _ctx->_cv.notify_one();
      }
      Status Await() {
        std::cout << "Subscribe Await " << std::endl;
        std::unique_lock<std::mutex> l(_ctx->_mtx);
        _ctx->_cv.wait(l, [this] { return _ctx->_done; });
        return std::move(_status);
      }

     private:
      std::function<bool(const std::string& resId, const std::string& resType,
                         const std::string& resData)>
          _rfunc = nullptr;
      /*
      backbonerpc::Response _response;
      backbonerpc::Request _request;
      ClientContext _context;
      std::mutex _mtx;
      std::condition_variable _cv;
      Status _status;
      bool _done = false;
      */
      Status _status;
      std::shared_ptr<ClientCtx> _ctx;
    };
    if (reqId == "" || reqType == "") {
      std::cout << "reqId:" << reqId << " reqType:" << reqType << std::endl;
      return false;
    }

    auto _ctx = std::make_shared<ClientCtx>();
    AddCtx(_ctx);

    Reader reader(_stub.get(), _ctx, reqId, reqType, reqData, func);
    Status status = reader.Await();
    if (status.ok()) {
      return true;
    } else {
      std::cout << "Subscribe rpc failed. " << status.error_code() << " "
                << status.error_details() << " " << status.error_message()
                << std::endl;
      return false;
    }
  }

  virtual bool Publish(
      std::function<bool(std::string& reqId, std::string& reqType,
                         std::string& reqData)>
          func,
      std::string& result) final override {
    if (!_stub) {
      std::cerr << "Initialization failed or not initialized" << std::endl;
      return false;
    }
    class Recorder : public grpc::ClientWriteReactor<backbonerpc::Request> {
     public:
      Recorder(XgRPC::Stub* stub, std::shared_ptr<ClientCtx>& ctx,
               std::function<bool(std::string& reqId, std::string& reqType,
                                  std::string& reqData)>
                   wfunc)
          : _wfunc(wfunc), _ctx(ctx) {
        stub->async()->Publish(&_ctx->_context, &_ctx->_response, this);
        AddHold();
        NextWrite();
        StartCall();
      }
      void OnWriteDone(bool ok) override {
        if (ok) {
          NextWrite();
        } else {
          std::cout << "Publish OnWriteDone failed" << std::endl;
          OnDone(Status(StatusCode::DATA_LOSS, "Write failed"));
        }
      }
      void OnDone(const Status& s) override {
        std::unique_lock<std::mutex> l(_ctx->_mtx);
        _status = s;
        _ctx->_done = true;
        _ctx->_cv.notify_one();
      }
      Status Await(backbonerpc::Response* stats) {
        std::cout << "Publish Await " << std::endl;
        std::unique_lock<std::mutex> l(_ctx->_mtx);
        _ctx->_cv.wait(l, [this] { return _ctx->_done; });
        *stats = _ctx->_response;
        return std::move(_status);
      }

     private:
      void NextWrite() {
        std::string reqId;
        std::string reqType;
        std::string reqData;
        if (_wfunc && _wfunc(reqId, reqType, reqData)) {

          _ctx->_request.set_id(std::move(reqId));
          _ctx->_request.set_type(std::move(reqType));
          _ctx->_request.set_data(std::move(reqData));
          StartWrite(&_ctx->_request);
        } else {
          StartWritesDone();
          RemoveHold();
        }
      }
      std::function<bool(std::string& reqId, std::string& reqType,
                         std::string& reqData)>
          _wfunc = nullptr;
      /*
      backbonerpc::Request _request;
      backbonerpc::Response _response;

      ClientContext _context;
      std::mutex _mtx;
      std::condition_variable _cv;
      Status _status;
      bool _done = false;
      */
      Status _status;
      std::shared_ptr<ClientCtx> _ctx;
    };

    auto _ctx = std::make_shared<ClientCtx>();
    AddCtx(_ctx);

    Recorder recorder(_stub.get(), _ctx, func);
    backbonerpc::Response stats;
    Status status = recorder.Await(&stats);
    result = stats.data();
    if (status.ok()) {
      return true;
    } else {
      std::cout << "Publish rpc failed. " << status.error_code() << " "
                << status.error_details() << " " << status.error_message()
                << std::endl;
      return false;
    }
  }
  virtual bool Publish(const std::string& reqId, const std::string& reqType,
                       const std::string& reqData,
                       std::string& result) final override {
    if (!_stub) {
      std::cerr << "Initialization failed or not initialized" << std::endl;
      return false;
    }
    class Recorder : public grpc::ClientWriteReactor<backbonerpc::Request> {
     public:
      Recorder(XgRPC::Stub* stub, std::shared_ptr<ClientCtx>& ctx,
               const std::string& reqId, const std::string& reqType,
               const std::string& reqData)
          : _ctx(ctx) {
        _ctx->_request.set_id(std::move(reqId));
        _ctx->_request.set_type(std::move(reqType));
        _ctx->_request.set_data(std::move(reqData));
        stub->async()->Publish(&_ctx->_context, &_ctx->_response, this);
        AddHold();
        NextWrite();
        StartCall();
      }
      void OnWriteDone(bool ok) override {
        if (ok) {
          OnDone(Status::OK);
        } else {
          std::cout << "Publish OnWriteDone failed" << std::endl;
          OnDone(Status(StatusCode::DATA_LOSS, "Write failed"));
        }
      }
      void OnDone(const Status& s) override {
        std::unique_lock<std::mutex> l(_ctx->_mtx);
        _status = s;
        _ctx->_done = true;
        _ctx->_cv.notify_one();
      }
      Status Await(backbonerpc::Response* stats) {
        std::cout << "Publish Await " << std::endl;
        std::unique_lock<std::mutex> l(_ctx->_mtx);
        _ctx->_cv.wait(l, [this] { return _ctx->_done; });
        *stats = _ctx->_response;
        return std::move(_status);
      }

     private:
      void NextWrite() { StartWrite(&_ctx->_request); }
      /*
      backbonerpc::Request _request;
      backbonerpc::Response _response;

      ClientContext _context;
      std::mutex _mtx;
      std::condition_variable _cv;
      Status _status;
      bool _done = false;
      */
      Status _status;
      std::shared_ptr<ClientCtx> _ctx;
    };
    if (reqId == "" || reqType == "") {
      std::cout << "reqId:" << reqId << " reqType:" << reqType << std::endl;
      return false;
    }
    auto _ctx = std::make_shared<ClientCtx>();
    AddCtx(_ctx);

    Recorder recorder(_stub.get(), _ctx, reqId, reqType, reqData);
    backbonerpc::Response stats;
    Status status = recorder.Await(&stats);
    result = stats.data();
    if (status.ok()) {
      return true;
    } else {
      std::cout << "Publish rpc failed. " << status.error_code() << " "
                << status.error_details() << " " << status.error_message()
                << std::endl;
      return false;
    }
  }

 private:
  std::unique_ptr<XgRPC::Stub> _stub = nullptr;
  std::shared_ptr<Channel> _channel;
  std::vector<std::weak_ptr<ClientCtx>> _ctxs;
  std::mutex _mtx;
};

class DataQueue {
 public:
  DataQueue(int max_size = 20) : _stop(false), _max_size(max_size) {
    _Clear();
  };
  ~DataQueue() {
    Stop();
    _Clear();
  };

  void Stop() {
    if (_stop) {
      return;
    }
    std::lock_guard<std::mutex> guard(_mtx);
    _stop = true;
    _conditionv.notify_all();
  };
  bool Push(const std::string& data) {
    if (_stop) {
      return false;
    }
    std::lock_guard<std::mutex> guard(_mtx);
    while (_queues.size() > _max_size) {
      _queues.pop();
    }
    _queues.emplace(data);
    _conditionv.notify_one();
    return true;
  };
  int Get(std::string& data, bool& quit) {
    data = "";
    if (_stop) {
      return 0;
    }
    std::unique_lock<std::mutex> uniqueLock(_mtx);
    auto hasTimeout = !_conditionv.wait_for(
        uniqueLock, std::chrono::seconds(1),
        [this, &quit]() { return quit || _stop || !_queues.empty(); });
    if (hasTimeout) {
      return 2;
    }
    if (_queues.empty()) {
      return 0;
    }
    data = std::move(_queues.front());
    _queues.pop();
    return 1;
  };

 private:
  void _Clear() {
    while (!_queues.empty()) {
      _queues.pop();
    }
  };
  mutable std::mutex _mtx;
  std::condition_variable _conditionv;
  std::queue<std::string> _queues;
  std::atomic<bool> _stop;
  int _max_size;
};

class TopicDispatcher {
 public:
  TopicDispatcher() { _topic_queues.clear(); }
  ~TopicDispatcher() { _topic_queues.clear(); }
  bool Enroll(const std::string& topic, std::shared_ptr<DataQueue>& dataQueue) {
    std::lock_guard<std::mutex> lck(_mtx);
    auto iter = _topic_queues.find(topic);
    if (iter == _topic_queues.end()) {
      _topic_queues[topic] = std::vector<std::weak_ptr<DataQueue>>{};
    }
    _topic_queues[topic].emplace_back(dataQueue);
    return false;
  }
  bool Dispatcher(const std::string& topic, const std::string& data) {
    std::vector<std::weak_ptr<DataQueue>> tmp;
    {
      std::lock_guard<std::mutex> lck(_mtx);
      auto iter = _topic_queues.find(topic);
      if (iter == _topic_queues.end()) {
        return false;
      }
      _expiredCount = 0;
      std::vector<std::weak_ptr<DataQueue>>& cache = _topic_queues[topic];
      for (auto iter = cache.begin(); iter != cache.end();) {
        if (iter->expired()) {
          _expiredCount++;
          iter++;
        } else {
          tmp.emplace_back((*iter));
          iter++;
        }
      }
      if (_expiredCount > 0) {
        _topic_queues[topic] = tmp;
      }
    }
    for (auto iter = tmp.begin(); iter != tmp.end(); iter++) {
      if (auto item = iter->lock()) {
        item->Push(data);
      }
    }
    return true;
  }

 private:
  std::map<std::string, std::vector<std::weak_ptr<DataQueue>>> _topic_queues;
  std::mutex _mtx;
  int _expiredCount = 0;
};

class ServerImpl final : public Server, public XgRPC::CallbackService {
 public:
  ServerImpl() {}
  bool RegisterCallHandler(
      const std::string& type,
      std::function<bool(const std::string& reqId, const std::string& reqType,
                         const std::string& reqData, std::string& result)>
          func) final override {
    _mtxCallFuncs.Lock();
    auto iter = _callFuncs.find(type);
    if (iter != _callFuncs.end()) {
      _mtxCallFuncs.Unlock();
      return false;
    }
    _callFuncs[type] = func;
    _mtxCallFuncs.Unlock();
    return true;
  }
  bool RegisterPullHandler(
      const std::string& type,
      std::function<bool(const std::string& reqId, const std::string& reqType,
                         const std::string& reqData, std::string& result)>
          func) final override {
    _mtxPullFuncs.Lock();
    auto iter = _pullFuncs.find(type);
    if (iter != _pullFuncs.end()) {
      _mtxPullFuncs.Unlock();
      return false;
    }
    _pullFuncs[type] = func;
    _mtxPullFuncs.Unlock();
    return true;
  }
  bool RegisterPushHandler(
      const std::string& type,
      std::function<bool(const std::string& reqId, const std::string& reqType,
                         const std::string& reqData, std::string& result)>
          func) final override {
    _mtxPushFuncs.Lock();
    auto iter = _pushFuncs.find(type);
    if (iter != _pushFuncs.end()) {
      _mtxPushFuncs.Unlock();
      return false;
    }
    _pushFuncs[type] = func;
    _mtxPushFuncs.Unlock();
    return true;
  }
  bool RegisterBidiHandler(
      const std::string& type,
      std::function<bool(const std::string& reqId, const std::string& reqType,
                         const std::string& reqData, std::string& result)>
          rfunc,
      std::function<bool(const std::string& reqId, const std::string& reqType,
                         const std::string& reqData, std::string& result)>
          wfunc) final override {
    _mtxBidiFuncs.Lock();
    auto iter = _bidiFuncs.find(type);
    if (iter != _bidiFuncs.end()) {
      _mtxBidiFuncs.Unlock();
      return false;
    }
    _bidiFuncs[type] = std::tuple<
        std::function<bool(const std::string& reqId, const std::string& reqType,
                           const std::string& reqData, std::string& result)>,
        std::function<bool(const std::string& reqId, const std::string& reqType,
                           const std::string& reqData, std::string& result)>>(
        rfunc, wfunc);
    _mtxBidiFuncs.Unlock();
    return true;
  }

  grpc::ServerUnaryReactor* Call(
      CallbackServerContext* context, const ::backbonerpc::Request* request,
      ::backbonerpc::Response* response) final override {
    response->set_id(request->id());
    response->set_type(request->type());
    auto* reactor = context->DefaultReactor();
    _mtxCallFuncs.Lock();
    auto iter = _callFuncs.find(request->type());
    if (iter != _callFuncs.end()) {
      auto handler = iter->second;
      _mtxCallFuncs.Unlock();
      if (handler) {
        std::string result;
        if (!handler(request->id(), request->type(), request->data(), result)) {
          reactor->Finish(Status(StatusCode::INTERNAL, result));
        } else {
          response->set_data(result);
          reactor->Finish(Status::OK);
        }
      } else {
        reactor->Finish(
            Status(StatusCode::UNAVAILABLE, "Handler function is null"));
      }
    } else {
      _mtxCallFuncs.Unlock();
      reactor->Finish(
          Status(StatusCode::NOT_FOUND, "Handler function not found"));
    }
    return reactor;
  }

  grpc::ServerWriteReactor<backbonerpc::Response>* Pull(
      CallbackServerContext* context,
      const backbonerpc::Request* request) override {
    class Puller : public grpc::ServerWriteReactor<backbonerpc::Response> {
     public:
      Puller(const backbonerpc::Request* request,
             std::function<
                 bool(const std::string& reqId, const std::string& reqType,
                      const std::string& reqData, std::string& result)>
                 wfunc)
          : _wfunc(wfunc), _request(*request) {
        _response.set_id(_request.id());
        _response.set_type(_request.type());
        NextWrite();
      }
      Puller(grpc::Status status) { Finish(status); }
      void OnDone() override {
        std::cout << "Pull OnDone" << std::endl;
        delete this;
      }
      void OnWriteDone(bool /*ok*/) override {
        std::cout << "Pull OnWriteDone" << std::endl;
        NextWrite();
      }
      void OnCancel() {
        std::cout << "Pull OnCancel" << std::endl;
        _cancel = true;
      }

     private:
      void NextWrite() {
        std::string result = "";
        if (!_cancel &&
            _wfunc(_request.id(), _request.type(), _request.data(), result)) {
          _response.set_data(result);
          StartWrite(&_response);
        } else {
          Finish(Status::OK);
        }
      }

     private:
      std::function<bool(const std::string& reqId, const std::string& reqType,
                         const std::string& reqData, std::string& result)>
          _wfunc = nullptr;
      backbonerpc::Request _request;
      backbonerpc::Response _response;
      int _cancel{false};
    };

    auto Init = [&](const Request* request) -> Puller* {
      _mtxPullFuncs.Lock();
      auto iter = _pullFuncs.find(request->type());
      if (iter != _pullFuncs.end()) {
        auto handler = iter->second;
        _mtxPullFuncs.Unlock();
        if (handler) {
          return new Puller(request, handler);
        } else {
          return new Puller(
              Status(StatusCode::UNAVAILABLE, "Handler function is null"));
        }
      } else {
        _mtxPullFuncs.Unlock();
        return new Puller(
            Status(StatusCode::NOT_FOUND, "Handler function not found"));
      }
    };
    return Init(request);
  }
  grpc::ServerReadReactor<backbonerpc::Request>* Push(
      CallbackServerContext* context,
      backbonerpc::Response* response) final override {
    class Recorder : public grpc::ServerReadReactor<backbonerpc::Request> {
     public:
      Recorder(
          backbonerpc::Response* response, absl::Mutex& mtxPushFuncs,
          std::map<std::string,
                   std::function<bool(
                       const std::string& reqId, const std::string& reqType,
                       const std::string& reqData, std::string& result)>>&
              pushFuncs)
          : _mtxPushFuncsPtr(&mtxPushFuncs),
            _pushFuncsPtr(&pushFuncs),
            _response(response) {
        StartRead(&_request);
      }
      Recorder(grpc::Status status) { Finish(status); }
      void OnDone() override {
        std::cout << "Push OnReadDone" << std::endl;
        delete this;
      }
      void OnReadDone(bool ok) override {
        std::cout << "Push OnReadDone" << std::endl;
        if (!_cancel && ok) {
          if (!_Initialized) {
            Init();
          }
          if (_Initialized) {
            std::string result;
            if (_rfunc(_request.id(), _request.type(), _request.data(),
                       result)) {
              StartRead(&_request);
            } else {
              Finish(Status(StatusCode::INTERNAL, result));
            }
          }
        } else {
          Finish(Status(StatusCode::DATA_LOSS, "Read failed"));
        }
      }
      void OnCancel() {
        std::cout << "Push OnCancel" << std::endl;
        _cancel = true;
      }

     private:
      void Init() {
        _response->set_id(_request.id());
        _response->set_type(_request.type());
        _mtxPushFuncsPtr->Lock();
        auto iter = _pushFuncsPtr->find(_request.type());
        if (iter != _pushFuncsPtr->end()) {
          auto handler = iter->second;
          _mtxPushFuncsPtr->Unlock();
          if (handler) {
            _rfunc = handler;
            _Initialized = true;
          } else {
            Finish(Status(StatusCode::UNAVAILABLE, "Handler function is null"));
          }
        } else {
          _mtxPushFuncsPtr->Unlock();
          Finish(Status(StatusCode::NOT_FOUND, "Handler function not found"));
        }
      }
      absl::Mutex* _mtxPushFuncsPtr = nullptr;
      std::map<std::string,
               std::function<bool(
                   const std::string& reqId, const std::string& reqType,
                   const std::string& reqData, std::string& result)>>*
          _pushFuncsPtr = nullptr;

      backbonerpc::Request _request;
      backbonerpc::Response* _response;
      std::function<bool(const std::string& reqId, const std::string& reqType,
                         const std::string& reqData, std::string& result)>
          _rfunc = nullptr;
      int _cancel{false};
      std::atomic<bool> _Initialized{false};
    };
    return new Recorder(response, _mtxPushFuncs, _pushFuncs);
  }

  grpc::ServerBidiReactor<backbonerpc::Request, backbonerpc::Response>* Bidi(
      CallbackServerContext* context) final override {
    class Chatter : public grpc::ServerBidiReactor<backbonerpc::Request,
                                                   backbonerpc::Response> {
     public:
      Chatter(
          absl::Mutex& mtxBidiFuncs,
          std::map<std::string,
                   std::tuple<
                       std::function<bool(
                           const std::string& reqId, const std::string& reqType,
                           const std::string& reqData, std::string& result)>,
                       std::function<bool(
                           const std::string& reqId, const std::string& reqType,
                           const std::string& reqData, std::string& result)>>>&
              bidiFuncs)
          : _mtxBidiFuncsPtr(&mtxBidiFuncs), _bidiFuncsPtr(&bidiFuncs) {
        StartRead(&_request);
      }
      void OnDone() override {
        std::cout << "Bidi OnDone" << std::endl;
        delete this;
      }
      void OnReadDone(bool ok) override {
        std::cout << "Bidi OnReadDone" << std::endl;
        if (!_cancel && ok) {
          if (!_Initialized) {
            Init();
          }
          if (_Initialized) {
            std::string result;
            if (_rfunc(_request.id(), _request.type(), _request.data(),
                       result)) {
              StartRead(&_request);
            } else {
              Finish(Status(StatusCode::INTERNAL, result));
            }
          }
        } else {
          Finish(Status(StatusCode::DATA_LOSS, "Read failed"));
        }
      }
      void OnWriteDone(bool /*ok*/) override {
        std::cout << "Bidi OnWriteDone" << std::endl;
        NextWrite();
      }
      void OnCancel() {
        std::cout << "Bidi OnCancel" << std::endl;
        _cancel = true;
      }

     private:
      void NextWrite() {
        std::string result = "";
        if (_wfunc(_request.id(), _request.type(), _request.data(), result)) {
          _response.set_data(std::move(result));
          StartWrite(&_response);
        } else {
          Finish(Status::OK);
        }
      }
      void Init() {
        _response.set_id(_request.id());
        _response.set_type(_request.type());
        _mtxBidiFuncsPtr->Lock();
        auto iter = _bidiFuncsPtr->find(_request.type());
        if (iter != _bidiFuncsPtr->end()) {
          auto recv_handler = std::get<0>(iter->second);
          auto send_handler = std::get<1>(iter->second);
          _mtxBidiFuncsPtr->Unlock();
          if (recv_handler && send_handler) {
            _rfunc = recv_handler;
            _wfunc = send_handler;
            _Initialized = true;
          } else {
            Finish(Status(StatusCode::UNAVAILABLE, "Handler function is null"));
          }
        } else {
          _mtxBidiFuncsPtr->Unlock();
          Finish(Status(StatusCode::NOT_FOUND, "Handler function not found"));
        }
      }
      absl::Mutex* _mtxBidiFuncsPtr = nullptr;
      std::map<
          std::string,
          std::tuple<std::function<bool(
                         const std::string& reqId, const std::string& reqType,
                         const std::string& reqData, std::string& result)>,
                     std::function<bool(
                         const std::string& reqId, const std::string& reqType,
                         const std::string& reqData, std::string& result)>>>*
          _bidiFuncsPtr = nullptr;

      std::function<bool(const std::string& reqId, const std::string& reqType,
                         const std::string& reqData, std::string& result)>
          _rfunc = nullptr;
      std::function<bool(const std::string& reqId, const std::string& reqType,
                         const std::string& reqData, std::string& result)>
          _wfunc = nullptr;

      backbonerpc::Request _request;
      backbonerpc::Response _response;
      int _cancel{false};
      std::atomic<bool> _Initialized{false};
    };
    return new Chatter(_mtxBidiFuncs, _bidiFuncs);
  }

  grpc::ServerWriteReactor<backbonerpc::Response>* Subscribe(
      CallbackServerContext* context,
      const backbonerpc::Request* request) final override {
    class Puller : public grpc::ServerWriteReactor<backbonerpc::Response> {
     public:
      Puller(const backbonerpc::Request* request,
             std::shared_ptr<DataQueue>& dataQueue)
          : _request(*request), _dataQueue(dataQueue), _cancel(false) {
        std::cout << "Subscribe Topic:" << _request.type() << std::endl;
        _response.set_id(_request.id());
        _response.set_type(_request.type());
        NextWrite();
      }
      void OnDone() override {
        std::cout << "UnSubscribe Topic:" << _response.type() << std::endl;
        _cancel = true;
        _dataQueue.reset();
        delete this;
      }
      void OnWriteDone(bool /*ok*/) override {
        std::cout << "Subscribe OnWriteDone" << std::endl;
        NextWrite();
      }
      void OnCancel() {
        std::cout << "Subscribe OnCancel" << std::endl;
        _cancel = true;
      }

     private:
      void NextWrite() {
        std::string result = "";
        auto res = _dataQueue->Get(result, _cancel);
        if (result != "") {
          _response.set_data(std::move(result));
          StartWrite(&_response);
        } else {
          //timeout
          if (res == 2) {
            std::thread([this]() {
              std::this_thread::sleep_for(std::chrono::milliseconds(200));
              NextWrite();
            }).detach();
          } else {
            Finish(Status::OK);
          }
        }
      }

     private:
      bool _cancel = false;
      backbonerpc::Request _request;
      backbonerpc::Response _response;
      std::shared_ptr<DataQueue> _dataQueue;
    };

    auto Init = [&](const backbonerpc::Request* request) -> Puller* {
      auto dataQueue = std::make_shared<DataQueue>();
      _topicDispatcher.Enroll(request->type(), dataQueue);
      return new Puller(request, dataQueue);
    };
    return Init(request);
  }
  grpc::ServerReadReactor<backbonerpc::Request>* Publish(
      CallbackServerContext* context,
      backbonerpc::Response* response) final override {
    class Recorder : public grpc::ServerReadReactor<backbonerpc::Request> {
     public:
      Recorder(backbonerpc::Response* response,
               TopicDispatcher& topicDispatcher)
          : _topicDispatcherPtr(&topicDispatcher), _response(response) {
        StartRead(&_request);
      }
      Recorder(grpc::Status status) { Finish(status); }
      void OnDone() override {
        std::cout << "Publish OnDone" << std::endl;
        delete this;
      }
      void OnReadDone(bool ok) override {
        std::cout << "Publish OnReadDone" << std::endl;
        if (ok) {
          if (!_Initialized) {
            Init();
          }
          if (_Initialized) {
            _topicDispatcherPtr->Dispatcher(_request.type(), _request.data());
            StartRead(&_request);
          }
        } else {
          Finish(Status(StatusCode::DATA_LOSS, "Read failed"));
        }
      }

     private:
      void Init() {
        _response->set_id(_request.id());
        _response->set_type(_request.type());
        _Initialized = true;
        std::cout << "Publish Topic:" << _request.type() << std::endl;
      }
      TopicDispatcher* _topicDispatcherPtr = nullptr;
      backbonerpc::Request _request;
      backbonerpc::Response* _response;
      std::atomic<bool> _Initialized{false};
    };
    return new Recorder(response, _topicDispatcher);
  }

  bool Subscribe(
      const std::string& reqId, const std::string& reqType,
      std::function<bool(const std::string& resId, const std::string& resType,
                         const std::string& resData)>
          func) final override {
    auto dataQueue = std::make_shared<DataQueue>();
    _topicDispatcher.Enroll(reqType, dataQueue);

    bool cancel = false;
    while (true) {
      std::string result = "";
      auto res = dataQueue->Get(result, cancel);
      if (result != "") {
        cancel = func(reqId, reqType, result);
      } else {
        //timeout
        if (res == 2) {
          std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
      }
    }
    return true;
  }
  bool Publish(const std::string& reqType,
               const std::string& reqData) final override {
    return _topicDispatcher.Dispatcher(reqType, reqData);
  }

 private:
  absl::Mutex _mtxCallFuncs;
  std::map<
      std::string,
      std::function<bool(const std::string& reqId, const std::string& reqType,
                         const std::string& reqData, std::string& result)>>
      _callFuncs;

  absl::Mutex _mtxPullFuncs;
  std::map<
      std::string,
      std::function<bool(const std::string& reqId, const std::string& reqType,
                         const std::string& reqData, std::string& result)>>
      _pullFuncs;

  absl::Mutex _mtxPushFuncs;
  std::map<
      std::string,
      std::function<bool(const std::string& reqId, const std::string& reqType,
                         const std::string& reqData, std::string& result)>>
      _pushFuncs;

  absl::Mutex _mtxBidiFuncs;
  std::map<std::string,
           std::tuple<std::function<bool(
                          const std::string& reqId, const std::string& reqType,
                          const std::string& reqData, std::string& result)>,
                      std::function<bool(
                          const std::string& reqId, const std::string& reqType,
                          const std::string& reqData, std::string& result)>>>
      _bidiFuncs;

  TopicDispatcher _topicDispatcher;
};

class ServerRunImpl : public ServerRun {
 public:
  ServerRunImpl(){};
  virtual ~ServerRunImpl(){};
  virtual bool Install(Server* ser) {
    try {
      auto srv = dynamic_cast<grpc::Service*>(ser);
      _builder.RegisterService(srv);
      return true;
    } catch (std::exception& e) {
      std::cout << e.what() << std::endl;
      return false;
    }
  };

  virtual bool Run(const std::string& server_address) {
    _builder.AddListeningPort(server_address,
                              grpc::InsecureServerCredentials());
    _builder.SetMaxSendMessageSize(1024 * 1024 * 100);
    _builder.SetMaxReceiveMessageSize(1024 * 1024 * 100);
    _builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_TIME_MS, 10 * 60 * 1000);
    _builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, 20 * 1000);
    _builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS, 1);
    _builder.AddChannelArgument(
        GRPC_ARG_HTTP2_MIN_RECV_PING_INTERVAL_WITHOUT_DATA_MS, 10 * 1000);

    try {
      auto server = _builder.BuildAndStart();
      // std::unique_ptr<grpc::Server> server(_builder.BuildAndStart());
      if (server) {
        std::cout << "Server listening on " << server_address << std::endl;
        server->Wait();
        return true;
      } else {
        std::cerr << "Server is null [" << server_address << "]" << std::endl;
        return false;
      }
    } catch (std::exception& e) {
      std::cerr << e.what() << std::endl;
    }
  };

 private:
  grpc::ServerBuilder _builder;
};
//////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Client> newClient() {
  return std::make_shared<ClientImpl>();
}

std::shared_ptr<Server> newServer() {
  return std::make_shared<ServerImpl>();
}

std::shared_ptr<ServerRun> newServerRun() {
  return std::make_shared<ServerRunImpl>();
}
}  // namespace BackboneGRPC

#ifdef __cplusplus
extern "C" {
#endif

std::string identity(void) {
  return XGRPC_IDENTITY;
}
std::string version(void) {
  return XGRPC_VERSION;
}
std::string description(void) {
  return XGRPC_DESCRIPTION;
}
void* create(const std::string& cfg) {
  if (cfg == "Client") {
    return new BackboneGRPC::ClientImpl();
  } else if (cfg == "Server") {
    return new BackboneGRPC::ServerImpl();
  } else if (cfg == "ServerRun") {
    return new BackboneGRPC::ServerRunImpl();
  }
  return nullptr;
}
void destroy(void* p) {
  if (p) {
    try {
      std::string type_name = typeid(p).name();
      if (type_name == typeid(BackboneGRPC::ClientImpl).name()) {
        auto tp = reinterpret_cast<BackboneGRPC::ClientImpl*>(p);
        delete tp;
      } else if (type_name == typeid(BackboneGRPC::ServerImpl).name()) {
        auto tp = reinterpret_cast<BackboneGRPC::ServerImpl*>(p);
        delete tp;
      } else if (type_name == typeid(BackboneGRPC::ServerRunImpl).name()) {
        auto tp = reinterpret_cast<BackboneGRPC::ServerRunImpl*>(p);
        delete tp;
      } else {
        std::cerr << "Unsupported type " << std::endl;
        return;
      }
    } catch (std::bad_typeid const& e) {
      std::cerr << "Destroy error:" << e.what() << std::endl;
    } catch (std::bad_cast const& e) {
      std::cerr << "Destroy error:" << e.what() << std::endl;
    }
  }
}
#ifdef __cplusplus
}
#endif
