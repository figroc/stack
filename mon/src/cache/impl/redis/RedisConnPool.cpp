#include "RedisConnPool.h"
#include <stdexcept>

namespace msvc { namespace cache {

using namespace std;

auto_ptr<RedisConnPool::ScopedConnection> RedisConnPool::Fetch(const RedisUri &uri)
{
	auto_ptr<ScopedConnection> conn;
	{
		boost::mutex::scoped_lock lock(_lock);
		redis_conn_map::iterator it = _conns.find(uri.userAtDb());
		if (it != _conns.end()) {
			while (!it->second.empty()) {
				redis_conn_ptr ptr = it->second.front();
				it->second.pop_front();
				if (ptr && !(ptr->err)) {
					conn.reset(new ScopedConnection(shared_from_this(), uri, it->second.front()));
					break;
				}
			}
		}
	}
	if (!conn.get()) {
		redis_conn_ptr ptr(redisConnect(uri.addr().c_str(), uri.port()), &redisFree);
		if (!ptr)
			throw runtime_error(string("failed to connect to ") + uri.uri());

		RedisHelper::Call(ptr, "AUTH", uri.pass());
		RedisHelper::Call(ptr, "SELECT", uri.name());

		redis_reply_ptr rpl = RedisHelper::Reply(ptr);
		if (!rpl || rpl->type != REDIS_REPLY_STATUS)
			throw runtime_error(string("failed to auth to ") + uri.uri());
		rpl = RedisHelper::Reply(ptr);
		if (!rpl || rpl->type != REDIS_REPLY_STATUS)
			throw runtime_error(string("failed to switch to ") + uri.uri());

		conn.reset(new ScopedConnection(shared_from_this(), uri, ptr));
	}
	return conn;
}

void RedisConnPool::Release(const RedisUri &uri, const redis_conn_ptr &conn)
{
	if (conn && !(conn->err)) {
		boost::mutex::scoped_lock lock(_lock);
		redis_conn_map::iterator it = _conns.find(uri.userAtDb());
		if (it != _conns.end()){
			while (it->second.size() >= MAX_CONN_IN_POOL)
				it->second.pop_back();
			it->second.push_front(conn);
		}
	}
}

}}
