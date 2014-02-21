#ifndef MSVC_EPOLL_WATCHER_H
#define MSVC_EPOLL_WATCHER_H

#include <stdint.h>
#include <sys/epoll.h>
#include <boost/atomic.hpp>
#include <boost/smart_ptr.hpp>
#include "EventArgs.h"
#include "util/util_export.h"
#include "perf/perf_export.h"

namespace msvc { namespace stack {

using namespace msvc::util;
using namespace msvc::perf;

class EPollWatcher {
public:
	typedef SafeFunctor<void (bool, int, const StatePtr &)> FnEventCallback;

private:
	static const uint32_t EPOLL_ERROR = EPOLLHUP | EPOLLERR;
	static const uint32_t EPOLL_DEFAULT = EPOLLET | EPOLLONESHOT | EPOLL_ERROR;

	static boost::shared_ptr<PerfCounter> _iot;
	static boost::atomic_uint _idx;

public:
	static void Init();
private:
	typedef CallbackContext<FnEventCallback> CtxEventCallback;
	static void ThreadPoolWorker(const bool event, const int fd, const CtxEventCallback ctx);

private:
	int _epoll;
	boost::shared_ptr<PerfCounter> _perf;

public:
	EPollWatcher() : _epoll(-1) { Initialize(); }
	~EPollWatcher() { TearDown(); }
private:
	EPollWatcher(const EPollWatcher &watcher);
	EPollWatcher &operator=(const EPollWatcher &watcher);

public:
	void InitPerf(const int instance);
	bool WatchEvent(const uint32_t event, const int fd,
			const FnEventCallback &callback, const StatePtr &state);
	void WaitOnEvent();

private:
	void Initialize();
	void TearDown();
};

}}

#endif /* MSVC_EPOLL_WATCHER_H */
