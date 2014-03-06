#ifndef DB_MYSQL_DB_CONN_POOL_H
#define DB_MYSQL_DB_CONN_POOL_H

#include "MySqlDbUri.h"
#include "../../util/DbPerfC.h"
#include <string>
#include <list>
#include <map>
#include <memory>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>

namespace msvc { namespace db {

using namespace msvc::perf;

class MySqlDbConnPool : public boost::enable_shared_from_this<MySqlDbConnPool> {
private:
	typedef boost::shared_ptr<sql::PreparedStatement> sql_stmt_ptr;
	typedef std::map<uintmax_t, sql_stmt_ptr> sql_stmt_map_type;
	typedef boost::shared_ptr<sql_stmt_map_type> sql_stmt_map_ptr;

	typedef boost::shared_ptr<sql::Connection> sql_conn_ptr;
	typedef std::pair<sql_conn_ptr, sql_stmt_map_ptr> sql_conn_info;

	typedef std::list<sql_conn_info> sql_conn_list_type;
	typedef boost::shared_ptr<sql_conn_list_type> sql_conn_list_ptr;

	typedef std::pair<PerfCounter *, PerfCounter *> sql_conn_perf;
	typedef std::pair<sql_conn_list_ptr, sql_conn_perf> sql_conn_entry;

	typedef std::map<std::string, sql_conn_entry> sql_conn_map;

public:
	class ScopedConnection {
		friend class MySqlDbConnPool;
	private:
		boost::shared_ptr<MySqlDbConnPool> _pool;
		MySqlDbUri _uri;
		sql_conn_info _conn;
		sql_conn_perf _perf;

	private:
		ScopedConnection(const boost::shared_ptr<MySqlDbConnPool> &pool, const MySqlDbUri &uri,
				const sql_conn_info &conn, const sql_conn_perf &perf)
			: _pool(pool), _uri(uri), _conn(conn), _perf(perf) {
			DbPerfC::Pool::used()->Increment();
		};
		ScopedConnection(const ScopedConnection &conn);
		ScopedConnection &operator=(const ScopedConnection &conn);
	public:
		~ScopedConnection() {
			_pool->Release(_uri, _conn);
			DbPerfC::Pool::used()->Decrement();
		};

	public:
		inline sql::Connection &operator*() { return *(_conn.first); };
		inline sql::Connection *operator->() { return _conn.first.get(); };

		inline sql::PreparedStatement *statement(const uintmax_t id) const {
			sql_stmt_map_type::const_iterator it = _conn.second->find(id);
			return it == _conn.second->end() ? 0 : it->second.get();
		}
		inline void statement(const uintmax_t id, sql::PreparedStatement *const statement) {
			(*(_conn.second))[id] = sql_stmt_ptr(statement);
		}

		inline PerfCounter *perfR() const { return _perf.first; };
		inline PerfCounter *perfW() const { return _perf.second; };
	};

private:
	static const int MAX_CONN_IN_POOL = 100;

public:
	inline static boost::shared_ptr<MySqlDbConnPool> Create() {
		return boost::shared_ptr<MySqlDbConnPool>(new MySqlDbConnPool());
	};

private:
	boost::mutex _lock;
	sql::mysql::MySQL_Driver *_driver;
	sql_conn_map _conns;

private:
	MySqlDbConnPool() : _lock(), _conns() { _driver = sql::mysql::get_driver_instance(); };
	MySqlDbConnPool(const MySqlDbConnPool &pool);
	MySqlDbConnPool &operator=(const MySqlDbConnPool &pool);

public:
	std::auto_ptr<ScopedConnection> Fetch(const MySqlDbUri &uri);

private:
	void Release(const MySqlDbUri &uri, const sql_conn_info &conn);
};

}}

#endif
