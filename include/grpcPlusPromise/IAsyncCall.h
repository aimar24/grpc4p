#ifndef GRPC_PLUS_PROMISE_IASYNC_CALL
#define GRPC_PLUS_PROMISE_IASYNC_CALL

#include <functional>
#include <any>
#include <vector>

namespace Promise
{
    class PromiseBase;
}

class AsyncCallDataBase;
class IAsyncCall
{
    public:
    IAsyncCall(const AsyncCallDataBase& data):_data(data){}

    virtual void done() 
    {
        for(const auto& c : _onStatusChanged){
            c();
        }
        internalDone();
    };

    virtual void internalDone() = 0;

    const AsyncCallDataBase& _data;

    virtual ~IAsyncCall() = default;

    std::vector<std::function<void()>> _onStatusChanged;

    //std::function<void(std::function<void(std::any)> , std::function<void(std::any)>>

    //std::unique_ptr <Promise::PromiseBase> _promise;

};

#endif //GRPC_PLUS_PROMISE_IASYNC_CALL