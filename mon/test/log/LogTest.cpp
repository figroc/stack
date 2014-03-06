#include <boost/test/unit_test.hpp>
#include "../../incl/log/log.h"

namespace msvc { namespace test {

using namespace std;
using namespace boost;
using namespace boost::chrono;
using namespace boost::unit_test;
using namespace msvc::log;

BOOST_AUTO_TEST_SUITE(LogTest)

BOOST_AUTO_TEST_CASE(TraceOutputTest)
{
	TraceClient::Init();
	LC_TRACE_DEBUG("%%", 1, "nuts");
	LC_TRACE_INFO("%%", 2, "nuts");
	LC_TRACE_WARN("%%", 3, "nuts");
	LC_TRACE_ERROR("%%", 4, "nuts");
	LC_TRACE_FATAL("%%", 5, "nuts");
	this_thread::sleep_for(seconds(1));
	BOOST_TEST_CHECKPOINT("LC_TRACE_*");
}

BOOST_AUTO_TEST_CASE(LogOutputTest)
{
	LogClient::Init();
	LC_LOG_EVENT("unit_test", 1, "some action");
	LC_LOG_EVENT("unit_test", 2, "some,action");
	LC_LOG_EVENT("unit_test", 3, "\"some action\"");
	LC_LOG_EVENT("unit_test", 4, "\"some,action\"");
	this_thread::sleep_for(seconds(1));
	BOOST_TEST_CHECKPOINT("LC_LOG_EVENT");
}

BOOST_AUTO_TEST_SUITE_END()
}}
