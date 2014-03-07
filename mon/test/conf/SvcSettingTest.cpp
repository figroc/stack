#include <boost/test/unit_test.hpp>
#include "incl/conf/conf.h"
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

BOOST_AUTO_TEST_SUITE(SvcSettingTest)

BOOST_AUTO_TEST_CASE(GetUriSvcTest)
{
	SvcSetting::__internal_switch_role__(RoleSvc::PFS);
	BOOST_CHECK_EQUAL(SvcSetting::UriSvc(RoleSvc::PFS), _VAL_SVC_PFS_URI_[0]);
	BOOST_CHECK_EQUAL(SvcSetting::UriSvc(RoleSvc::GFS), _VAL_SVC_GFS_URI_[0]);
	BOOST_CHECK_EQUAL(SvcSetting::UriSvc(RoleCac::USR_R), _VAL_CAC_USR_R_URI_[0]);
}

BOOST_AUTO_TEST_CASE(GetUriAuxTest)
{
	SvcSetting::__internal_switch_role__(RoleSvc::PFS);
	BOOST_CHECK_EQUAL(SvcSetting::UriAux(_CONF_PERF_), _VAL_SVC_PFS_PERF_);
	SvcSetting::__internal_switch_role__(RoleSvc::GFS);
	BOOST_CHECK_EQUAL(SvcSetting::UriAux(_CONF_PERF_), _VAL_SVC_GFS_PERF_);
	SvcSetting::__internal_switch_role__(RoleCac::USR_R);
	BOOST_CHECK_EQUAL(SvcSetting::UriAux(_CONF_PERF_), _VAL_CAC_USR_R_PERF_);
	SvcSetting::__internal_switch_role__(RoleSvc::PFS);
}

BOOST_AUTO_TEST_CASE(BooleanTest)
{
	BOOST_CHECK_EQUAL(SvcSetting::Boolean(_CONF_BTR_), _VAL_BTR_);
	BOOST_CHECK_EQUAL(SvcSetting::Boolean(_CONF_BFA_), _VAL_BFA_);
}

BOOST_AUTO_TEST_CASE(IntegerTest)
{
	BOOST_CHECK_EQUAL(SvcSetting::Integer(_CONF_INT_), _VAL_INT_);
}

BOOST_AUTO_TEST_CASE(StringTest)
{
	BOOST_CHECK_EQUAL(SvcSetting::String(_CONF_STR_), _VAL_STR_);
	vector<string> strs = SvcSetting::Strings(_CONF_STR_);
	BOOST_REQUIRE_EQUAL(strs.size(), 1);
	BOOST_CHECK_EQUAL(strs[0], _VAL_STR_);
}

BOOST_AUTO_TEST_SUITE_END()
}}
