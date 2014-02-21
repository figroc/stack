#ifndef MSVC_LOG_TRACE_FORMAT_H
#define MSVC_LOG_TRACE_FORMAT_H

#include "TraceType.h"
#include <string>
#include <sstream>
#include <exception>
#include <boost/type_traits.hpp>

namespace msvc { namespace log {

class TraceFormat {
private:
	template<typename T, bool X> struct _helper {
	inline static void output(std::stringstream &buffer, const T &val) {
		buffer << val;
	};};
	template<typename T> struct _helper<T, true> {
	inline static void output(std::stringstream &buffer, const T &val) {
		buffer << "\n!!! " << val.what();
	};};

private:
	static const std::string LEVEL_STR[];
	static const std::string HOST_NAME;
	static const int PROCESS_ID;

private:
	int _pos;
	std::string _format;
	std::stringstream _buffer;

public:
	explicit TraceFormat(const std::string &format)
		: _pos(0), _format(format), _buffer() { };

public:
	TraceFormat &Output(const TraceLevel level, const std::string &file, const int line);
	template<typename T>
	inline TraceFormat &operator<<(const T &val) {
		PrepareVal(); _helper<T, boost::is_base_of<std::exception, T>::value>::output(_buffer, val); return *this;
	};
	std::string String();

private:
	void PrepareVal();

};

}}

#endif //MSVC_LOG_TRACE_FORMAT_H
