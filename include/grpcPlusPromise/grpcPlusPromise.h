#ifndef GRPC_PLUS_PROMISE
#define GRPC_PLUS_PROMISE

#include <grpcPlusPromise/AsyncCall.h>
#include <Promise/Promise.h>
#include <grpc++/grpc++.h>

#include <any>
#include <chrono>
#include <type_traits>

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;


template<typename T>
struct function_traits;

template<typename T , typename R, typename ...Args>
struct function_traits<R (T::*)(Args...)>
{
    static const size_t nargs = sizeof...(Args);

    typedef R result_type;

    template <size_t i>
    struct arg
    {
        typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
    };
};

template<typename STUB__>
class GrcpPlusPromise {
public:
    explicit GrcpPlusPromise(std::shared_ptr<Channel> channel)
        : stub_(STUB__::NewStub(channel)) {

        std::chrono::time_point deadline = std::chrono::system_clock::now() +
                std::chrono::milliseconds(1000000);

        channel->WaitForConnected(deadline);
    }


    template <typename D , typename R>
    using functionCall = std::unique_ptr< ::grpc::ClientAsyncResponseReader<D>>
    (STUB__::Stub::*)(::grpc::ClientContext* context,const R &request, ::grpc::CompletionQueue* cq);

    // Assembles the client's payload and sends it to the server.
    template<typename REQUEST__ ,typename ASYNC_CALL__ , typename DATA__>
    std::unique_ptr<Promise::PromiseBase> callInternal(functionCall<DATA__,REQUEST__> CALL_FUNC__,REQUEST__ &request , ASYNC_CALL__ *call) {

        std::unique_ptr<Promise::PromiseBase> p = std::make_unique<Promise::PromiseBase>([& , call](Promise::PromiseBase::Handle& then,Promise::PromiseBase::Handle& catchHndle){
                call->_onStatusChanged.push_back([&, call](){
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

        // stub_->PrepareAsync*() creates an RPC object, returning
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

    template<typename T>
    struct FunctionWrapper;

    template<typename T , typename R, typename ...Args>
    struct FunctionWrapper<R (T::*)(Args...)>
    {

        public:
        using _REQUEST = typename std::remove_reference<typename std::remove_const<typename function_traits<R (T::*)(Args...)>::template arg<1>::type>::type>::type ;
        using _REPLY = typename std::remove_pointer<typename function_traits<decltype(&R::element_type::Finish)>::template arg<0>::type>::type;
        using _ASYNC_CALL = ASyncCall<AsyncCallData<_REPLY>>;
        using FType = R (T::*)(Args...);

        FunctionWrapper(GrcpPlusPromise<STUB__> *t, FType f):_t(t), _f(f)
        {

        }

        std::unique_ptr<Promise::PromiseBase> operator ()(_REQUEST & r, _ASYNC_CALL *a){
            return _t->GrcpPlusPromise<STUB__>::callInternal<_REQUEST,_ASYNC_CALL,_REPLY>(_f,r,a);
        };

        private:
        GrcpPlusPromise<STUB__> *_t = nullptr;
        FType _f;



    };



    template<typename R, typename ...Args>
    constexpr auto call(R (STUB__::Stub::*fun) (Args...))
    {
        using _REQUEST = typename std::remove_reference<typename std::remove_const<typename function_traits<decltype(fun)>::template arg<1>::type>::type>::type ;
        using _REPLY = typename R::element_type;
        using _ASYNC_CALL = ASyncCall<AsyncCallData<_REPLY>>;

        return FunctionWrapper<R (STUB__::Stub::*) (Args...)>(this, fun);
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
