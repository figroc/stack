#ifndef MSVC_LOG_TRACE_CLIENT_H
#define MSVC_LOG_TRACE_CLIENT_H

#include "TraceType.h"
#include "TraceQueue.h"
#include "TraceFile.h"
#include <string>
#include <boost/atomic.hpp>

namespace msvc { namespace log {

class TraceClient {
private:
	static boost::atomic_bool _init;
	static msvc::log::_di::TraceQueue<std::string> _queue;
	static msvc::log::_di::TraceFile _file;

public:
	static void Init();
	static void Record(const std::string &message);

private:
	static void Recorder();

private:
	TraceClient();
};

}}

#endif //MSVC_LOG_TRACE_CLIENT_H
