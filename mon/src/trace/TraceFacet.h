#ifndef MSVC_LOG_DI_TRACE_FACET_H
#define MSVC_LOG_DI_TRACE_FACET_H

#include "TraceType.h"
#include "TraceFormat.h"
#include "TraceClient.h"
#include "TraceSetting.h"
#include <string>

namespace msvc { namespace log {

#define LC_TRACE_DEBUG(...) msvc::log::_di::Trace(LC_TL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LC_TRACE_INFO(...) msvc::log::_di::Trace(LC_TL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LC_TRACE_WARN(...) msvc::log::_di::Trace(LC_TL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define LC_TRACE_ERROR(...) msvc::log::_di::Trace(LC_TL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LC_TRACE_FATAL(...) msvc::log::_di::Trace(LC_TL_FATAL, __FILE__, __LINE__, __VA_ARGS__)

}}

namespace msvc { namespace log { namespace _di {

#define MSVC_LOG_DI_TRACE_FACET_TEMPLATE_BODY(args) \
	if (level >= TraceSetting::Level()) try {\
		TraceFormat message(format); \
		message.Output(level, file, line) << args; \
		TraceClient::Record(message.String()); \
	} catch (const std::exception &ex) { \
		std::cerr << "[-]" << file << ": " << line << " !!! " << ex.what() << std::endl; \
	}

void Trace(TraceLevel level, const std::string &file, const int line, const std::string &format);

template<typename A1>
void Trace(TraceLevel level, const std::string &file, const int line, const std::string &format,
		const A1 &a1)
{
	MSVC_LOG_DI_TRACE_FACET_TEMPLATE_BODY(a1);
}

template<typename A1, typename A2>
void Trace(TraceLevel level, const std::string &file, const int line, const std::string &format,
		const A1 &a1, const A2 &a2)
{
	MSVC_LOG_DI_TRACE_FACET_TEMPLATE_BODY(a1 << a2);
}

template<typename A1, typename A2, typename A3>
void Trace(TraceLevel level, const std::string &file, const int line, const std::string &format,
		const A1 &a1, const A2 &a2, const A3 &a3)
{
	MSVC_LOG_DI_TRACE_FACET_TEMPLATE_BODY(a1 << a2 << a3);
}

template<typename A1, typename A2, typename A3, typename A4>
void Trace(TraceLevel level, const std::string &file, const int line, const std::string &format,
		const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4)
{
	MSVC_LOG_DI_TRACE_FACET_TEMPLATE_BODY(a1 << a2 << a3 << a4);
}

template<typename A1, typename A2, typename A3, typename A4, typename A5>
void Trace(TraceLevel level, const std::string &file, const int line, const std::string &format,
		const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5)
{
	MSVC_LOG_DI_TRACE_FACET_TEMPLATE_BODY(a1 << a2 << a3 << a4 << a5);
}

template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
void Trace(TraceLevel level, const std::string &file, const int line, const std::string &format,
		const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6)
{
	MSVC_LOG_DI_TRACE_FACET_TEMPLATE_BODY(a1 << a2 << a3 << a4 << a5 << a6);
}

template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
void Trace(TraceLevel level, const std::string &file, const int line, const std::string &format,
		const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7)
{
	MSVC_LOG_DI_TRACE_FACET_TEMPLATE_BODY(a1 << a2 << a3 << a4 << a5 << a6 << a7);
}

template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
void Trace(TraceLevel level, const std::string &file, const int line, const std::string &format,
		const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7, const A8 &a8)
{
	MSVC_LOG_DI_TRACE_FACET_TEMPLATE_BODY(a1 << a2 << a3 << a4 << a5 << a6 << a7 << a8);
}

template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
void Trace(TraceLevel level, const std::string &file, const int line, const std::string &format,
		const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7, const A8 &a8, const A9 &a9)
{
	MSVC_LOG_DI_TRACE_FACET_TEMPLATE_BODY(a1 << a2 << a3 << a4 << a5 << a6 << a7 << a8 << a9);
}

}}}

#endif //MSVC_LOG_DI_TRACE_FACET_H
