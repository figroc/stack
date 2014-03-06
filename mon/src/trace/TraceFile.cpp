#include "TraceFile.h"
#include <boost/filesystem.hpp>

namespace msvc { namespace log { namespace _di {

using namespace std;

void TraceFile::Switch(const string &path)
{
	if (path.empty() || path == _path)
		return;
	if (path[path.size() - 1] != '/') {
		string tpath = path + '/';
		if (tpath == _path)
			return;
		_path = tpath;
	} else {
		_path = path;
	}
	_switch = true;
}

void TraceFile::TestSwitch()
{
	file_time_t ftt(boost::posix_time::not_a_date_time);
	if (_switch || (ftt = _file_time_now()) != _time) {
		if (!_path.empty()) {
			boost::system::error_code err;
			boost::filesystem::create_directories(_path, err);
		}
		_time = ftt.is_not_a_date_time() ? _file_time_now() : ftt;
		string filename = _file_name_compose();
		if (_file.is_open())
			_file.close();
		_file.open(filename.c_str(), ios::binary | ios::out | ios::app);
		_switch = false;
	}
}

}}}
