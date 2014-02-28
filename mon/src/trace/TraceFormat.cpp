#include "TraceFormat.h"
#include <iostream>
#include <boost/date_time.hpp>
#include "incl/util/util.h"

namespace msvc { namespace log { namespace _di {

using namespace std;
using namespace msvc::util;

const string TraceFormat::LEVEL_STR[] = { "-", "DEBUG", "INFO", "WARN", "ERROR", "FATAL" };
const string TraceFormat::HOST_NAME = GetHostName();
const int TraceFormat::PROCESS_ID = GetProcessId();

TraceFormat &TraceFormat::Output(const TraceLevel level, const string &file, const int line)
{
	int i = level / 10;
	if (i < 0 || i > sizeof(LEVEL_STR))
		i = 0;
	_buffer << "***\n[" << boost::posix_time::to_iso_extended_string(
				boost::posix_time::microsec_clock::universal_time())
			<< "] " << LEVEL_STR[i]
			<< " <" << PROCESS_ID << '.' << GetThreadId() << ">@" << HOST_NAME
			<< " \n+++ " << file << ": " << line << " \n";
	return *this;
}

string TraceFormat::String()
{
	if (_pos < _format.size()) {
		_buffer << _format.substr(_pos);
		_pos = _format.size();
	}
	return _buffer.str();
}

void TraceFormat::PrepareVal()
{
	static const std::string PLACE_HOLDER = "%%";
	if (_pos < _format.size()) {
		int i = _format.find(PLACE_HOLDER, _pos);
		if (i == std::string::npos) {
			_buffer << _format.substr(_pos);
			_pos = _format.size();
		} else {
			_buffer << _format.substr(_pos, i - _pos);
			_pos = i + PLACE_HOLDER.size();
		}
	}
}

}}}
