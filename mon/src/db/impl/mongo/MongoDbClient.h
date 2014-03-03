#ifndef DB_MONGODB_CLIENT_H
#define DB_MONGODB_CLIENT_H

#include <memory>
#include <string>
#include <stdexcept>

#include <boost/smart_ptr.hpp>
#include <mongo/client/dbclient.h>

#include "../../Database.h"
#include "../DbUri.h"
#include "MongoDbConnPool.h"

namespace msvc { namespace db {

class MongoDbClient : public Database {
private:
	static MongoDbConnPool _pool;

public:
	static std::auto_ptr<Database> Get(const DbUri &uri);

private:
	MongoDbUri _uri;
	std::auto_ptr<MongoDbConnPool::ScopedClient> _client;

private:
	MongoDbClient(const MongoDbUri &uri) : _uri(uri) {
		_client = _pool.Fetch(uri);
	}

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

private:
	inline void CheckLastError(const std::string &table, const std::string &operation) {
		const std::string error = (*_client)->getLastError(_uri.name());
		if (!error.empty())
			throw std::runtime_error(std::string("failed to ").append(operation).append(": ")
				  .append(_uri.userAtDb()).append(".").append(table));
	}
};

}}

#endif /* DB_MONGODB_CLIENT_H */
