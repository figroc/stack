#ifndef MSVC_RPC_CONNECTION_GROUP_H
#define MSVC_RPC_CONNECTION_GROUP_H

#include "RpcConnection.h"
#include <string>
#include <map>
#include <memory>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/signals2.hpp>

namespace msvc { namespace rpc { namespace _di {

class ConnectionManager {
private:
	boost::mutex _lock;
	std::map< std::string, boost::shared_ptr<RpcConnection> > _conns;

public:
	boost::signals2::signal<void(RpcConnection &, std::auto_ptr<RpcRequest>)> OnRequestRecieved;
	boost::signals2::signal<void(RpcConnection &, std::auto_ptr<RpcResponse>)> OnResponseReceived;

public:
	ConnectionManager() : _lock(), _conns() { };

public:
	boost::shared_ptr<RpcConnection> GetOne(const std::string &remote);

private:
	void OnConnectionClosed(RpcConnection &);
};

}}}

#endif
