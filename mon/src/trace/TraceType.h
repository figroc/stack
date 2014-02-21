#ifndef MSVC_LOG_TRACE_TYPE_H
#define MSVC_LOG_TRACE_TYPE_H

namespace msvc { namespace log {

enum TraceLevel {
	LC_TL_DEBUG = 10,
	LC_TL_INFO = 20,
	LC_TL_WARN = 30,
	LC_TL_ERROR = 40,
	LC_TL_FATAL = 50
};

}}

#endif //MSVC_LOG_TRACE_TYPE_H
