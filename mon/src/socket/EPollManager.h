#ifndef MSVC_EPOLL_MANAGER_H
#define MSVC_EPOLL_MANAGER_H

#include <stdint.h>
#include <utility>
#include <boost/atomic.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/scoped_array.hpp>
#include "EPollWatcher.h"
#include "util/util_export.h"

namespace msvc { namespace stack {

using namespace msvc::util;

enum EPollMode { EPM_READ = 0, EPM_WRITE = 1 };

class EPollManager {
public:
	typedef SafeFunctor<void (bool, int, const StatePtr &)> FnEventCallback;

private:
	static const size_t EPM_WATCHERS;
	static const size_t EPM_OFFSET[2];
	static const uint32_t EPM_EVENT[2];

	static boost::atomic_bool _init;
	static boost::scoped_array<EPollWatcher> _watcher;

public:
	static void Init();
	static void Stop();

	static bool Watch(const EPollMode mode, const int fd,
			const FnEventCallback &callback, const StatePtr &state);

private:
	EPollManager();
};

}}

#endif /* MSVC_EPOLL_MANAGER_H */
