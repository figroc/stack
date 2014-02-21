#ifndef DB_DATABASE_H
#define DB_DATABASE_H

#include <memory>

#include "doc/DocRecord.h"
#include "spec/OprSpec.h"
#include "spec/OprParam.h"

namespace msvc { namespace db {

class Database {
public:
	static void Init();
	static std::auto_ptr<Database> Get(const std::string &uri);

public:
	virtual ~Database() { }

public:
	virtual std::auto_ptr<DocTable> Query(const std::string &table,
			const QuerySpec &query, const OprParam &param
	) = 0;
	virtual void Insert(const std::string &table,
			const DataSpec &data, const OprParam &param
	) = 0;
	virtual void Update(const std::string &table,
			const ModifySpec &modify, const OprParam &param
	) = 0;
	virtual void Remove(const std::string &table,
			const RemoveSpec &remove, const OprParam &param
	) = 0;

};

}}

#endif /* DB_DATABASE_H */
