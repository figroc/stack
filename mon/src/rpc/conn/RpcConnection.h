#ifndef MSVC_RPC_RPC_CONNECTION_H
#define MSVC_RPC_RPC_CONNECTION_H

#include "../../sock/DataBuffer.h"
#include "../message/RpcRequest.h"
#include "../message/RpcResponse.h"
#include "../parser/Parser.h"
#include <string>
#include <memory>
#include <boost/signals2.hpp>

namespace msvc { namespace rpc { namespace _di {

using namespace msvc::sock;

class RpcConnection {
public:
	boost::signals2::signal<void(RpcConnection &, std::auto_ptr<RpcConnection>)> OnConnected;
	boost::signals2::signal<void(RpcConnection &, std::auto_ptr<RpcRequest>)> OnRequestRecieved;
	boost::signals2::signal<void(RpcConnection &, std::auto_ptr<RpcResponse>)> OnResponseReceived;
	boost::signals2::signal<void(RpcConnection &)> OnDisconnected;

public:
	const std::string &localEP() const;
	const std::string &remoteEP() const;

public:
	void Listen(const std::string &local);
	void Connect(const std::string &remote);
	void BeginRecieve();
	void Send(const DataBuffer &data);
};

}}}

#endif
