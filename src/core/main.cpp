
#include <iostream>
#include <vector>

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>
#include "../grpc/services.grpc.pb.h"
#include "scheduler.h"
#include "calldata.h"
using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerAsyncReaderWriter;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerCompletionQueue;
using grpc::Status;
using services::GameService;
using services::ServiceMsg;


class ServerImpl final  {
public:
    ~ServerImpl() {
        server_->Shutdown();
        // Always shutdown the completion queue after the server.
        cq_->Shutdown();
    }

    // There is no shutdown handling in this code.
    void Run(int poolsize ,std::string server_address) {

        ServerBuilder builder;
        // Listen on the given address without any authentication mechanism.
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        // Register "service_" as the instance through which we'll communicate with
        // clients. In this case it corresponds to an *asynchronous* service.
        builder.RegisterService(&service_);
        // Get hold of the completion queue used for the asynchronous communication
        // with the gRPC runtime.
        cq_ = builder.AddCompletionQueue();
        // Finally assemble the server.
        server_ = builder.BuildAndStart();
        LogInfo("robot Server listening on:%s", server_address.c_str());

        Scheduler::getInstance()->init(poolsize);

        //server's main loop.
        handleRpcs();

        Scheduler::getInstance()->jionAllThreads();
  }

private:
    // This can be run in multiple threads if needed.
    void handleRpcs() {
      // Spawn a new CallData instance to serve new clients.
      new CallData(&service_, cq_.get());
      void* tag;  // uniquely identifies a request.
      bool ok;
      while (true) {
          // Block waiting to read the next event from the completion queue. The
          // event is uniquely identified by its tag, which in this case is the
          // memory address of a CallData instance.
          cq_->Next(&tag, &ok);
          if (!ok && tag !=NULL) {
            static_cast<CallData*>(tag)->setstatus(CallData::CallStatus::FINISH);
            static_cast<CallData*>(tag)->Proceed();
            continue;
          }
          if (ok && tag !=NULL) {
             static_cast<CallData*>(tag)->Proceed();
          }
      }
  }

private:
    std::unique_ptr<ServerCompletionQueue> cq_;
    GameService::AsyncService service_;
    std::unique_ptr<Server> server_;
    std::vector<LuaRobot*> vec_luarobot_;
};

int main(int argc, char** argv) {
    if (argc != 3)
    {
      std::cerr << "Usage: ./robot thread-num port\n";
      return 1;
    }
    std::string port=argv[2];
    std::string addr="0.0.0.0:";
    ServerImpl server;
    server.Run(atoi(argv[1]),addr+port);
    return 0;
}
