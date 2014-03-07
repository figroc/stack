#ifndef MSVC_LOG_LOG_FORMAT_H
#define MSVC_LOG_LOG_FORMAT_H

#include <string>
#include <sstream>
#include <boost/date_time.hpp>
#include "incl/util/util.h"

namespace msvc { namespace log { namespace _di {

using namespace msvc::util;

class LogFormat {
private:
	template<typename S, typename T>
	inline static void _output(S &buffer, const T &val) {
		buffer << val;
	};
	template<typename S>
	inline static void _output(S &buffer, const char &val) {
		if (val == ',')
			buffer << "\",\"";
		else if (val == '"')
			buffer << "\"\"\"\"";
		else
			buffer << val;
	}
	template<typename S>
	inline static void _output(S &buffer, const char *const &val) {
		_output(buffer, std::string(val));
	}
	template<typename S>
	inline static void _output(S &buffer, const std::string &val) {
		size_t c(StringFind(val, ',')), q(StringFind(val, '"'));
		if (q != std::string::npos)
			buffer << '"' << StringReplace(val, '"', "\"\"", q) << '"';
		else if (c != std::string::npos)
			buffer << '"' << val << '"';
		else
			buffer << val;
	};
	template<typename S>
	inline static void _output(S &buffer, const boost::gregorian::date &val) {
		buffer << boost::gregorian::to_iso_extended_string(val);
	};
	template<typename S>
	inline static void _output(S &buffer, const boost::posix_time::ptime &val) {
		buffer << boost::posix_time::to_iso_extended_string(val);
	};

private:
	std::ostringstream _buffer;

public:
	LogFormat() : _buffer() {
		_output(_buffer, boost::posix_time::microsec_clock::universal_time());
	};

public:
	template<typename T>
	inline LogFormat &operator<<(const T &val) {
		_output(_buffer << ',', val); return *this;
	}
	inline std::string String() {
		return _buffer.str();
	}

};

}}}

#endif
