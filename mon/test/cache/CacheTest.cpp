#include <boost/test/unit_test.hpp>
#include <boost/lambda/lambda.hpp>
#include "incl/cache/cache.h"

namespace msvc { namespace test {

using namespace std;
using namespace boost;
using namespace boost::lambda;
using namespace boost::unit_test;
using namespace msvc::cache;

namespace {
void _cache_single_test(const auto_ptr<Cache> &cache)
{
	BOOST_REQUIRE(cache.get());

	BOOST_CHECK_EQUAL(cache->GetStr("key1"), "");

	cache->SetStr("key1", "value1");
	BOOST_CHECK_EQUAL(cache->GetStr("key1"), "value1");

	cache->SetStr("key2", "value2");
	BOOST_CHECK_EQUAL(cache->GetStr("key2"), "value2");

	vector<string> keys; keys.push_back("key1"); keys.push_back("key2");
	vector<string> vals = cache->GetStr(keys);
	BOOST_CHECK_EQUAL(vals.size(), 2);
	if (vals.size() == 2) {
		BOOST_CHECK_EQUAL(vals[0], "value1");
		BOOST_CHECK_EQUAL(vals[1], "value2");
	}

	cache->Drop("key1");
	BOOST_CHECK_EQUAL(cache->GetStr("key1"), "");
	cache->Drop(keys);
	BOOST_CHECK_EQUAL(cache->GetStr("key2"), "");
}

void _cache_set_test(const auto_ptr<Cache> &cache)
{
	BOOST_REQUIRE(cache.get());

	vector<string> set = cache->GetStrSet("key");
	BOOST_CHECK_EQUAL(set.size(), 0);

	cache->SetStrSet("key", "value");
	set = cache->GetStrSet("key");
	BOOST_CHECK_EQUAL(set.size(), 1);
	if (set.size() == 1) {
		BOOST_CHECK_EQUAL(set[0], "value");
	}

	cache->Drop("key");
	set = cache->GetStrSet("key");
	BOOST_CHECK_EQUAL(set.size(), 0);

	vector<string> vals; vals.push_back("value1"); vals.push_back("value2");
	cache->SetStrSet("key", vals);
	set = cache->GetStrSet("key");
	BOOST_CHECK_EQUAL(set.size(), 2);

	cache->AddStrSet("key", "value3");
	set = cache->GetStrSet("key");
	BOOST_CHECK_EQUAL(set.size(), 3);

	cache->AddStrSet("key", vals);
	set = cache->GetStrSet("key");
	BOOST_CHECK_EQUAL(set.size(), 3);

	cache->SubStrSet("key", vals);
	set = cache->GetStrSet("key");
	BOOST_CHECK_EQUAL(set.size(), 1);

	cache->SubStrSet("key", "value3");
	set = cache->GetStrSet("key");
	BOOST_CHECK_EQUAL(set.size(), 0);

	cache->Drop("key");
}
}

BOOST_AUTO_TEST_SUITE(CacheTest)

BOOST_AUTO_TEST_CASE(RedisTest)
{
	auto_ptr<Cache> cache = Cache::Get("redis://:@127.0.0.1:6379/1");
	BOOST_REQUIRE(cache.get());

	_cache_single_test(cache);
	_cache_set_test(cache);
}

BOOST_AUTO_TEST_SUITE_END()

}}
