#include "CfgLoader.h"
#include "SvcLocator.h"
#include "SvcSetting.h"
#include <sys/stat.h>
#include <boost/chrono.hpp>
#include <boost/program_options.hpp>

namespace msvc { namespace cfg {

using namespace std;

boost::atomic_bool CfgLoader::_init(false);
boost::mutex CfgLoader::_lock;
map< string, boost::shared_ptr<Conf> > CfgLoader::_conf;

void CfgLoader::Init(const SvcRole &role)
{
	if (!_init.exchange(true)) {
		boost::thread(FileWatcher).detach();
		SvcLocator::Init();
		SvcSetting::Init(role);
	}
}

Conf *CfgLoader::GetConf(const string &name)
{
	boost::mutex::scoped_lock lock(_lock);
	map< string, boost::shared_ptr<Conf> >::iterator
		it = _conf.find(name);
	if (_conf.end() == it) {
		boost::shared_ptr<Conf> conf(CreateConf(name));
		it = _conf.insert(make_pair(name, conf)).first;
	}
	return it->second.get();
}

void CfgLoader::FileWatcher()
{
#ifdef LOCAL_DEBUG
	static const boost::chrono::seconds interval(5);
#else
	static const boost::chrono::minutes interval(5);
#endif
	while (true) try {
		boost::this_thread::sleep_for(interval);
		{
			boost::mutex::scoped_lock lock(_lock);
			for (map< string, boost::shared_ptr<Conf> >::iterator
				it = _conf.begin(); it != _conf.end(); ++it) try {
				const boost::shared_ptr<Conf> &conf = it->second;
				const time_t filetime = GetFileTime(conf->GetFilename());
				if (conf->IsChanged(filetime)) {
					auto_ptr<conf_value_map>
						property = LoadFromFile(conf->GetFilename());
					conf->Update(property, filetime);
				}
			} catch (const exception &ex) {
				cerr << "[ConfLoader::FileWatcher] " << it->first
				     << ": " << ex.what() << endl;
			}
		}
	} catch (const exception &ex) {
		cerr << "[ConfLoader::FileWatcher]: " << ex.what() << endl;
	} catch (...) {
		cerr << "[ConfLoader::FileWatcher] DEATH!" << endl;
		throw;
	}
}

string CfgLoader::GetFileName(const string &name)
{
	return name + ".conf";
}

time_t CfgLoader::GetFileTime(const string &name)
{
	try {
		struct stat buf;
		::stat(name.c_str(), &buf);
		return buf.st_mtime;
	} catch (const exception &ex) {
		cerr << "[ConfLoader::GetFileTime] exception for " << name
		     << ": " << ex.what() << endl;
	}
	return 0;
}

auto_ptr<Conf> CfgLoader::CreateConf(const string &name)
{
	const string filename = GetFileName(name);
	const time_t filetime = GetFileTime(filename);
	auto_ptr<conf_value_map> property = LoadFromFile(filename);
	return auto_ptr<Conf>(new Conf(filename, filetime, property));
}

auto_ptr<conf_value_map> CfgLoader::LoadFromFile(const string &name)
{
	static const boost::program_options::options_description NO_OPT_DES;
	try {
		boost::program_options::basic_parsed_options<char>
			opts = boost::program_options::parse_config_file<char>(name.c_str(), NO_OPT_DES, true);
		auto_ptr<conf_value_map> conf(new conf_value_map());
		for (int i = 0; i < opts.options.size(); ++i) {
			conf_value_map::iterator it = conf->find(opts.options[i].string_key);
			if (it == conf->end()) {
				it = conf->insert(make_pair(opts.options[i].string_key, conf_value_list())).first;
			}
			it->second.values<string>().insert(it->second.values<string>().end(),
					opts.options[i].value.begin(), opts.options[i].value.end());
		}

		for (conf_value_map::iterator it = conf->begin(); it != conf->end(); ++it) {
			bool cast_bool = true;
			bool cast_long = true;
			conf_value_list &cvl(it->second);
			for (int i = 0; i < cvl.values<string>().size(); ++i) {
				if (cast_bool) {
					bool r;
					cast_bool = StringCastTry(cvl.values<string>()[i], r);
					if (cast_bool) {
						cvl.values<bool>().push_back(r);
					} else {
						cvl.values<bool>().clear();
					}
				}
				if (cast_long) {
					long r;
					cast_long = StringCastTry(cvl.values<string>()[i], r);
					if (cast_long) {
						cvl.values<long>().push_back(r);
					} else {
						cvl.values<long>().clear();
					}
				}
			}
		}
		return conf;
	} catch (const exception &ex) {
		cerr << "[ConfLoader::LoadFromFile] " << name
		     << ": " << ex.what() << endl;
	}
	return auto_ptr<conf_value_map>(0);
}

}}
