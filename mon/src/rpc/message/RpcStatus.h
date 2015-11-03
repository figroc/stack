#ifndef MSVC_RPC_RPC_STATUS_H
#define MSVC_RPC_RPC_STATUS_H

namespace msvc { namespace rpc {

class RpcStatus {
public:
	static const int OK = 200;
	static const int ClientError = 400;
	static const int ServerError = 500;
	static const int GlobalError = 600;
    
public:
    inline static bool IsOK(int status) {
        return status / 100 == 2;
    };
    inline static bool IsError(int status) {
        return status / 100 != 2;
    };
    inline static bool IsClientError(int status) {
        return status / 100 == 4;
    };
    inline static bool IsServerError(int status) {
        return status / 100 == 5;
    };
    inline static bool IsGlobalError(int status) {
        return status / 100 == 6;
    };

private:
	RpcStatus();
};

}}

#endif
