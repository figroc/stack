#ifndef MSVC_RPC_RPC_STATUS_H
#define MSVC_RPC_RPC_STATUS_H

namespace msvc { namespace rpc {

class RpcStatus {
public:
	static const int Continue = 100;
	static const int OK = 200;
	static const int Bad_Request = 400;
	static const int Server_Error = 500;
	static const int Global_Failure = 600;

private:
	RpcStatus();
};

}}

#endif
