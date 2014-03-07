#ifndef MSVC_TEST_LOG_FIXTURE_H
#define MSVC_TEST_LOG_FIXTURE_H

#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include "incl/log/log.h"

namespace msvc { namespace test {

using namespace boost;
using namespace boost::chrono;
using namespace msvc::log;

class LogFixture {

public:
	LogFixture() {
		TraceClient::Init();
		LogClient::Init();
	};
	virtual ~LogFixture() {
		this_thread::sleep_for(seconds(1));
	};
};

}}

#endif
