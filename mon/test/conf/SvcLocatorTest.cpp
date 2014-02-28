#include <boost/test/unit_test.hpp>
#include "ConfFileFixture.h"
#include <boost/lambda/lambda.hpp>
#include <boost/chrono.hpp>

namespace msvc { namespace test {

using namespace std;
using namespace boost;
using namespace boost::unit_test;
using namespace boost::lambda;
using namespace msvc::cfg;

namespace {
const string _USR_NONE_ = "";
const int _SIZE_OF_USR_ = 2;
const string _USR_[] = { "1", "2" };

template<typename F, typename U>
void _svc_locator_rr_test(const SvcRole &role, const F &user, const int size, const U &uri)
{
	bool first(true);
	int shift(0);
	for (int i = 0; i < 15; ++i) {
		const string result = SvcLocator::GetUri(role, user(i));
		if (first) {
			for ( ; shift < size; ++shift) {
				if (result == uri[shift])
					break;
			}
			first = false;
		}
		BOOST_CHECK_EQUAL(result, uri[(i + shift) % size]);
	}
}
}

BOOST_FIXTURE_TEST_SUITE(SvcLocatorTest, ConfFileFixture)

BOOST_AUTO_TEST_CASE(LocateRRTest)
{
	_svc_locator_rr_test(RoleSvc::PFS,
			constant(_USR_)[boost::lambda::_1 % _SIZE_OF_USR_],
			_SIZE_OF_URI_, _VAL_SVC_PFS_URI_);
}

BOOST_AUTO_TEST_CASE(LocateModTest)
{
	map<string, string> check;
	for (int i = 0; i < 5; ++i) {
		string usr = _USR_[i % _SIZE_OF_USR_];
		string uri = SvcLocator::GetUri(RoleCac::USR_R, usr);
		map<string, string>::iterator it = check.find(usr);
		if (it == check.end()) {
			check.insert(make_pair(usr, uri));
		} else {
			BOOST_CHECK_EQUAL(uri, it->second);
		}
	}
	BOOST_CHECK_EQUAL(check.size(), _SIZE_OF_USR_);
}

BOOST_AUTO_TEST_CASE(LocateMod2RRTest)
{
	_svc_locator_rr_test(RoleCac::USR_R,
			constant(_USR_NONE_),
			_SIZE_OF_URI_, _VAL_CAC_USR_R_URI_);
}

BOOST_AUTO_TEST_SUITE_END()
}}
