#include <stdexcept>

#include <boost/lexical_cast.hpp>

#include "MongoDbHelper.h"

namespace msvc { namespace db {

using namespace std;
using namespace boost;
using namespace mongo;

#define BSON_VALUE(builder, type, param, index) \
	switch (type) { \
		case PVT_TIME: 	  builder << mongo::Date_t(param.getValue<time_t>(index++) * 1000ul); break; \
		case PVT_INTEGER: builder << param.getValue<int>(index++); break; \
		case PVT_STRING:  builder << param.getValue<std::string>(index++); break; \
		case PVT_UUID:	  builder << mongo::BSONBinData( \
										param.getValue<boost::uuids::uuid>(index++).data, \
										boost::uuids::uuid::static_size(), mongo::newUUID \
									 ); break; \
		default: throw std::logic_error( \
				std::string("invalid PropValueType for MongoDbHelper::BSON_VALUE: ") \
				+= lexical_cast<std::string>(type)); \
	}

auto_ptr<DocTable> MongoDbHelper::CreateDocFromCursor(
		const QuerySpec &query, const auto_ptr<DBClientCursor> &cursor
){
	auto_ptr<DocTable> doc(new DocTable());
	for (int i = 0; i < query.data.size(); ++i) {
		doc->addName(query.data[i]);
	}

	while (cursor->more()) {
		const BSONObj data = cursor->nextSafe();
		const shared_ptr<DocRawRow> &row = doc->addRow();

		BSONObjIterator it = data.begin();
		while (it.more()) {
			const BSONElement e = it.next();
			const int n = row->index(e.fieldName());
			if (n < 0)
				continue;
			switch (query.data[n].type()) {
				case PVT_TIME: 		row->set(n, e.Date().toTimeT()); break;
				case PVT_INTEGER: 	row->set(n, e.Int());            break;
				case PVT_UUID:	{
					int len = 0;
					const char *data = e.binDataClean(len);
					boost::uuids::uuid uid;
					memcpy(&uid, data, std::min<size_t>(len, boost::uuids::uuid::static_size()));
					row->set(n, uid);
				} break;
				case PVT_STRING: 	// fall through
				default: 			row->set(n, e.String()); break;
			}
		}
		while (row->size() < query.data.size()) {
			row->add(PropValue());
		}
	}
	return doc;
}

BSONObj MongoDbHelper::BuildData(const vector<PropName> &data, const OprParam &param, int &index)
{
	BSONObjBuilder builder(64);
	for (int i = 0; i < data.size(); ++i) {
		BSON_VALUE(builder << data[i].name(), data[i].type(), param, index);
	}
	return builder.obj();
}

Query MongoDbHelper::BuildQuery(const QueryOption &query, const OprParam &param, int &index)
{
	return Query(BuildQueryInner(query, param, index));
}

BSONObj MongoDbHelper::BuildProjection(const vector<PropName> &proj)
{
	BSONObjBuilder builder(64);
	for (int i = 0; i < proj.size(); ++i) {
		builder << proj[i].name() << 1;
	}
	return builder.obj();
}

BSONObj MongoDbHelper::BuildSort(const vector<SortOption> &sort)
{
	BSONObjBuilder builder(64);
	for (int i = 0; i < sort.size(); ++i) {
		builder << sort[i].name() << MapSortOperator(sort[i].op());
	}
	return builder.obj();
}

BSONObj MongoDbHelper::BuildModify(const vector<ModifyOption> &modify, const OprParam &param, int &index)
{
	map< string, shared_ptr<BSONObjBuilder> > builder;
	for (int i = 0; i < modify.size(); ++i) {
		string op = MapModifyOperator(modify[i].op());
		map< string, shared_ptr<BSONObjBuilder> >::iterator
			it = builder.find(op);
		if (builder.end() == it) {
			it = builder.insert(std::make_pair(op, new BSONObjBuilder(64))).first;
		}
		BSON_VALUE(*it->second << modify[i].name(), modify[i].type(), param, index);
	}
	BSONObjBuilder result(64);
	for (map< string, shared_ptr<BSONObjBuilder> >::iterator
			it = builder.begin(); it != builder.end(); ++it) {
		result << (it->first) << (it->second->obj());
	}
	return result.obj();
}

BSONObj MongoDbHelper::BuildQueryInner(const QueryOption &query, const OprParam &param, int &index)
{
	BSONArrayBuilder builder(64);
	for (int i = 0; i < query.size(); ++i) {
		const ICriteria &criteria = query[i];
		if (criteria.compose()) {
			const QueryOption &nest = static_cast<const QueryOption &>(criteria);
			builder << BuildQueryInner(nest, param, index);
		} else {
			const CriteriaOption &inner = static_cast<const CriteriaOption &>(criteria);
			builder << BuildCriteriaInner(inner, param, index);
		}
	}
	return BSON(MapLogicalOperator(query.op()) << builder.arr());
}

BSONObj MongoDbHelper::BuildCriteriaInner(const CriteriaOption &criteria, const OprParam &param, int &index)
{
	BSONObjBuilder builder(64);
	BSONObjBuilderValueStream &stream(builder << criteria.name());
	if (COT_EQ == criteria.op()) {
		BSON_VALUE(stream, criteria.type(), param, index);
	} else if (COT_IN == criteria.op()) {
		const OprValue &value(param.at(index++));
		BSONArrayBuilder arr;
		for (int i = 0; i < value.size(); /* i++ in BSON_VALUE */) {
			BSON_VALUE(arr, criteria.type(), value, i);
		}
		stream << BSON("$in" << arr.arr());
	} else {
		Labeler label = stream << MapCompareOperator(criteria.op());
		BSON_VALUE(label, criteria.type(), param, index);
	}
	return builder.obj();
}

Labeler::Label &MongoDbHelper::MapCompareOperator(const CompareOpType cop)
{
	switch (cop) {
		case COT_GT:  return mongo::GT;
		case COT_GTE: return mongo::GTE;
		case COT_LT:  return mongo::LT;
		case COT_LTE: return mongo::LTE;
		case COT_IN:
		case COT_EQ:  // fall through
		default: throw logic_error(
			string("invalid CompareOpType for MongoDbHelper::MapCompareOperator: ")
			+= lexical_cast<string>(cop));
	}
}

string MongoDbHelper::MapLogicalOperator(const LogicalOpType lop)
{
	switch (lop) {
		case LOT_AND: return "$and";
		case LOT_OR:  return "$or";
		default: throw logic_error(
			string("invalid LogicalOpType for MongoDbHelper::MapLogicalOperator: ")
			+= lexical_cast<string>(lop));
	}
}

string MongoDbHelper::MapModifyOperator(const ModifyOpType mop)
{
	switch (mop) {
		case MOT_SET: return "$set";
		case MOT_INC: return "$inc";
		default: throw logic_error(
			string("invalid ModifyOpType for MongoDbHelper::MapModifyOperator: ")
			+= lexical_cast<string>(mop));
	}
}

int MongoDbHelper::MapSortOperator(const SortOpType sop)
{
	switch (sop) {
		case SOT_ASC:  return 1;
		case SOT_DESC: return -1;
		default: throw logic_error(
			string("invalid SortOpType for MongoDbHelper::MapSortOperator: ")
			+= lexical_cast<string>(sop));
	}
}

#undef BSON_VALUE

}}

