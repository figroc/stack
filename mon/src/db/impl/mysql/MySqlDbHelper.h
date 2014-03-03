#ifndef DB_MYSQL_DB_HELPER_H
#define DB_MYSQL_DB_HELPER_H

#include "../../doc/PropItem.h"
#include "../../doc/DocRecord.h"
#include "../../spec/OprSpec.h"
#include "../../spec/OprParam.h"
#include <memory>
#include <mysql_connection.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

namespace msvc { namespace db {

class MySqlDbHelper {
public:
	static std::auto_ptr<DocTable> CreateDocFromCursor(
			const QuerySpec &query, const std::auto_ptr<sql::ResultSet> &cursor
		);

	static std::string BuildData(const std::vector<PropName> &data);
	static std::string BuildModify(const std::vector<ModifyOption> &modify);

	static std::string BuildQuery(const QueryOption &query);

	static std::string BuildProjection(const std::vector<PropName> &proj);
	static std::string BuildSort(const std::vector<SortOption> &sort);

private:
	static std::string BuildQueryInner(const QueryOption &query);
	static std::string BuildCriteriaInner(const CriteriaOption &criteria);

private:
	MySqlDbHelper();
};

}}

#endif
