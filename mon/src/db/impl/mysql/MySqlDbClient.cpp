#include "MySqlDbClient.h"
#include "MySqlDbHelper.h"
#include <stdexcept>

namespace msvc { namespace db {

using namespace std;
using namespace msvc::perf;
using namespace msvc::util;

boost::shared_ptr<MySqlDbConnPool> MySqlDbClient::_pool = MySqlDbConnPool::Create();

auto_ptr<Database> MySqlDbClient::Get(const Uri &uri)
{
	MySqlDbUri info(uri);
	if (!info.valid())
		throw logic_error(std::string("invalid mysql connection string: ").append(uri.uri()));
	return auto_ptr<Database>(new MySqlDbClient(info));
}

auto_ptr<DocTable> MySqlDbClient::Query(const string &table, const QuerySpec &query, const OprParam &param)
{
	PerfCounting(_client->perfR());
	PerfTiming(_client->perfR());

	sql::PreparedStatement *stmt = _client->statement(query.version());
	if (!stmt) {
		string sql = MySqlDbHelper::BuildProjection(query.data);
		sql.append(" from `").append(table).append("` ");
		sql.append(MySqlDbHelper::BuildQuery(query.query)).push_back(' ');
		sql.append(MySqlDbHelper::BuildSort(query.sort));

		stmt = (*_client)->prepareStatement(sql);
		if (!stmt)
			throw runtime_error(string("failed to prepare: ") + sql);
		_client->statement(query.version(), stmt);
	}

	int index = 0;
	MySqlDbHelper::FillQuery(stmt, query.query, param, index);

	auto_ptr<DocTable> doc;
	try {
		auto_ptr<sql::ResultSet> rs(stmt->executeQuery());
		doc = MySqlDbHelper::CreateDocFromCursor(query, rs);
		stmt->clearParameters();
	} catch (...) {
		stmt->clearParameters();
		throw;
	}
	return doc;
}

void MySqlDbClient::Insert(const std::string &table, const DataSpec &data, const OprParam &param)
{
	PerfCounting(_client->perfW());
	PerfTiming(_client->perfW());

	sql::PreparedStatement *stmt = _client->statement(data.version());
	if (!stmt) {
		string sql("insert into `");
		sql.append(table).append("` ");
		sql.append(MySqlDbHelper::BuildData(data.data));

		stmt = (*_client)->prepareStatement(sql);
		if (!stmt)
			throw runtime_error(string("failed to prepare: ") + sql);
		_client->statement(data.version(), stmt);
	}

	int index = 0;
	MySqlDbHelper::FillData(stmt, data.data, param, index);

	try {
		stmt->execute();
		stmt->clearParameters();
	} catch (...) {
		stmt->clearParameters();
		throw;
	}
}

void MySqlDbClient::Update(const std::string &table, const ModifySpec &modify, const OprParam &param)
{
	PerfCounting(_client->perfW());
	PerfTiming(_client->perfW());

	sql::PreparedStatement *stmt = _client->statement(modify.version());
	if (!stmt) {
		string sql("update `");
		sql.append(table).append("` ");
		sql.append(MySqlDbHelper::BuildModify(modify.modify)).push_back(' ');
		sql.append(MySqlDbHelper::BuildQuery(modify.query));

		stmt = (*_client)->prepareStatement(sql);
		if (!stmt)
			throw runtime_error(string("failed to prepare: ") + sql);
		_client->statement(modify.version(), stmt);
	}

	int index = 0;
	MySqlDbHelper::FillModify(stmt, modify.modify, param, index);
	MySqlDbHelper::FillQuery(stmt, modify.query, param, index);

	try {
		stmt->execute();
		stmt->clearParameters();
	} catch (...) {
		stmt->clearParameters();
		throw;
	}
}

void MySqlDbClient::Remove(const std::string &table, const RemoveSpec &remove, const OprParam &param)
{
	PerfCounting(_client->perfW());
	PerfTiming(_client->perfW());

	sql::PreparedStatement *stmt = _client->statement(remove.version());
	if (!stmt) {
		string sql("delete from `");
		sql.append(table).append("` ");
		sql.append(MySqlDbHelper::BuildQuery(remove.query));

		stmt = (*_client)->prepareStatement(sql);
		if (!stmt)
			throw runtime_error(string("failed to prepare: ") + sql);
		_client->statement(remove.version(), stmt);
	}

	int index = 0;
	MySqlDbHelper::FillQuery(stmt, remove.query, param, index);

	try {
		stmt->execute();
		stmt->clearParameters();
	} catch (...) {
		stmt->clearParameters();
		throw;
	}
}

}}
