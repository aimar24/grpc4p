#ifndef GRPC_PLUS_PROMISE_IASYNC_CALL
#define GRPC_PLUS_PROMISE_IASYNC_CALL

class IAsyncCall
{
    public:
    void done();

    virtual ~IAsyncCall() = default;
};

#endif //GRPC_PLUS_PROMISE_IASYNC_CALL