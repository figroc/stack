#ifndef MSVC_CACHE_REDIS_CONN_POOL_H
#define MSVC_CACHE_REDIS_CONN_POOL_H

#include "RedisUri.h"
#include "RedisHelper.h"
#include <list>
#include <map>
#include <memory>
#include <boost/thread.hpp>

namespace msvc { namespace cache {

class RedisConnPool : public boost::enable_shared_from_this<RedisConnPool> {
private:
	typedef std::list<redis_conn_ptr> redis_conn_list;
	typedef std::map<std::string, redis_conn_list> redis_conn_map;

public:
	class ScopedConnection {
		friend class RedisConnPool;
	private:
		boost::shared_ptr<RedisConnPool> _pool;
		RedisUri _uri;
		redis_conn_ptr _conn;

	private:
		ScopedConnection(const boost::shared_ptr<RedisConnPool> &pool, const RedisUri &uri,
				const redis_conn_ptr &conn)	: _pool(pool), _uri(uri), _conn(conn) { };
		ScopedConnection(const ScopedConnection &conn);
		ScopedConnection &operator=(const ScopedConnection &conn);
	public:
		~ScopedConnection() {
			if (!(_conn->err))
				_pool->Release(_uri, _conn);
		};

	public:
		inline const redis_conn_ptr &ptr() const { return _conn; };

		inline redisContext &operator*() { return *_conn; };
		inline redisContext *operator->() { return _conn.get(); };
	};

private:
	static const int MAX_CONN_IN_POOL = 100;

public:
	inline static boost::shared_ptr<RedisConnPool> Create() {
		return boost::shared_ptr<RedisConnPool>(new RedisConnPool());
	};

private:
	boost::mutex _lock;
	redis_conn_map _conns;

private:
	RedisConnPool() : _lock(), _conns() {  };
	RedisConnPool(const RedisConnPool &pool);
	RedisConnPool &operator=(const RedisConnPool &pool);

public:
	std::auto_ptr<ScopedConnection> Fetch(const RedisUri &uri);

private:
	void Release(const RedisUri &uri, const redis_conn_ptr &conn);
};

}}

#endif
