#include "ConnectionManager.h"
#include <utility>

namespace msvc { namespace rpc { namespace _di {

using namespace std;

boost::shared_ptr<RpcConnection> ConnectionManager::GetOne(const string &remote)
{
	boost::mutex::scoped_lock lock(_lock);
	map< string, boost::shared_ptr<RpcConnection> >::iterator
	it = _conns.find(remote);
	if (it == _conns.end()) {
		boost::shared_ptr<RpcConnection> conn(new RpcConnection());
		conn->Connect(remote);
		it = _conns.insert(make_pair(remote, conn)).second;
	}
	return it->second;
}

void ConnectionManager::OnConnectionClosed(RpcConnection &conn)
{
	boost::mutex::scoped_lock lock(_lock);
	_conns.erase(conn.remoteEP());
}

}}}
