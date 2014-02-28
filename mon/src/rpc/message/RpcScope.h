#ifndef MSVC_RPC_RPC_SCOPE_H
#define MSVC_RPC_RPC_SCOPE_H

#include <string>

namespace msvc { namespace rpc {

class RpcScope {
public:
	static const std::string Auth;
	static const std::string Profile;
	static const std::string Roster;
	static const std::string Group;
	static const std::string Message;
	static const std::string Presence;

private:
	RpcScope();
};

}}

#endif
