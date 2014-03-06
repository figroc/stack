#ifndef MSVC_LOG_TRACE_FORMAT_H
#define MSVC_LOG_TRACE_FORMAT_H

#include "TraceType.h"
#include <string>
#include <sstream>
#include <exception>
#include <boost/type_traits.hpp>

namespace msvc { namespace log { namespace _di {

class TraceFormat {
private:
	template<bool> struct _helper {	};

private:
	static const std::string LEVEL_STR[];
	static const std::string HOST_NAME;
	static const int PROCESS_ID;

private:
	int _pos;
	std::string _format;
	std::ostringstream _buffer;

public:
	explicit TraceFormat(const std::string &format)
		: _pos(0), _format(format), _buffer() { };

public:
	TraceFormat &Output(const TraceLevel level, const std::string &file, const int line);
	template<typename T>
	inline TraceFormat &operator<<(const T &val) {
		typedef _helper<boost::is_base_of<std::exception, T>::value> _stream_t;
		PrepareVal(); _stream_t::output(_buffer, val); return *this;
	};
	std::string String();

private:
	void PrepareVal();

};

template<> struct TraceFormat::_helper<false> { template<typename S, typename T>
inline static void output(S &buffer, const T &val) {
	buffer << val;
};};

template<> struct TraceFormat::_helper<true> { template<typename S, typename T>
inline static void output(S &buffer, const T &val) {
	buffer << "\n!!! " << val.what();
};};

}}}

#endif //MSVC_LOG_TRACE_FORMAT_H
