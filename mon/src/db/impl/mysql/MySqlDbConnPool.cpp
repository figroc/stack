#include "MySqlDbConnPool.h"
#include <utility>

namespace msvc { namespace db {

using namespace std;

auto_ptr<MySqlDbConnPool::ScopedConnection> MySqlDbConnPool::Fetch(const MySqlDbUri &uri)
{
	auto_ptr<ScopedConnection> conn;
	{
		boost::mutex::scoped_lock lock(_lock);
		sql_conn_map::iterator it = _conns.find(uri.userAtDb());
		if (it != _conns.end()) {
			conn.reset(new ScopedConnection(shared_from_this(), uri, it->second.front()));
			it->second.pop_front();
		}
	}
	if (!conn.get()) {
		conn.reset(new ScopedConnection(shared_from_this(), uri, _driver.connect(uri.host(), uri.user(), uri.pass())));
		(*conn)->setSchema(uri.name());
	}
	return conn;
}

void MySqlDbConnPool::Release(const MySqlDbUri &uri, const sql_conn_ptr conn)
{
	if (conn && !conn->isClosed()) {
		boost::mutex::scoped_lock lock(_lock);
		sql_conn_map::iterator it = _conns.find(uri.userAtDb());
		if (it == _conns.end()) {
			it = _conns.insert(make_pair(uri.userAtDb(), sql_conn_list())).first;
		}
		it->second.push_back(conn);
	}
}

}}
