#include "MySqlDbConnPool.h"
#include "../../util/DbPerfC.h"
#include <utility>

namespace msvc { namespace db {

using namespace std;

auto_ptr<MySqlDbConnPool::ScopedConnection> MySqlDbConnPool::Fetch(const MySqlDbUri &uri)
{
	auto_ptr<ScopedConnection> conn;
	sql_conn_map::iterator it;
	{
		boost::mutex::scoped_lock lock(_lock);
		it = _conns.find(uri.userAtDb());
		if (it == _conns.end()) {
			it = _conns.insert(make_pair(uri.userAtDb(),
									sql_conn_entry(
											sql_conn_list_ptr(new sql_conn_list_type()),
											sql_conn_perf(
													DbPerfC::Fetch(uri.name(), 'r'),
													DbPerfC::Fetch(uri.name(), 'w')
											)
									))
				).first;
		} else {
			while (!it->second.first->empty()) {
				sql_conn_info info = it->second.first->front();
				it->second.first->pop_front();
				DbPerfC::Pool::free()->Decrement();
				if (!(info.first->isClosed())) {
					conn.reset(new ScopedConnection(shared_from_this(), uri, info, it->second.second));
					break;
				}
			}
		}
	}

	if (!conn.get()) {
		conn.reset(new ScopedConnection(shared_from_this(), uri,
				sql_conn_info(
						sql_conn_ptr(_driver->connect(uri.host(), uri.user(), uri.pass())),
						sql_stmt_map_ptr(new sql_stmt_map_type())
				), it->second.second));
		(*conn)->setSchema(uri.name());
	}
	return conn;
}

void MySqlDbConnPool::Release(const MySqlDbUri &uri, const sql_conn_info &conn)
{
	if (conn.first && !(conn.first->isClosed())) {
		boost::mutex::scoped_lock lock(_lock);
		sql_conn_map::iterator it = _conns.find(uri.userAtDb());
		if (it != _conns.end()) {
			while (it->second.first->size() >= MAX_CONN_IN_POOL) {
				it->second.first->pop_back();
				DbPerfC::Pool::free()->Decrement();
			}
			it->second.first->push_front(conn);
			DbPerfC::Pool::free()->Increment();
		}
	}
}

}}
