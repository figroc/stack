#ifndef MSVC_RPC_RPC_RESPONSE_H
#define MSVC_RPC_RPC_RESPONSE_H

#include "RpcMessage.h"

namespace msvc { namespace rpc {

class RpcResponse : public RpcMessage {
private:
    int _status;
    
public:
    RpcResponse(): RpcMessage(), _status() { };
    
public:
	int &status();
	const int &status() const;
};

}}

#endif
