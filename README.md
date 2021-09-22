# grpc4p (grpc++PlusPromise)

this repository is intended to offer a wrapper around grpc async feature with an API like the Javascript Promises.


    auto  promise  =  asyncGrpc.call(&helloworld::Greeter::Stub::PrepareAsyncSayHello)(_req,call);
    
    promise->then([](std::any  data){
        std::cerr  <<  "call success! \n";
	}).then([](std::any  data){
	    try
	    {
	        const  auto  _data  =  std::any_cast<ASyncCall<AsyncCallData<helloworld::HelloReply>> *>(data);
	        std::cerr  <<  _data->_data.reply.message() <<  '\n';
	    }
	    catch (const  std::bad_any_cast&  e)
	    {
	        std::cerr  <<  e.what() <<  '\n';
	    }
	}).error([](){
	    std::cerr  <<  "call failed! \n";
	});
