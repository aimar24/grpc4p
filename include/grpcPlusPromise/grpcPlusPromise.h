#ifndef GRPC_PLUS_PROMISE
#define GRPC_PLUS_PROMISE

#include <grpcPlusPromise/AsyncCall.h>
#include <Promise/Promise.h>
#include <any>
#include <grpc++/grpc++.h>

#include <chrono>

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;

template<typename STUB__>
class GreeterClient {
public:
    explicit GreeterClient(std::shared_ptr<Channel> channel)
        : stub_(STUB__::NewStub(channel)) {

        std::chrono::time_point deadline = std::chrono::system_clock::now() +
                std::chrono::milliseconds(1000000);

        channel->WaitForConnected(deadline);
    }

    // Assembles the client's payload and sends it to the server.
    template<typename REQUEST__ ,typename ASYNC_CALL__ , typename DATA__,
             std::unique_ptr< ::grpc::ClientAsyncResponseReader<DATA__>>
             (STUB__::Stub::*CALL_FUNC__)(::grpc::ClientContext* context,const REQUEST__ &request, ::grpc::CompletionQueue* cq)>
    std::unique_ptr<Promise::PromiseBase> call(REQUEST__ &request , ASYNC_CALL__ *call) {

        std::unique_ptr<Promise::PromiseBase> p = std::make_unique<Promise::PromiseBase>([&](Promise::PromiseBase::Handle& then,Promise::PromiseBase::Handle& catchHndle){
                call->_onStatusChanged.push_back([&](){
            std::any a = call;
            if(call->_data.status.ok()){
                then(a);
            }
            else
            {
                catchHndle(a);
            }
        });
    });

        //call._promise = std::move(p);

        // Data we are sending to the server.
        //HelloRequest request;
        //request.set_name(user);

        // Call object to store rpc data
        //AsyncClientCall* call = new AsyncClientCall;

        // stub_->PrepareAsyncSayHello() creates an RPC object, returning
        // an instance to store in "call" but does not actually start the RPC
        // Because we are using the asynchronous API, we need to hold on to
        // the "call" instance in order to get updates on the ongoing RPC.
        call->_data.response_reader =
                (stub_.get()->*CALL_FUNC__)(&call->_data.context, request, &cq_);

        // StartCall initiates the RPC call
        call->_data.response_reader->StartCall();

        // Request that, upon completion of the RPC, "reply" be updated with the
        // server's response; "status" with the indication of whether the operation
        // was successful. Tag the request with the memory address of the call object.
        call->_data.response_reader->Finish(&call->_data.reply, &call->_data.status, (void*)call);

        return p;

    }

    // Loop while listening for completed responses.
    // Prints out the response from the server.
    void AsyncCompleteRpc() {
        void* got_tag;
        bool ok = false;

        // Block until the next result is available in the completion queue "cq".
        while (cq_.Next(&got_tag, &ok)) {
            // The tag in this example is the memory location of the call object
            IAsyncCall* call = static_cast<IAsyncCall*>(got_tag);

            // Verify that the request was completed successfully. Note that "ok"
            // corresponds solely to the request for updates introduced by Finish().
            GPR_ASSERT(ok);

            if (call->_data.status.ok()){
                std::cout << "Greeter received: " <<  std::endl;
                call->done();

            }
            //            else if(call->_data.status.error_code() == grpc::StatusCode::ABORTED)
            //            {

            //            }
            else
                std::cout << "RPC failed" << std::endl;

            // Once we're complete, deallocate the call object.
            delete call;
        }
    }

private:

    

    // Out of the passed in Channel comes the stub, stored here, our view of the
    // server's exposed services.
    std::unique_ptr<typename STUB__::Stub> stub_;

    // The producer-consumer queue we use to communicate asynchronously with the
    // gRPC runtime.
    CompletionQueue cq_;
};

#endif //GRPC_PLUS_PROMISE
