#include <boost/test/unit_test.hpp>
#include "DatabaseFixture.h"
#include <boost/lambda/lambda.hpp>

namespace msvc { namespace test {

using namespace std;
using namespace boost;
using namespace boost::unit_test;
using namespace boost::lambda;
using namespace msvc::db;

namespace {
void _database_test(const auto_ptr<Database> &db)
{
	BOOST_REQUIRE(db.get());

	QuerySpec query = QuerySpec() << PropName("name", PVT_STRING) << PropName("age", PVT_INTEGER);
	auto_ptr<DocTable> doc = db->Query("user", query, OprParam());
	BOOST_REQUIRE(doc.get());
	BOOST_CHECK_EQUAL(doc->size(), 0);

	DataSpec data = DataSpec() << PropName("name", PVT_STRING) << PropName("age", PVT_INTEGER);
	db->Insert("user", data, OprParam() << "john" << 25);

	doc = db->Query("user", query, OprParam());
	BOOST_REQUIRE(doc.get());
	BOOST_CHECK_EQUAL(doc->size(), 1);
	BOOST_CHECK_EQUAL((*doc)[0]->at("name").getValue<string>(), "john");
	BOOST_CHECK_EQUAL((*doc)[0]->at("age").getValue<int>(), 25);

	ModifySpec modify = ModifySpec() << ModifyOption("age", PVT_INTEGER)
									<< (QueryOption() << CriteriaOption("name", PVT_STRING));
	db->Update("user", modify, OprParam() << 30 << "john");

	doc = db->Query("user", query, OprParam());
	BOOST_REQUIRE(doc.get());
	BOOST_CHECK_EQUAL(doc->size(), 1);
	BOOST_CHECK_EQUAL((*doc)[0]->at("name").getValue<string>(), "john");
	BOOST_CHECK_EQUAL((*doc)[0]->at("age").getValue<int>(), 30);

	RemoveSpec remove = RemoveSpec() << (QueryOption() << CriteriaOption("name", PVT_STRING));
	db->Remove("user", remove, OprParam() << "john");

	doc = db->Query("user", query, OprParam());
	BOOST_REQUIRE(doc.get());
	BOOST_CHECK_EQUAL(doc->size(), 0);
}
}

BOOST_FIXTURE_TEST_SUITE(DatabaseTest, DatabaseFixture)

BOOST_AUTO_TEST_CASE(MySqlDbTest)
{
	_database_test(Database::Get("mysql://dev:test@127.0.0.1:3306/test"));
}

BOOST_AUTO_TEST_CASE(MongoDbTest)
{
	_database_test(Database::Get("mongodb://dev:test@127.0.0.1:27017/test"));
}

BOOST_AUTO_TEST_SUITE_END()

}}
