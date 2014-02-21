#ifndef PERF_PERF_MANAGER_H
#define PERF_PERF_MANAGER_H

#include <map>
#include <vector>
#include <boost/smart_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include "PerfCounter.h"

namespace msvc { namespace perf {

class PerfManager {
public:
	static const std::string SEPARATOR;

private:
	static boost::mutex _lock;
	static std::map< std::string, boost::shared_ptr<PerfCounter> > _perf;
	static std::map< std::string, boost::shared_ptr< std::vector<std::string> > > _tree;

public:
	// name is split by '^' for structural display
	static PerfCounter *Fetch(const std::string &name);
	static std::vector<PerfCounter *> Query(const std::string &name);

private:
	static void UpdateTree(const std::string &name);
	static void UpdateTree(const std::string &prefix, const std::string &name);

private:
	PerfManager();
};

}}

#endif /* PERF_PERF_MANAGER_H */
