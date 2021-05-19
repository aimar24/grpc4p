#ifndef GRPC_PLUS_PROMISE
#define GRPC_PLUS_PROMISE

#include <AsyncCall.h>

#include <grpc++/grpc++.h>

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;

template<typename STUB__>
class GreeterClient {
  public:
    explicit GreeterClient(std::shared_ptr<Channel> channel)
            : stub_(STUB__::NewStub(channel)) {}

    // Assembles the client's payload and sends it to the server.
    template<typename ASYNC_CALL__ , typename CALL_FUNC__>
    void call(const ASYNC_CALL__ &call) {
        // Data we are sending to the server.
        //HelloRequest request;
        //request.set_name(user);

        // Call object to store rpc data
        //AsyncClientCall* call = new AsyncClientCall;

        // stub_->PrepareAsyncSayHello() creates an RPC object, returning
        // an instance to store in "call" but does not actually start the RPC
        // Because we are using the asynchronous API, we need to hold on to
        // the "call" instance in order to get updates on the ongoing RPC.
        call->response_reader =
            stub_->CALL_FUNC__(&call._data.context, request, &cq_);

        // StartCall initiates the RPC call
        call->response_reader->StartCall();

        // Request that, upon completion of the RPC, "reply" be updated with the
        // server's response; "status" with the indication of whether the operation
        // was successful. Tag the request with the memory address of the call object.
        call->response_reader->Finish(&call->reply, &call->status, (void*)call);

    }

    // Loop while listening for completed responses.
    // Prints out the response from the server.
    void AsyncCompleteRpc() {
        void* got_tag;
        bool ok = false;

        // Block until the next result is available in the completion queue "cq".
        while (cq_.Next(&got_tag, &ok)) {
            // The tag in this example is the memory location of the call object
            AsyncClientCall* call = static_cast<AsyncClientCall*>(got_tag);

            // Verify that the request was completed successfully. Note that "ok"
            // corresponds solely to the request for updates introduced by Finish().
            GPR_ASSERT(ok);

            if (call->status.ok())
                std::cout << "Greeter received: " << call->reply.message() << std::endl;
            else
                std::cout << "RPC failed" << std::endl;

            // Once we're complete, deallocate the call object.
            delete call;
        }
    }

  private:

    

    // Out of the passed in Channel comes the stub, stored here, our view of the
    // server's exposed services.
    std::unique_ptr<STUB__::Stub> stub_;

    // The producer-consumer queue we use to communicate asynchronously with the
    // gRPC runtime.
    CompletionQueue cq_;
};

#endif //GRPC_PLUS_PROMISE