#include <cstring>
#include <string>
#include <stdexcept>
#include <algorithm>

#include "MongoDbClient.h"
#include "MongoDbHelper.h"
#include "../../util/DbPerfC.h"

namespace msvc { namespace db {

using namespace msvc::perf;

MongoDbConnPool MongoDbClient::_pool;

std::auto_ptr<Database> MongoDbClient::Get(const DbUri &uri)
{
	const MongoDbUri info(uri);
	if (!info.valid()) {
		throw std::logic_error(std::string("invalid mongodb connection string: ").append(uri.uri()));
	}
	return std::auto_ptr<Database>(new MongoDbClient(info));
}

std::auto_ptr<DocTable> MongoDbClient::Query(const std::string &table, const QuerySpec &query, const OprParam &param)
{
	PerfCounting counting(_client->perfR());
	PerfTiming timing(_client->perfR());

	int index = 0;
	const mongo::BSONObj proj = MongoDbHelper::BuildProjection(query.data);
	mongo::Query filter = MongoDbHelper::BuildQuery(query.query, param, index);
	if (query.sort.size() > 0)
		filter.sort(MongoDbHelper::BuildSort(query.sort));

	std::auto_ptr<mongo::DBClientCursor> cursor = (*_client)->query(
			_uri.name() + "." + table, filter, 10000, 0, &proj
		);
	if (!cursor.get()) {
		throw std::runtime_error(std::string("failed to query: ")
			  .append(_uri.userAtDb()).append(".").append(table));
	}

	return MongoDbHelper::CreateDocFromCursor(query, cursor);
}

void MongoDbClient::Insert(const std::string &table, const DataSpec &data, const OprParam &param)
{
	static const std::string OP = "insert";
	PerfCounting counting(_client->perfW());
	PerfTiming timing(_client->perfW());

	int index = 0;
	const mongo::BSONObj insert = MongoDbHelper::BuildData(data.data, param, index);
	(*_client)->insert(_uri.name() + "." + table, insert);
	CheckLastError(table, OP);
}

void MongoDbClient::Update(const std::string &table, const ModifySpec &modify, const OprParam &param)
{
	static const std::string OP = "update";
	PerfCounting counting(_client->perfW());
	PerfTiming timing(_client->perfW());

	int index = 0;
	const mongo::BSONObj data = MongoDbHelper::BuildModify(modify.modify, param, index);
	const mongo::Query filter = MongoDbHelper::BuildQuery(modify.query, param, index);
	(*_client)->update(_uri.name() + "." + table, filter, data, false, true);
	CheckLastError(table, OP);
}

void MongoDbClient::Remove(const std::string &table, const RemoveSpec &remove, const OprParam &param)
{
	static const std::string OP = "remove";
	PerfCounting counting(_client->perfW());
	PerfTiming timing(_client->perfW());

	int index = 0;
	const mongo::Query filter = MongoDbHelper::BuildQuery(remove.query, param, index);
	(*_client)->remove(_uri.name() + "." + table, filter, false);
	CheckLastError(table, OP);
}

}}

