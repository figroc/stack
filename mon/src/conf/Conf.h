#ifndef CFG_CONF_H
#define CFG_CONF_H

#include <time.h>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "incl/util/util.h"

namespace msvc { namespace cfg {

using namespace msvc::util;

struct conf_value_list {
private:
	std::vector<bool> _booleans;
	std::vector<long> _integers;
	std::vector<std::string> _strings;
public:
	template<typename T> inline const std::vector<T> &values() const;
	template<typename T> inline std::vector<T> &values();
};

template<> inline const std::vector<bool> &conf_value_list::values<bool>() const { return _booleans; }
template<> inline const std::vector<long> &conf_value_list::values<long>() const { return _integers; }
template<> inline const std::vector<std::string> &conf_value_list::values<std::string>() const { return _strings; }
template<> inline std::vector<bool> &conf_value_list::values<bool>() { return _booleans; }
template<> inline std::vector<long> &conf_value_list::values<long>() { return _integers; }
template<> inline std::vector<std::string> &conf_value_list::values<std::string>() { return _strings; }

typedef std::map<std::string, conf_value_list> conf_value_map;

class Conf {
	friend class CfgLoader;

private:
	std::string _filename;
	time_t _filetime;

	mutable boost::mutex _lockConf;
	boost::shared_ptr<const conf_value_map> _conf;

	mutable boost::mutex _lockWatcher;
	std::vector< SafeFunctor<void (Conf *)> > _watcher;

private:
	Conf(const std::string &filename, const time_t filetime, std::auto_ptr<conf_value_map> &conf) :
		_filename(filename), _filetime(filetime), _lockConf(), _conf(conf),	_lockWatcher(), _watcher() {
		if (!_conf) _filetime = 0;
	}
public:
	virtual ~Conf() { }

public:
	inline void WatchChange(void (*const callback)(Conf *conf)) { RegisterWatcher(make_functor(callback)); }
	template<typename T> // caller must keep owner alive, we take weak_ptr from shared_ptr
	inline void WatchChange(const boost::shared_ptr<T> &owner, void (T::*const callback)(Conf *conf)) { RegisterWatcher(make_functor(callback, owner, false)); }

	inline boost::shared_ptr<const conf_value_map> GetAll() const {
		boost::mutex::scoped_lock lock(_lockConf); return _conf;
	}

	template<typename T>
	T GetValue(const std::string &name, const T &def = T()) const {
		const boost::shared_ptr<const conf_value_map> conf = GetAll();
		if (!conf)
			return def;
		conf_value_map::const_iterator it = conf->find(name);
		if (it == conf->end())
			return def;
		if (it->second.values<T>().empty())
			return def;
		return it->second.values<T>().front();
	}

	template<typename T>
	std::vector<T> GetValues(const std::string &name) const {
		const boost::shared_ptr<const conf_value_map> conf = GetAll();
		if (!conf)
			return std::vector<T>();
		conf_value_map::const_iterator it = conf->find(name);
		if (it == conf->end())
			return std::vector<T>();
		return it->second.values<T>();
	}

private:
	inline const std::string &GetFilename() const { return _filename; }
	inline bool IsChanged(const time_t filetime) const { return 0 != filetime && _filetime != filetime; }

	void Update(std::auto_ptr<conf_value_map> &conf, const time_t filetime);

	void RegisterWatcher(const SafeFunctor<void (Conf *)> &watcher);
};

}}

#endif /* CFG_CONF_H */
