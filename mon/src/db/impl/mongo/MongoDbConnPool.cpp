#include <stdexcept>

#include "MongoDbConnPool.h"

namespace msvc { namespace db {

using namespace std;
using namespace boost;
using namespace mongo;

void MongoDbConnPool::Authenticator::onCreate(DBClientBase *const conn)
{
	if (conn) {
		string error;
		if (!conn->auth(_uri.name(), _uri.user(), _uri.pass(), error, true))
			throw runtime_error(string("failed to authenticate for: ")
					.append(_uri.userAtDb()).append(" !").append(error));
		_con.fetch_add(1, memory_order_relaxed);
		DbPerfC::Pool::free()->Increment();
	}
}

void MongoDbConnPool::Authenticator::onDestroy(DBClientBase *const conn)
{
	_con.fetch_sub(1, memory_order_relaxed);
	DbPerfC::Pool::free()->Decrement();
}

MongoDbConnPool::PoolForUser::PoolForUser(const MongoDbUri &uri) : _uri(uri)
{
	string error;
	_host = ConnectionString::parse(uri.host(), error);
	if (!_host.isValid()) {
		throw logic_error(string("invalid mongodb connection string: ")
				.append(uri.userAtDb()).append(" !").append(error));
	}
	_perfR = DbPerfC::Fetch(uri.name(), 'r');
	_perfW = DbPerfC::Fetch(uri.name(), 'w');
	_auth = new MongoDbConnPool::Authenticator(uri);
	_pool = new DBConnectionPool();
	_pool->addHook(_auth);
}

MongoDbConnPool::PoolForUser::~PoolForUser()
{
	if (_pool)			// delete _pool first
		delete _pool;
	if (_auth) {		// then _auth
		DbPerfC::Pool::free()->Decrement(_auth->con());
		delete _auth;
	}
}

auto_ptr<MongoDbConnPool::ScopedClient> MongoDbConnPool::Fetch(const MongoDbUri &uri)
{
	shared_ptr<PoolForUser> pool;
	{
		boost::mutex::scoped_lock lock(_lock);

		map< string, shared_ptr<PoolForUser> >::iterator
			it = _pool.find(uri.userAtDb());
		if (_pool.end() != it && uri.pass() != it->second->uri().pass()) {
			_pool.erase(it);
			it = _pool.end();
		}
		if (_pool.end() == it) {
			pool.reset(new PoolForUser(uri));
			_pool.insert(std::make_pair(uri.userAtDb(), pool));
		} else {
			pool = it->second;
		}
	}
	if (!pool) {
		throw runtime_error(string("failed to create conn pool for: ") += uri.userAtDb());
	}

	return auto_ptr<ScopedClient>(new ScopedClient(pool));
}

}}
