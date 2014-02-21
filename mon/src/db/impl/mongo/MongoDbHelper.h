#ifndef DB_MONGODB_HELPER_H
#define DB_MONGODB_HELPER_H

#include <string>
#include <vector>
#include <memory>

#include <mongo/bson/bson.h>
#include <mongo/client/dbclient.h>

#include "../../doc/DocRecord.h"
#include "../../spec/OprSpec.h"
#include "../../spec/OprParam.h"

namespace msvc { namespace db {

class MongoDbHelper {
public:
	static std::auto_ptr<DocTable> CreateDocFromCursor(
			const QuerySpec &query, const std::auto_ptr<mongo::DBClientCursor> &cursor
		);

	static mongo::BSONObj BuildData(
			const std::vector<PropName> &data, const OprParam &param, int &index
		);

	static mongo::Query BuildQuery(const QueryOption &query, const OprParam &param, int &index);

	static mongo::BSONObj BuildProjection(const std::vector<PropName> &proj);
	static mongo::BSONObj BuildSort(const std::vector<SortOption> &sort);

	static mongo::BSONObj BuildModify(
			const std::vector<ModifyOption> &modify, const OprParam &param, int &index
		);

private:
	static mongo::BSONObj BuildQueryInner(
			const QueryOption &query, const OprParam &param, int &index
		);
	static mongo::BSONObj BuildCriteriaInner(
			const CriteriaOption &criteria, const OprParam &param, int &index
		);

	static mongo::Labeler::Label &MapCompareOperator(const CompareOpType cop);
	static std::string MapLogicalOperator(const LogicalOpType lop);
	static std::string MapModifyOperator(const ModifyOpType mop);
	static int MapSortOperator(const SortOpType sop);

private:
	MongoDbHelper() { }
};

}}

#endif /* DB_MONGODB_HELPER_H */
