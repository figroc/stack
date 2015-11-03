#ifndef MSVC_RPC_RPC_REQUEST_H
#define MSVC_RPC_RPC_REQUEST_H

#include "RpcMessage.h"

namespace msvc { namespace rpc {

class RpcRequest : public RpcMessage {
private:
    std::string _uri;
    
public:
    RpcRequest() : RpcMessage(), _uri() { };
    
public:
	std::string &uri();
	const std::string &uri() const;
    
};

}}

#endif
