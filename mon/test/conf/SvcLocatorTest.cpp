#include <boost/test/unit_test.hpp>
#include "ConfFileFixture.h"

namespace msvc { namespace test {

using namespace std;
using namespace boost;
using namespace boost::unit_test;
using namespace msvc::cfg;

namespace {
  const string _USR_NONE_ = "";
  const int _SIZE_OF_USR_ = 2;
  const string _USR_[] = { "1", "2" };
}

BOOST_FIXTURE_TEST_SUITE(SvcLocatorTest, ConfFileFixture)

BOOST_AUTO_TEST_CASE(LocateRRTest)
{
	for (int i = 0; i < 5; ++i) {
		BOOST_CHECK_EQUAL(
				SvcLocator::GetUri(RoleSvc::PFS, _USR_[i % _SIZE_OF_USR_]),
				_VAL_SVC_PFS_URI_[(i + 1) % _SIZE_OF_URI_]
			);
	}
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
	for (int i = 0; i < 5; ++i) {
		BOOST_CHECK_EQUAL(
				SvcLocator::GetUri(RoleCac::USR_R, _USR_NONE_),
				_VAL_CAC_USR_R_URI_[(i + 1) % _SIZE_OF_URI_]
			);
	}
}

BOOST_AUTO_TEST_SUITE_END()
}}
