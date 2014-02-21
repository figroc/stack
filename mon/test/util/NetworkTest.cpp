#include <boost/test/unit_test.hpp>
#include "../../incl/util/util.h"

namespace msvc { namespace test {

using namespace std;
using namespace boost;
using namespace boost::unit_test;
using namespace msvc::util;

namespace {
	const string _IPv4Lo_ = "127.0.0.1";
	const string _IPv6Lo_ = "::1";
	const string _IPv4_ = "127.0.0.1";
	const string _IPv6_ = "::1";
	const string _Port_ = "80";
	const string _EPv4_ = _IPv4_ + ':' + _Port_;
	const string _EPv6_ = '[' + _IPv6_ + "]:" + _Port_;
}

BOOST_AUTO_TEST_SUITE(NetworkTest)

BOOST_AUTO_TEST_CASE(GetLocalIpAddressTest)
{
	vector<string> ips = GetLocalIpAddress();
	BOOST_REQUIRE_GT(ips.size(), 0);
	for (int i = 0; i < ips.size(); ++i) {
		BOOST_CHECK_NE(ips[i], _IPv4Lo_);
		BOOST_CHECK_NE(ips[i], _IPv6Lo_);
		BOOST_TEST_MESSAGE(ips[i]);
	}
}

BOOST_AUTO_TEST_CASE(ComposeEndpointTest)
{
	string ep = ComposeEndpoint(_IPv4_, _Port_);
	BOOST_CHECK_EQUAL(ep, _EPv4_);

	ep = ComposeEndpoint(_IPv6_, _Port_);
	BOOST_CHECK_EQUAL(ep, _EPv6_);
}

BOOST_AUTO_TEST_CASE(ParseEndpointTest)
{
	pair<string, string> ep = ParseEndpoint(_EPv4_);
	BOOST_CHECK_EQUAL(ep.first, _IPv4_);
	BOOST_CHECK_EQUAL(ep.second, _Port_);

	ep = ParseEndpoint(_EPv6_);
	BOOST_CHECK_EQUAL(ep.first, _IPv6_);
	BOOST_CHECK_EQUAL(ep.second, _Port_);
}

BOOST_AUTO_TEST_SUITE_END()
}}
