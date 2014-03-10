#ifndef MSVC_CACHE_REDIS_CONN_POOL_H
#define MSVC_CACHE_REDIS_CONN_POOL_H

#include "RedisUri.h"
#include "RedisHelper.h"
#include "../../util/CachePerfC.h"
#include <list>
#include <map>
#include <memory>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>

namespace msvc { namespace cache {

class RedisConnPool : public boost::enable_shared_from_this<RedisConnPool> {
private:
	typedef std::list<redis_conn_ptr> redis_conn_list_type;
	typedef boost::shared_ptr<redis_conn_list_type> redis_conn_list_ptr;
	typedef std::pair<redis_conn_list_ptr, PerfCounter *> redis_conn_info;
	typedef std::map<std::string, redis_conn_info> redis_conn_map;

public:
	class ScopedConnection {
		friend class RedisConnPool;
	private:
		boost::shared_ptr<RedisConnPool> _pool;
		RedisUri _uri;
		redis_conn_ptr _conn;
		PerfCounter *_perf;

	private:
		ScopedConnection(const boost::shared_ptr<RedisConnPool> &pool, const RedisUri &uri,
				const redis_conn_ptr &conn, PerfCounter *const perf)
			: _pool(pool), _uri(uri), _conn(conn), _perf(perf) {
			CachePerfC::Pool::used()->Increment();
		};
		ScopedConnection(const ScopedConnection &conn);
		ScopedConnection &operator=(const ScopedConnection &conn);
	public:
		~ScopedConnection() {
			_pool->Release(_uri, _conn);
			CachePerfC::Pool::used()->Decrement();
		};

	public:
		inline const redis_conn_ptr &ptr() const { return _conn; };

		inline redisContext &operator*() { return *_conn; };
		inline redisContext *operator->() { return _conn.get(); };

		inline PerfCounter *perf() const { return _perf; };
	};

private:
	static const int MAX_CONN_IN_POOL = 300;

public:
	inline static boost::shared_ptr<RedisConnPool> Create() {
		return boost::shared_ptr<RedisConnPool>(new RedisConnPool());
	};

private:
	inline static void _FreeRedisContext(redis_conn_ptr::element_type *context) {
		if (context) redisFree(context);
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
