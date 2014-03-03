#ifndef DB_MYSQL_DB_CONN_POOL_H
#define DB_MYSQL_DB_CONN_POOL_H

#include "MySqlDbUri.h"
#include <string>
#include <list>
#include <map>
#include <memory>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>
#include <mysql_driver.h>
#include <mysql_connection.h>

namespace msvc { namespace db {

class MySqlDbConnPool : public boost::enable_shared_from_this<MySqlDbConnPool> {
private:
	typedef sql::Connection *sql_conn_ptr;
	typedef std::list<sql_conn_ptr> sql_conn_list;
	typedef std::map<std::string, sql_conn_list> sql_conn_map;

public:
	class ScopedConnection {
		friend class MySqlDbConnPool;
	private:
		boost::shared_ptr<MySqlDbConnPool> _pool;
		MySqlDbUri _uri;
		sql_conn_ptr _conn;

	private:
		ScopedConnection(
				const boost::shared_ptr<MySqlDbConnPool> &pool,
				const MySqlDbUri &uri,
				sql::Connection *conn)
			: _pool(pool), _uri(uri), _conn(conn) { };
		ScopedConnection(const ScopedConnection &conn);
		ScopedConnection &operator=(const ScopedConnection &conn);
	public:
		~ScopedConnection() {
			if (_conn->isClosed()) delete _conn; else _pool->Release(_uri, _conn);
		};

	public:
		inline sql::Connection &operator*() { return *_conn; };
		inline sql::Connection *operator->() { return _conn; };
	};

public:
	inline static boost::shared_ptr<MySqlDbConnPool> Create() {
		return boost::shared_ptr<MySqlDbConnPool>(new MySqlDbConnPool());
	};

private:
	boost::mutex _lock;
	sql::mysql::MySQL_Driver _driver;
	sql_conn_map _conns;

private:
	MySqlDbConnPool() : _lock(), _driver(), _conns() { };
	MySqlDbConnPool(const MySqlDbConnPool &pool);
	MySqlDbConnPool &operator=(const MySqlDbConnPool &pool);

public:
	std::auto_ptr<ScopedConnection> Fetch(const MySqlDbUri &uri);

private:
	void Release(const MySqlDbUri &uri, const sql_conn_ptr conn);
};

}}

#endif
