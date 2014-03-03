#ifndef DB_MONGODB_CONNPOOL_H
#define DB_MONGODB_CONNPOOL_H

#include <string>
#include <map>
#include <memory>

#include <boost/smart_ptr.hpp>
#include <boost/atomic.hpp>
#include <boost/thread/mutex.hpp>
#include <mongo/client/dbclient.h>

#include "MongoDbUri.h"
#include "../../util/DbPerfC.h"

namespace msvc { namespace db {

using namespace msvc::perf;

class MongoDbConnPool {
private:
	class Authenticator : public mongo::DBConnectionHook {
	private:
		MongoDbUri _uri;
		boost::atomic_int _con;

	public:
		Authenticator(const MongoDbUri &uri) : _uri(uri), _con(0) { }

	public:
		inline int con() const { return _con.load(boost::memory_order_relaxed); }
		virtual void onCreate(mongo::DBClientBase *const conn);
		virtual void onDestroy(mongo::DBClientBase *const conn);
	};

	class PoolForUser {
	private:
		MongoDbUri _uri;
		mongo::ConnectionString _host;
		Authenticator *_auth;
		mongo::DBConnectionPool *_pool;
		PerfCounter *_perfR;
		PerfCounter *_perfW;

	public:
		PoolForUser(const MongoDbUri &uri);
		virtual ~PoolForUser();
	private:
		PoolForUser(const PoolForUser &pool);
		PoolForUser &operator=(const PoolForUser &pool);

	public:
		inline const MongoDbUri &uri() const { return _uri; }
		inline mongo::DBClientBase *get() {
			mongo::DBClientBase *conn = _pool->get(_host);
			if (!conn) {
				throw std::runtime_error(std::string("failed to get conn for: ") += _uri.userAtDb());
			}
			DbPerfC::Pool::free()->Decrement();
			DbPerfC::Pool::used()->Increment();
			return conn;
		}
		inline void rel(mongo::DBClientBase *conn) {
			if (conn) {
				_pool->release(_uri.host(), conn);
				DbPerfC::Pool::used()->Decrement();
				DbPerfC::Pool::free()->Increment();
			}
		}
		inline PerfCounter *perfR() const { return _perfR; }
		inline PerfCounter *perfW() const { return _perfW; }
	};

public:
	class ScopedClient {
		friend class MongoDbConnPool;
	private:
		boost::shared_ptr<PoolForUser> _pool;
		mongo::DBClientBase *_client;

	private:
		ScopedClient(const boost::shared_ptr<PoolForUser> &pool) : _pool(pool) {
			_client = pool->get();
		}
		ScopedClient(const ScopedClient &client);
		ScopedClient &operator=(const ScopedClient &client);
	public:
		~ScopedClient() { _pool->rel(_client); }

	public:
		inline mongo::DBClientBase &operator*() { return *_client; }
		inline mongo::DBClientBase *operator->() { return _client; }
		inline PerfCounter *perfR() const { return _pool->perfR(); }
		inline PerfCounter *perfW() const { return _pool->perfW(); }
	};

private:
	boost::mutex _lock;
	std::map< std::string, boost::shared_ptr<PoolForUser> > _pool;

public:
	MongoDbConnPool() { }
	virtual ~MongoDbConnPool() { }
private:
	MongoDbConnPool(const MongoDbConnPool &) { }
	MongoDbConnPool &operator=(const MongoDbConnPool &) { return *this; }

public:
	std::auto_ptr<ScopedClient> Fetch(const MongoDbUri &uri);
};

}}

#endif /* DB_MONGODB_CONNPOOL_H */
