// Class encompasing the state and logic needed to serve a request.
#ifndef CALL_DATA
#define CALL_DATA
#include <iostream>
#include <vector>
#include <chrono>
#include "../net/platform.h"

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>
#include "../grpc/services.grpc.pb.h"
#include <boost/noncopyable.hpp>
#include "scheduler.h"

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerAsyncReaderWriter;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerCompletionQueue;
using grpc::Status;
using services::GameService;
using services::ServiceMsg;

class CallData :private boost::noncopyable {
public:
     enum CallStatus {
         CREATE     = 1,
         READ       = 2,
         PROCESS    = 3,
         WRITE      = 4,
         FINISH     = 5 };
public:
  // Take in the "service" instance (in this case representing an asynchronous
  // server) and the completion queue "cq" used for asynchronous communication
  // with the gRPC runtime.
  explicit CallData(GameService::AsyncService* service, ServerCompletionQueue* cq)
      :service_(service), cq_(cq),stream_(&ctx_),status_(CREATE) {
          // Invoke the serving logic right away.
          Proceed();
  }
  ServiceMsg& getmsg(){return requestmsg_;}
  int getstatus(){return status_;}
  void setstatus(CallStatus status){ status_=status;}
  void setResult(::google::protobuf::uint32 protoid,std::string &val){
      responsemsg_.set_protoid(protoid);
      responsemsg_.clear_payload();
      responsemsg_.set_payload(val);
  }
  void Proceed() {
    if (status_ == CREATE) {
      // As part of the initial CREATE state, we *request* that the system
      // start processing SayHello requests. In this request, "this" acts are
      // the tag uniquely identifying the request (so that different CallData
      // instances can serve different requests concurrently), in this case
      // the memory address of this CallData instance.
      LogDebug("enter status CREATE ");
      service_->RequestStream(&ctx_, &stream_, cq_, cq_,this);
      status_ = READ;
  } else if (status_ == READ) {
      // Spawn a new CallData instance to serve new clients while we process
      // the one for this CallData. The instance will deallocate itself as
      // part of its FINISH state.
      LogDebug("enter status READ ");
      new CallData(service_, cq_);
      stream_.Read(&requestmsg_,this);
      status_ = PROCESS;
      LogDebug("recieve battle request!battleid:%d ",requestmsg_.logid());
  }else if (status_ == PROCESS) {
      // The actual processing.
      // And we are done! Let the gRPC runtime know we've finished, using the
      // memory address of this instance as the uniquely identifying tag for
      // the event.
     LogDebug("enter status PROCESS ");
     Scheduler::getInstance()->dispatch((void*)this);
} else if (status_ == WRITE) {
        LogDebug("enter status WRITE ,send response to client");
        stream_.Write(responsemsg_, this);
        status_ = FINISH;
}else {
      LogDebug("enter status FINISH , delete grpc CallData");
      GPR_ASSERT(status_ == FINISH);
      // Once in the FINISH state, deallocate ourselves (CallData).
      delete this;
    }
  }

 private:
  // The means of communication with the gRPC runtime for an asynchronous
  // server.
  GameService::AsyncService* service_;
  // The producer-consumer queue where for asynchronous server notifications.
  ServerCompletionQueue* cq_;

  // Context for the rpc, allowing to tweak aspects of it such as the use
  // of compression, authentication, as well as to send metadata back to the
  // client.
  ServerContext ctx_;

  // What we get from the client.
 ServerAsyncReaderWriter<ServiceMsg,ServiceMsg> stream_;

 ServiceMsg requestmsg_;
 ServiceMsg responsemsg_;

  // Let's implement a tiny state machine with the following states.
  CallStatus status_;  // The current serving state.
};
#endif
