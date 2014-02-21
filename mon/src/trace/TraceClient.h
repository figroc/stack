#ifndef MSVC_LOG_TRACE_CLIENT_H
#define MSVC_LOG_TRACE_CLIENT_H

#include "TraceType.h"
#include <string>
#include <exception>
#include <boost/atomic.hpp>

namespace msvc { namespace log {

class TraceClient {
private:
	static boost::atomic_bool _init;

public:
	static void Init();
	static void Record(const std::string &message);

private:
	TraceClient();
};

}}

#endif //MSVC_LOG_TRACE_CLIENT_H
