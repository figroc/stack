#include <boost/test/unit_test.hpp>
#include "incl/util/util.h"

namespace msvc { namespace test {

using namespace std;
using namespace boost;
using namespace boost::unit_test;
using namespace msvc::util;
using namespace msvc::util::_di;

namespace {
	const string _ToSplit_ = "1,2,3,x5,6,7";
	const string _Delimeter_ = ",x";
	const int _AsSize_ = 6;
	const string _AsStrings_[] = { "1", "2", "3", "5", "6", "7" };
	const int _AsIntegers_[] = { 1, 2, 3, 5, 6, 7 };
}

BOOST_AUTO_TEST_SUITE(StringTest)

BOOST_AUTO_TEST_CASE(StringCastIntegerTest)
{
	BOOST_CHECK_EQUAL(StringCast<long>(""), 0L);
	BOOST_CHECK_EQUAL(StringCast<long>("", 5L), 5L);
	BOOST_CHECK_EQUAL(StringCast<long>("0"), 0L);
	BOOST_CHECK_EQUAL(StringCast<long>("0", 5L), 0L);
	BOOST_CHECK_EQUAL(StringCast<long>("12345"), 12345L);
	BOOST_CHECK_EQUAL(StringCast<long>("-12345"), -12345L);
	BOOST_CHECK_EQUAL(StringCast<long>("not_integer", 5L), 5L);
}

BOOST_AUTO_TEST_CASE(StringCastBooleanTest)
{
	BOOST_CHECK_EQUAL(StringCast<bool>(""), false);
	BOOST_CHECK_EQUAL(StringCast<bool>("true"), true);
	BOOST_CHECK_EQUAL(StringCast<bool>("false"), false);
	BOOST_CHECK_EQUAL(StringCast<bool>("false", true), false);
	BOOST_CHECK_EQUAL(StringCast<bool>("on"), true);
	BOOST_CHECK_EQUAL(StringCast<bool>("off"), false);
	BOOST_CHECK_EQUAL(StringCast<bool>("off", true), false);
	BOOST_CHECK_EQUAL(StringCast<bool>("enable"), true);
	BOOST_CHECK_EQUAL(StringCast<bool>("disable"), false);
	BOOST_CHECK_EQUAL(StringCast<bool>("disable", true), false);
	BOOST_CHECK_EQUAL(StringCast<bool>("1"), true);
	BOOST_CHECK_EQUAL(StringCast<bool>("0"), false);
	BOOST_CHECK_EQUAL(StringCast<bool>("0", true), false);
	BOOST_CHECK_EQUAL(StringCast<bool>("-1"), false);
	BOOST_CHECK_EQUAL(StringCast<bool>("-1", true), true);
	BOOST_CHECK_EQUAL(StringCast<bool>("5"), false);
	BOOST_CHECK_EQUAL(StringCast<bool>("5", true), true);
	BOOST_CHECK_EQUAL(StringCast<bool>("-5"), false);
	BOOST_CHECK_EQUAL(StringCast<bool>("-5", true), true);
	BOOST_CHECK_EQUAL(StringCast<bool>("not_bool", true), true);
	BOOST_CHECK_EQUAL(StringCast<bool>("not_bool", false), false);
}

BOOST_AUTO_TEST_CASE(StringSplitStringTest)
{
	vector<string> strs = StringSplit<string>(_ToSplit_, _Delimeter_);
	BOOST_REQUIRE_EQUAL(strs.size(), _AsSize_);
	for (int i = 0; i < strs.size(); ++i)
		BOOST_CHECK_EQUAL(strs[i], _AsStrings_[i]);
}

BOOST_AUTO_TEST_CASE(StringSplitIntegerTest)
{
	vector<int> ints = StringSplit<int>(_ToSplit_, _Delimeter_);
	BOOST_CHECK_EQUAL(ints.size(), _AsSize_);
	for (int i = 0; i < ints.size(); ++i)
		BOOST_CHECK_EQUAL(ints[i], _AsIntegers_[i]);
}

BOOST_AUTO_TEST_SUITE_END()
}}
