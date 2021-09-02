#ifndef GRPC_PLUS_PROMISE_IASYNC_CALL
#define GRPC_PLUS_PROMISE_IASYNC_CALL

#include <functional>

class AsyncCallDataBase;
class IAsyncCall
{
    public:
    IAsyncCall(const AsyncCallDataBase& data):_data(data){}
    virtual void done() = 0;

    const AsyncCallDataBase& _data;

    virtual ~IAsyncCall() = default;

    std::function<void(const IAsyncCall* const)> _onStatusChanged;

};

#endif //GRPC_PLUS_PROMISE_IASYNC_CALL