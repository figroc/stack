#ifndef MSVC_LOG_LOG_FACET_H
#define MSVC_LOG_LOG_FACET_H

#include "LogFormat.h"
#include "LogClient.h"
#include "LogFormat.h"
#include <string>
#include <boost/date_time.hpp>

namespace msvc { namespace log {

#define LC_LOG_EVENT(...) msvc::log::_di::LogEvent(__VA_ARGS__)

}}

namespace msvc { namespace log { namespace _di {

#define MSVC_LOG_DI_LOG_EVENT_FACET_TEMPLATE_BODY(args) \
	try {\
		LogClient::Append(name, (LogFormat() << args).String()); \
	} catch (const std::exception &ex) { \
		std::cerr << "[-]" << name << ": " << ex.what() << std::endl; \
	}

template<typename A1>
void LogEvent(const std::string &name, const A1 &a1) {
	MSVC_LOG_DI_LOG_EVENT_FACET_TEMPLATE_BODY(a1);
}

template<typename A1, typename A2>
void LogEvent(const std::string &name, const A1 &a1, const A2 &a2) {
	MSVC_LOG_DI_LOG_EVENT_FACET_TEMPLATE_BODY(a1 << a2);
}

template<typename A1, typename A2, typename A3>
void LogEvent(const std::string &name, const A1 &a1, const A2 &a2, const A3 &a3) {
	MSVC_LOG_DI_LOG_EVENT_FACET_TEMPLATE_BODY(a1 << a2 << a3);
}

template<typename A1, typename A2, typename A3, typename A4>
void LogEvent(const std::string &name, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4) {
	MSVC_LOG_DI_LOG_EVENT_FACET_TEMPLATE_BODY(a1 << a2 << a3 << a4);
}

template<typename A1, typename A2, typename A3, typename A4, typename A5>
void LogEvent(const std::string &name, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4,
		const A5 &a5) {
	MSVC_LOG_DI_LOG_EVENT_FACET_TEMPLATE_BODY(a1 << a2 << a3 << a4 << a5);
}

template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
void LogEvent(const std::string &name, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4,
		const A5 &a5, const A6 &a6) {
	MSVC_LOG_DI_LOG_EVENT_FACET_TEMPLATE_BODY(a1 << a2 << a3 << a4 << a5 << a6);
}

template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
void LogEvent(const std::string &name, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4,
		const A5 &a5, const A6 &a6, const A7 &a7) {
	MSVC_LOG_DI_LOG_EVENT_FACET_TEMPLATE_BODY(a1 << a2 << a3 << a4 << a5 << a6 << a7);
}

template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
void LogEvent(const std::string &name, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4,
		const A5 &a5, const A6 &a6, const A7 &a7, const A8 &a8) {
	MSVC_LOG_DI_LOG_EVENT_FACET_TEMPLATE_BODY(a1 << a2 << a3 << a4 << a5 << a6 << a7 << a8);
}

template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
void LogEvent(const std::string &name, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4,
		const A5 &a5, const A6 &a6, const A7 &a7, const A8 &a8, const A9 &a9) {
	MSVC_LOG_DI_LOG_EVENT_FACET_TEMPLATE_BODY(a1 << a2 << a3 << a4 << a5 << a6 << a7 << a8 << a9);
}

#undef MSVC_LOG_DI_LOG_EVENT_FACET_TEMPLATE_BODY

}}}

#endif
