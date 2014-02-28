#ifndef MSVC_RPC_RPC_TRANSACTION_H
#define MSVC_RPC_RPC_TRANSACTION_H

#include "../message/RpcMessage.h"
#include "../message/RpcRequest.h"
#include "../message/RpcResponse.h"

namespace msvc { namespace rpc {

class RpcTransaction {
public:
	const std::string &id() const;

	const RpcRequest &request() const;

	RpcResponse &response();
	const RpcResponse &response() const;

public:
	void SendRequest();
	void SendResponse();
};

}}

#endif
