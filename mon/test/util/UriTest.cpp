#include <boost/test/unit_test.hpp>
#include "incl/util/util.h"

namespace msvc { namespace test {

using namespace std;
using namespace boost;
using namespace boost::unit_test;
using namespace msvc::util;

BOOST_AUTO_TEST_SUITE(UriTest)

BOOST_AUTO_TEST_CASE(FullTest)
{
	static const string str("http://usr:pass@host1:port1,host2:port2/path?q1&q2=v2&q3=");
	Uri uri(str);
	BOOST_CHECK(uri.valid());
	BOOST_CHECK_EQUAL(uri.uri(), str);
	BOOST_CHECK_EQUAL(uri.scheme(), "http");
	BOOST_CHECK_EQUAL(uri.user(), "usr");
	BOOST_CHECK_EQUAL(uri.pass(), "pass");
	BOOST_CHECK_EQUAL(uri.host(), "host1:port1,host2:port2");
	BOOST_CHECK_EQUAL(uri.path(), "path");
	BOOST_CHECK_EQUAL(uri.identity(), "http:usr@host1:port1,host2:port2$path");
	BOOST_CHECK(uri.optExists("q1"));
	BOOST_CHECK_EQUAL(uri.optValue("q1"), "");
	BOOST_CHECK_EQUAL(uri.optValue("q2"), "v2");
	BOOST_CHECK(uri.optExists("q3"));
	BOOST_CHECK_EQUAL(uri.optValue("q3"), "");
}

BOOST_AUTO_TEST_CASE(NoUserTest)
{
	static const string str("redis://:pass@host:port/path?q1");
	Uri uri(str);
	BOOST_CHECK(uri.valid());
	BOOST_CHECK_EQUAL(uri.uri(), str);
	BOOST_CHECK_EQUAL(uri.scheme(), "redis");
	BOOST_CHECK_EQUAL(uri.user(), "");
	BOOST_CHECK_EQUAL(uri.pass(), "pass");
	BOOST_CHECK_EQUAL(uri.host(), "host:port");
	BOOST_CHECK_EQUAL(uri.path(), "path");
	BOOST_CHECK_EQUAL(uri.identity(), "redis:@host:port$path");
	BOOST_CHECK(uri.optExists("q1"));
	BOOST_CHECK_EQUAL(uri.optValue("q1"), "");
}

BOOST_AUTO_TEST_CASE(NoUserPassTest)
{
	static const string str("ftp://:@host:port/path?");
	Uri uri(str);
	BOOST_CHECK(uri.valid());
	BOOST_CHECK_EQUAL(uri.uri(), str);
	BOOST_CHECK_EQUAL(uri.scheme(), "ftp");
	BOOST_CHECK_EQUAL(uri.user(), "");
	BOOST_CHECK_EQUAL(uri.pass(), "");
	BOOST_CHECK_EQUAL(uri.host(), "host:port");
	BOOST_CHECK_EQUAL(uri.path(), "path");
	BOOST_CHECK_EQUAL(uri.identity(), "ftp:@host:port$path");
	BOOST_CHECK(!uri.optExists("q1"));
	BOOST_CHECK_EQUAL(uri.optValue("q1"), "");
}

BOOST_AUTO_TEST_CASE(NoUserPassHostTest)
{
	static const string str("local://:@/path");
	Uri uri(str);
	BOOST_CHECK(uri.valid());
	BOOST_CHECK_EQUAL(uri.uri(), str);
	BOOST_CHECK_EQUAL(uri.scheme(), "local");
	BOOST_CHECK_EQUAL(uri.user(), "");
	BOOST_CHECK_EQUAL(uri.pass(), "");
	BOOST_CHECK_EQUAL(uri.host(), "");
	BOOST_CHECK_EQUAL(uri.path(), "path");
	BOOST_CHECK_EQUAL(uri.identity(), "local:@$path");
	BOOST_CHECK(!uri.optExists("q1"));
	BOOST_CHECK_EQUAL(uri.optValue("q1"), "");
}

BOOST_AUTO_TEST_CASE(AbUserPassTest)
{
	static const string str("x://host:port/path");
	Uri uri(str);
	BOOST_CHECK(!uri.valid());
	BOOST_CHECK_EQUAL(uri.uri(), str);
}

BOOST_AUTO_TEST_SUITE_END()
}}
