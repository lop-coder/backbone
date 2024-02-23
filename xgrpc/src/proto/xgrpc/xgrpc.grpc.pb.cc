// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: xgrpc.proto

#include "xgrpc.pb.h"
#include "xgrpc.grpc.pb.h"

#include <functional>
#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/impl/channel_interface.h>
#include <grpcpp/impl/client_unary_call.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/support/message_allocator.h>
#include <grpcpp/support/method_handler.h>
#include <grpcpp/impl/rpc_service_method.h>
#include <grpcpp/support/server_callback.h>
#include <grpcpp/impl/server_callback_handlers.h>
#include <grpcpp/server_context.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/support/sync_stream.h>
namespace backbonerpc {

static const char* XgRPC_method_names[] = {
  "/backbonerpc.XgRPC/Call",
  "/backbonerpc.XgRPC/Pull",
  "/backbonerpc.XgRPC/Push",
  "/backbonerpc.XgRPC/Bidi",
  "/backbonerpc.XgRPC/Subscribe",
  "/backbonerpc.XgRPC/Publish",
};

std::unique_ptr< XgRPC::Stub> XgRPC::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< XgRPC::Stub> stub(new XgRPC::Stub(channel, options));
  return stub;
}

XgRPC::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_Call_(XgRPC_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Pull_(XgRPC_method_names[1], options.suffix_for_stats(),::grpc::internal::RpcMethod::SERVER_STREAMING, channel)
  , rpcmethod_Push_(XgRPC_method_names[2], options.suffix_for_stats(),::grpc::internal::RpcMethod::CLIENT_STREAMING, channel)
  , rpcmethod_Bidi_(XgRPC_method_names[3], options.suffix_for_stats(),::grpc::internal::RpcMethod::BIDI_STREAMING, channel)
  , rpcmethod_Subscribe_(XgRPC_method_names[4], options.suffix_for_stats(),::grpc::internal::RpcMethod::SERVER_STREAMING, channel)
  , rpcmethod_Publish_(XgRPC_method_names[5], options.suffix_for_stats(),::grpc::internal::RpcMethod::CLIENT_STREAMING, channel)
  {}

::grpc::Status XgRPC::Stub::Call(::grpc::ClientContext* context, const ::backbonerpc::Request& request, ::backbonerpc::Response* response) {
  return ::grpc::internal::BlockingUnaryCall< ::backbonerpc::Request, ::backbonerpc::Response, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_Call_, context, request, response);
}

void XgRPC::Stub::async::Call(::grpc::ClientContext* context, const ::backbonerpc::Request* request, ::backbonerpc::Response* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::backbonerpc::Request, ::backbonerpc::Response, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Call_, context, request, response, std::move(f));
}

void XgRPC::Stub::async::Call(::grpc::ClientContext* context, const ::backbonerpc::Request* request, ::backbonerpc::Response* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Call_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::backbonerpc::Response>* XgRPC::Stub::PrepareAsyncCallRaw(::grpc::ClientContext* context, const ::backbonerpc::Request& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::backbonerpc::Response, ::backbonerpc::Request, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_Call_, context, request);
}

::grpc::ClientAsyncResponseReader< ::backbonerpc::Response>* XgRPC::Stub::AsyncCallRaw(::grpc::ClientContext* context, const ::backbonerpc::Request& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncCallRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::ClientReader< ::backbonerpc::Response>* XgRPC::Stub::PullRaw(::grpc::ClientContext* context, const ::backbonerpc::Request& request) {
  return ::grpc::internal::ClientReaderFactory< ::backbonerpc::Response>::Create(channel_.get(), rpcmethod_Pull_, context, request);
}

void XgRPC::Stub::async::Pull(::grpc::ClientContext* context, const ::backbonerpc::Request* request, ::grpc::ClientReadReactor< ::backbonerpc::Response>* reactor) {
  ::grpc::internal::ClientCallbackReaderFactory< ::backbonerpc::Response>::Create(stub_->channel_.get(), stub_->rpcmethod_Pull_, context, request, reactor);
}

::grpc::ClientAsyncReader< ::backbonerpc::Response>* XgRPC::Stub::AsyncPullRaw(::grpc::ClientContext* context, const ::backbonerpc::Request& request, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::backbonerpc::Response>::Create(channel_.get(), cq, rpcmethod_Pull_, context, request, true, tag);
}

::grpc::ClientAsyncReader< ::backbonerpc::Response>* XgRPC::Stub::PrepareAsyncPullRaw(::grpc::ClientContext* context, const ::backbonerpc::Request& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::backbonerpc::Response>::Create(channel_.get(), cq, rpcmethod_Pull_, context, request, false, nullptr);
}

::grpc::ClientWriter< ::backbonerpc::Request>* XgRPC::Stub::PushRaw(::grpc::ClientContext* context, ::backbonerpc::Response* response) {
  return ::grpc::internal::ClientWriterFactory< ::backbonerpc::Request>::Create(channel_.get(), rpcmethod_Push_, context, response);
}

void XgRPC::Stub::async::Push(::grpc::ClientContext* context, ::backbonerpc::Response* response, ::grpc::ClientWriteReactor< ::backbonerpc::Request>* reactor) {
  ::grpc::internal::ClientCallbackWriterFactory< ::backbonerpc::Request>::Create(stub_->channel_.get(), stub_->rpcmethod_Push_, context, response, reactor);
}

::grpc::ClientAsyncWriter< ::backbonerpc::Request>* XgRPC::Stub::AsyncPushRaw(::grpc::ClientContext* context, ::backbonerpc::Response* response, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncWriterFactory< ::backbonerpc::Request>::Create(channel_.get(), cq, rpcmethod_Push_, context, response, true, tag);
}

::grpc::ClientAsyncWriter< ::backbonerpc::Request>* XgRPC::Stub::PrepareAsyncPushRaw(::grpc::ClientContext* context, ::backbonerpc::Response* response, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncWriterFactory< ::backbonerpc::Request>::Create(channel_.get(), cq, rpcmethod_Push_, context, response, false, nullptr);
}

::grpc::ClientReaderWriter< ::backbonerpc::Request, ::backbonerpc::Response>* XgRPC::Stub::BidiRaw(::grpc::ClientContext* context) {
  return ::grpc::internal::ClientReaderWriterFactory< ::backbonerpc::Request, ::backbonerpc::Response>::Create(channel_.get(), rpcmethod_Bidi_, context);
}

void XgRPC::Stub::async::Bidi(::grpc::ClientContext* context, ::grpc::ClientBidiReactor< ::backbonerpc::Request,::backbonerpc::Response>* reactor) {
  ::grpc::internal::ClientCallbackReaderWriterFactory< ::backbonerpc::Request,::backbonerpc::Response>::Create(stub_->channel_.get(), stub_->rpcmethod_Bidi_, context, reactor);
}

::grpc::ClientAsyncReaderWriter< ::backbonerpc::Request, ::backbonerpc::Response>* XgRPC::Stub::AsyncBidiRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncReaderWriterFactory< ::backbonerpc::Request, ::backbonerpc::Response>::Create(channel_.get(), cq, rpcmethod_Bidi_, context, true, tag);
}

::grpc::ClientAsyncReaderWriter< ::backbonerpc::Request, ::backbonerpc::Response>* XgRPC::Stub::PrepareAsyncBidiRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncReaderWriterFactory< ::backbonerpc::Request, ::backbonerpc::Response>::Create(channel_.get(), cq, rpcmethod_Bidi_, context, false, nullptr);
}

::grpc::ClientReader< ::backbonerpc::Response>* XgRPC::Stub::SubscribeRaw(::grpc::ClientContext* context, const ::backbonerpc::Request& request) {
  return ::grpc::internal::ClientReaderFactory< ::backbonerpc::Response>::Create(channel_.get(), rpcmethod_Subscribe_, context, request);
}

void XgRPC::Stub::async::Subscribe(::grpc::ClientContext* context, const ::backbonerpc::Request* request, ::grpc::ClientReadReactor< ::backbonerpc::Response>* reactor) {
  ::grpc::internal::ClientCallbackReaderFactory< ::backbonerpc::Response>::Create(stub_->channel_.get(), stub_->rpcmethod_Subscribe_, context, request, reactor);
}

::grpc::ClientAsyncReader< ::backbonerpc::Response>* XgRPC::Stub::AsyncSubscribeRaw(::grpc::ClientContext* context, const ::backbonerpc::Request& request, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::backbonerpc::Response>::Create(channel_.get(), cq, rpcmethod_Subscribe_, context, request, true, tag);
}

::grpc::ClientAsyncReader< ::backbonerpc::Response>* XgRPC::Stub::PrepareAsyncSubscribeRaw(::grpc::ClientContext* context, const ::backbonerpc::Request& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::backbonerpc::Response>::Create(channel_.get(), cq, rpcmethod_Subscribe_, context, request, false, nullptr);
}

::grpc::ClientWriter< ::backbonerpc::Request>* XgRPC::Stub::PublishRaw(::grpc::ClientContext* context, ::backbonerpc::Response* response) {
  return ::grpc::internal::ClientWriterFactory< ::backbonerpc::Request>::Create(channel_.get(), rpcmethod_Publish_, context, response);
}

void XgRPC::Stub::async::Publish(::grpc::ClientContext* context, ::backbonerpc::Response* response, ::grpc::ClientWriteReactor< ::backbonerpc::Request>* reactor) {
  ::grpc::internal::ClientCallbackWriterFactory< ::backbonerpc::Request>::Create(stub_->channel_.get(), stub_->rpcmethod_Publish_, context, response, reactor);
}

::grpc::ClientAsyncWriter< ::backbonerpc::Request>* XgRPC::Stub::AsyncPublishRaw(::grpc::ClientContext* context, ::backbonerpc::Response* response, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncWriterFactory< ::backbonerpc::Request>::Create(channel_.get(), cq, rpcmethod_Publish_, context, response, true, tag);
}

::grpc::ClientAsyncWriter< ::backbonerpc::Request>* XgRPC::Stub::PrepareAsyncPublishRaw(::grpc::ClientContext* context, ::backbonerpc::Response* response, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncWriterFactory< ::backbonerpc::Request>::Create(channel_.get(), cq, rpcmethod_Publish_, context, response, false, nullptr);
}

XgRPC::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      XgRPC_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< XgRPC::Service, ::backbonerpc::Request, ::backbonerpc::Response, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](XgRPC::Service* service,
             ::grpc::ServerContext* ctx,
             const ::backbonerpc::Request* req,
             ::backbonerpc::Response* resp) {
               return service->Call(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      XgRPC_method_names[1],
      ::grpc::internal::RpcMethod::SERVER_STREAMING,
      new ::grpc::internal::ServerStreamingHandler< XgRPC::Service, ::backbonerpc::Request, ::backbonerpc::Response>(
          [](XgRPC::Service* service,
             ::grpc::ServerContext* ctx,
             const ::backbonerpc::Request* req,
             ::grpc::ServerWriter<::backbonerpc::Response>* writer) {
               return service->Pull(ctx, req, writer);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      XgRPC_method_names[2],
      ::grpc::internal::RpcMethod::CLIENT_STREAMING,
      new ::grpc::internal::ClientStreamingHandler< XgRPC::Service, ::backbonerpc::Request, ::backbonerpc::Response>(
          [](XgRPC::Service* service,
             ::grpc::ServerContext* ctx,
             ::grpc::ServerReader<::backbonerpc::Request>* reader,
             ::backbonerpc::Response* resp) {
               return service->Push(ctx, reader, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      XgRPC_method_names[3],
      ::grpc::internal::RpcMethod::BIDI_STREAMING,
      new ::grpc::internal::BidiStreamingHandler< XgRPC::Service, ::backbonerpc::Request, ::backbonerpc::Response>(
          [](XgRPC::Service* service,
             ::grpc::ServerContext* ctx,
             ::grpc::ServerReaderWriter<::backbonerpc::Response,
             ::backbonerpc::Request>* stream) {
               return service->Bidi(ctx, stream);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      XgRPC_method_names[4],
      ::grpc::internal::RpcMethod::SERVER_STREAMING,
      new ::grpc::internal::ServerStreamingHandler< XgRPC::Service, ::backbonerpc::Request, ::backbonerpc::Response>(
          [](XgRPC::Service* service,
             ::grpc::ServerContext* ctx,
             const ::backbonerpc::Request* req,
             ::grpc::ServerWriter<::backbonerpc::Response>* writer) {
               return service->Subscribe(ctx, req, writer);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      XgRPC_method_names[5],
      ::grpc::internal::RpcMethod::CLIENT_STREAMING,
      new ::grpc::internal::ClientStreamingHandler< XgRPC::Service, ::backbonerpc::Request, ::backbonerpc::Response>(
          [](XgRPC::Service* service,
             ::grpc::ServerContext* ctx,
             ::grpc::ServerReader<::backbonerpc::Request>* reader,
             ::backbonerpc::Response* resp) {
               return service->Publish(ctx, reader, resp);
             }, this)));
}

XgRPC::Service::~Service() {
}

::grpc::Status XgRPC::Service::Call(::grpc::ServerContext* context, const ::backbonerpc::Request* request, ::backbonerpc::Response* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status XgRPC::Service::Pull(::grpc::ServerContext* context, const ::backbonerpc::Request* request, ::grpc::ServerWriter< ::backbonerpc::Response>* writer) {
  (void) context;
  (void) request;
  (void) writer;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status XgRPC::Service::Push(::grpc::ServerContext* context, ::grpc::ServerReader< ::backbonerpc::Request>* reader, ::backbonerpc::Response* response) {
  (void) context;
  (void) reader;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status XgRPC::Service::Bidi(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::backbonerpc::Response, ::backbonerpc::Request>* stream) {
  (void) context;
  (void) stream;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status XgRPC::Service::Subscribe(::grpc::ServerContext* context, const ::backbonerpc::Request* request, ::grpc::ServerWriter< ::backbonerpc::Response>* writer) {
  (void) context;
  (void) request;
  (void) writer;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status XgRPC::Service::Publish(::grpc::ServerContext* context, ::grpc::ServerReader< ::backbonerpc::Request>* reader, ::backbonerpc::Response* response) {
  (void) context;
  (void) reader;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace backbonerpc

