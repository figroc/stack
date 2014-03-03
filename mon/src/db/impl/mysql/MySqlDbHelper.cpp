#include "MySqlDbHelper.h"

namespace msvc { namespace db {

using namespace std;

auto_ptr<DocTable> MySqlDbHelper::CreateDocFromCursor(const QuerySpec &query, const auto_ptr<sql::ResultSet> &cursor)
{
	auto_ptr<DocTable> doc(new DocTable());
	for (int i = 0; i < query.data.size(); ++i) {
		doc->addName(query.data[i]);
	}

	while (cursor->next()) {
		const boost::shared_ptr<DocRawRow> row = doc->addRow();
		for (int i = 0; i < query.data.size(); ++i) {
			switch (query.data[i].type()) {
			case PVT_INTEGER: row->add(cursor->getInt(i)); break;
			case PVT_TIME: row->add((time_t)(cursor->getInt(i))); break;
			//TODO: PVT_UUID
			case PVT_STRING:
			default: row->add(cursor->getString(i).asStdString()); break;
			}
		}
	}

	return doc;
}

string MySqlDbHelper::BuildData(const vector<PropName> &data)
{
	static const char PREFIX[] = { '(', ',' };

	string part;
	if (!data.empty()) {
		for (int i = 0; i < data.size(); ++i) {
			part.push_back(PREFIX[(bool)i]);
			part.push_back('`');
			part.append(data[i].name());
			part.push_back('`');
		}
		part.append(") values ");
		for (int i = 0; i < data.size(); ++i) {
			part.push_back(PREFIX[(bool)i]);
			part.push_back('?');
		}
		part.push_back(')');
	}
	return part;
}

string MySqlDbHelper::BuildModify(const vector<ModifyOption> &modify)
{
	static const char PREFIX[] = { ' ', ',' };

	string part;
	if (!modify.empty()) {
		part.append("set");
		for(int i = 0; i < modify.size(); ++i) {
			part.push_back(PREFIX[(bool)i]);
			part.push_back('`');
			part.append(modify[i].name());
			part.push_back('`');
			switch (modify[i].op()) {
			case MOT_INC:
				part.append("=`").append(modify[i].name()).append("`+?");
				break;
			case MOT_SET:
			default:
				part.append("=?");
				break;
			}
		}
	}
	return part;
}

string MySqlDbHelper::BuildQuery(const QueryOption &query)
{
	string part;
	if (query.size() > 0) {
		part.append("where ").append(BuildQueryInner(query));
	}
	return part;
}

string MySqlDbHelper::BuildProjection(const vector<PropName> &proj)
{
	static const char PREFIX[] = { ' ', ',' };

	string part;
	if (!proj.empty()) {
		part.append("select");
		for (int i = 0; i < proj.size(); ++i) {
			part.push_back(PREFIX[(bool)i]);
			part.push_back('`');
			part.append(proj[i].name());
			part.push_back('`');
		}
	}
	return part;
}

string MySqlDbHelper::BuildSort(const vector<SortOption> &sort)
{
	static const string SOT[] = { " asc", " desc" };
	static const char PREFIX[] = { ' ', ',' };

	string part;
	if (!sort.empty()) {
		part.append("order by");
		for (int i = 0; i < sort.size(); ++i) {
			part.push_back(PREFIX[(bool)i]);
			part.push_back('`');
			part.append(sort[i].name());
			part.push_back('`');
			part.append(SOT[sort[i].op()]);
		}
	}
	return part;
}

string MySqlDbHelper::BuildQueryInner(const QueryOption &query)
{
	static const string LOT[] = { "and", "or" };

	const string lop[] = { "", LOT[query.op()] };

	string part;
	for (int i = 0; i < query.size(); ++i) {
		part.append(lop[(bool)i]);
		part.push_back(' ');
		if (query[i].compose()) {
			part.push_back('(');
			part.append(BuildQueryInner(static_cast<const QueryOption &>(query[i])));
			part.push_back(')');
		} else {
			part.append(BuildCriteriaInner(static_cast<const CriteriaOption &>(query[i])));
		}
		part.push_back(' ');
	}
	return part;
}

string MySqlDbHelper::BuildCriteriaInner(const CriteriaOption &criteria)
{
	static const string COT[] = { "=", "<", ">", "<=", ">=", " in " };

	string part;
	part.push_back('`');
	part.append(criteria.name());
	part.push_back('`');
	part.append(COT[criteria.op()]);
	part.push_back('?'); //TODO: <in> needs extra work
	return part;
}

}}
