#ifndef DB_DB_PERF_COUNTER_H
#define DB_DB_PERF_COUNTER_H

#include <string>
#include <utility>
#include <map>
#include <memory>
#include <boost/thread/mutex.hpp>

#include "incl/perf/perf.h"

namespace msvc { namespace db {

using namespace msvc::perf;

class DbPerfC {
public:
	class Pool {
	private:
		static PerfCounter *_free;
		static PerfCounter *_used;

	public:
		inline static void Init() {
			_free = PerfManager::Fetch("db^pool^conn^free");
			_used = PerfManager::Fetch("db^pool^conn^used");
		}
		inline static PerfCounter *free() { return _free; }
		inline static PerfCounter *used() { return _used; }

	private:
		Pool();
	};

private:
	static boost::mutex _lock;
	static std::map<std::string, PerfCounter *> _perf;

public:
	inline static void Init() { Pool::Init(); }

	inline static PerfCounter *Fetch(const std::string &db, const char op) {
		const std::string name = "db^role^" + db + '^' + op;
		boost::mutex::scoped_lock lock(_lock);
		std::map<std::string, PerfCounter *>::iterator
			it = _perf.find(name);
		if (_perf.end() == it) {
			it = _perf.insert(make_pair(name, PerfManager::Fetch(name))).first;
		}
		return it->second;
	}

private:
	DbPerfC();
};

}}

#endif /* DB_DB_PERF_COUNTER_H */
