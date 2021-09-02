#ifndef GRPC_PLUS_PROMISE_ASYNC_CALL
#define GRPC_PLUS_PROMISE_ASYNC_CALL

#include <grpcPlusPromise/IAsyncCall.h>

#include <grpc++/grpc++.h>

#include <memory>

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;


struct AsyncCallDataBase
{
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // Storage for the status of the RPC upon completion.
    Status status;
};

// struct for keeping state and data information
template <typename REPLAY__>
struct AsyncCallData: public AsyncCallDataBase
{
    // Container for the data we expect from the server.
    REPLAY__ reply;

    std::unique_ptr<ClientAsyncResponseReader<REPLAY__>> response_reader;
};

template <typename AYSNC_DATA__>
class ASyncCall : public IAsyncCall
{
    template<typename> friend class GreeterClient;

public:

    ASyncCall():IAsyncCall(_data)
    {

    }
    virtual void done() override
    {

    };

public: //TODO: make this private
//private:
    AYSNC_DATA__ _data;
};

#endif // GRPC_PLUS_PROMISE_ASYNC_CALL