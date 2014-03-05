#ifndef DB_MYSQL_DB_CLIENT_H
#define DB_MYSQL_DB_CLIENT_H

#include "../../Database.h"
#include "MySqlDbUri.h"
#include "MySqlDbConnPool.h"
#include <string>
#include <memory>
#include <boost/smart_ptr.hpp>

namespace msvc { namespace db {

using namespace msvc::util;

class MySqlDbClient : public Database {
private:
	static boost::shared_ptr<MySqlDbConnPool> _pool;

public:
	static std::auto_ptr<Database> Get(const Uri &uri);

private:
	MySqlDbUri _uri;
	std::auto_ptr<MySqlDbConnPool::ScopedConnection> _client;

private:
	MySqlDbClient(const MySqlDbUri &uri) : _uri(uri) {
		_client = _pool->Fetch(uri);
	}
	MySqlDbClient(const MySqlDbClient &client);
	MySqlDbClient &operator=(const MySqlDbClient &client);

public:
	virtual std::auto_ptr<DocTable> Query(const std::string &table,
			const QuerySpec &query, const OprParam &param
		);
	virtual void Insert(const std::string &table,
			const DataSpec &data, const OprParam &param
		);
	virtual void Update(const std::string &table,
			const ModifySpec &modify, const OprParam &param
		);
	virtual void Remove(const std::string &table,
			const RemoveSpec &remove, const OprParam &param
		);

};

}}

#endif
