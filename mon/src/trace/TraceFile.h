#ifndef MSVC_LOG_TRACE_FILE_H
#define MSVC_LOG_TRACE_FILE_H

#include <string>
#include <fstream>
#include <boost/date_time.hpp>

namespace msvc { namespace log { namespace _di {

class TraceFile {
private:
	typedef boost::posix_time::time_duration file_hour_t;
	typedef boost::posix_time::ptime file_time_t;
	typedef boost::posix_time::microsec_clock file_clock_t;

private:
	std::string _path;
	std::string _name;
	std::string _fext;

	bool _switch;
	std::ofstream _file;
	file_time_t _time;

public:
	TraceFile(const std::string &name, const std::string &ext) :
		_path(), _name(name), _fext(ext),
		_switch(true), _file(), _time() { };

public:
	void Switch(const std::string &path);

	inline void Write(const std::string &message) {
		TestSwitch(); _file << message << std::endl;
	}

private:
	void TestSwitch();

	inline file_time_t _file_time_now() {
		file_time_t now = file_clock_t::universal_time();
		return file_time_t(now.date(), file_hour_t(now.time_of_day().hours(), 0, 0));
	}
	inline std::string _file_name_compose() {
		return _path + _name + '_' + boost::posix_time::to_iso_string(_time).substr(0, 11) + _fext;
	}
};

}}}

#endif
