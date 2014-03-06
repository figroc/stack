#include "MySqlDbHelper.h"
#include <cppconn/prepared_statement.h>

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
		for (int i = 0; i < query.data.size(); ) {
			switch (query.data[i].type()) {
			case PVT_INTEGER:
				row->add(cursor->getInt(++i)); break;
			case PVT_TIME:
				row->add(static_cast<time_t>(cursor->getInt(++i))); break;
			case PVT_UUID: {
				sql::SQLString str = cursor->getString(++i);
				boost::uuids::uuid uid;
				if (str.length() == uid.size()) {
					for (int n = 0; n < uid.size(); ++n)
						uid.data[n] = str[n];
				} else {
					memset(&uid, 0, 8);
				}
				row->add(uid);
				} break;
			case PVT_STRING:
			default: row->add(cursor->getString(++i).asStdString()); break;
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

void MySqlDbHelper::FillData(sql::PreparedStatement *const statement,
			const vector<PropName> &data, const OprParam &param, int &index)
{
	for (int i = 0; i < data.size(); ++i) {
		SetParameter(statement, index, data[i].type(), param.at(index++));
	}
}

void MySqlDbHelper::FillModify(sql::PreparedStatement *const statement,
			const vector<ModifyOption> &modify, const OprParam &param, int &index)
{
	for (int i = 0; i < modify.size(); ++i) {
		SetParameter(statement, index, modify[i].type(), param.at(index++));
	}
}

void MySqlDbHelper::FillQuery(sql::PreparedStatement *const statement,
			const QueryOption &query, const OprParam &param, int &index)
{
	for (int i = 0; i < query.size(); ++i) {
		if (query[i].compose()) {
			FillQuery(statement, static_cast<const QueryOption &>(query[i]), param, index);
		} else {
			SetParameter(statement, index,
					static_cast<const CriteriaOption &>(query[i]).type(),
					param.at(index++));
		}
	}
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

void MySqlDbHelper::SetParameter(sql::PreparedStatement *const statement,
				const int index, const PropValueType type, const OprValue &value)
{
	switch (type) {
	case PVT_INTEGER:
		statement->setInt(index, value.getValue<int>(0));
		break;
	case PVT_TIME:
		statement->setInt(index, static_cast<int>(value.getValue<time_t>(0)));
		break;
	case PVT_UUID: {
		boost::uuids::uuid uid = value.getValue<boost::uuids::uuid>(0);
		string str;
		for (int n = 0; n < uid.size(); ++n)
			str.push_back(uid.data[n]);
		statement->setString(index, str);
		} break;
	case PVT_STRING:
	default:
		statement->setString(index, value.getValue<string>(0));
		break;
	}
}

}}
