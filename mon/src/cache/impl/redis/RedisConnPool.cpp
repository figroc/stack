#include "RedisConnPool.h"
#include <stdexcept>

namespace msvc { namespace cache {

using namespace std;

auto_ptr<RedisConnPool::ScopedConnection> RedisConnPool::Fetch(const RedisUri &uri)
{
	auto_ptr<ScopedConnection> conn;
	redis_conn_map::iterator it;
	{
		boost::mutex::scoped_lock lock(_lock);
		it = _conns.find(uri.userAtDb());
		if (it == _conns.end()) {
			it = _conns.insert(make_pair(uri.userAtDb(),
					redis_conn_info(redis_conn_list_ptr(new redis_conn_list_type()),
									CachePerfC::Fetch(uri.host(), uri.name()))
					)).first;
		} else {
			while (!it->second.first->empty()) {
				redis_conn_ptr ptr = it->second.first->front();
				it->second.first->pop_front();
				CachePerfC::Pool::free()->Decrement();
				if (!(ptr->err)) {
					conn.reset(new ScopedConnection(shared_from_this(), uri,
							it->second.first->front(), it->second.second));
					break;
				}
			}
		}
	}

	if (!conn.get()) {
		redis_conn_ptr ptr(redisConnect(uri.addr().c_str(), uri.port()), &_FreeRedisContext);
		if (!ptr)
			throw runtime_error(string("failed to connect to ") + uri.uri());

		if (!uri.pass().empty())
			RedisHelper::Call(ptr, "AUTH", uri.pass());
		RedisHelper::Call(ptr, "SELECT", uri.name());

		redis_reply_ptr rpl;
		if (!uri.pass().empty()) {
			rpl = RedisHelper::Reply(ptr);
			if (!rpl || rpl->type != REDIS_REPLY_STATUS)
				throw runtime_error(string("failed to auth to ") + uri.uri());
		}
		rpl = RedisHelper::Reply(ptr);
		if (!rpl || rpl->type != REDIS_REPLY_STATUS)
			throw runtime_error(string("failed to switch to ") + uri.uri());

		conn.reset(new ScopedConnection(shared_from_this(), uri, ptr, it->second.second));
	}
	return conn;
}

void RedisConnPool::Release(const RedisUri &uri, const redis_conn_ptr &conn)
{
	if (conn && !(conn->err)) {
		boost::mutex::scoped_lock lock(_lock);
		redis_conn_map::iterator it = _conns.find(uri.userAtDb());
		if (it != _conns.end()){
			while (it->second.first->size() >= MAX_CONN_IN_POOL) {
				it->second.first->pop_back();
				CachePerfC::Pool::free()->Decrement();
			}
			it->second.first->push_front(conn);
			CachePerfC::Pool::free()->Increment();
		}
	}
}

}}
