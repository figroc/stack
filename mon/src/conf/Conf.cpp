#include "Conf.h"

namespace msvc { namespace cfg {

using namespace std;
using namespace msvc::util;

void Conf::Update(auto_ptr<conf_value_map> &conf, const time_t filetime)
{
	if (!conf.get()) return;

	{
		boost::mutex::scoped_lock lock(_lockConf);
		_conf = conf;
		_filetime = filetime;
	}

	vector< SafeFunctor<void (Conf *)> > callback;
	{
		boost::mutex::scoped_lock lock(_lockWatcher);
		for (vector< SafeFunctor<void (Conf *)> >::iterator
			it = _watcher.begin(); it != _watcher.end(); ++it) {
			if (it->callable())
				callback.push_back(*it);
		}
		if (callback.size() != _watcher.size())
			_watcher = callback;
	}

	for (vector< SafeFunctor<void (Conf *)> >::iterator
		it = callback.begin(); it != callback.end(); ++it) {
		const ScopedPtr ptr = it->callable();
		if (ptr) try {
			it->operator()(this);
		} catch (const std::exception &ex) {
			cerr << "[Conf::Update] error invoking watcher: " << ex.what() << endl;
		}
	}
}

void Conf::RegisterWatcher(const SafeFunctor<void (Conf *)> &watcher)
{
	if (watcher.callable()) {
		boost::mutex::scoped_lock lock(_lockWatcher);
		_watcher.push_back(watcher);
	}
}

}}
