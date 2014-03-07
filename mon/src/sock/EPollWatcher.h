#ifndef MSVC_EPOLL_WATCHER_H
#define MSVC_EPOLL_WATCHER_H

#include <stdint.h>
#include <sys/epoll.h>
#include <boost/atomic.hpp>
#include <boost/smart_ptr.hpp>
#include "EventArgs.h"
#include "incl/util/util.h"
#include "incl/perf/perf.h"

namespace msvc { namespace sock {

using namespace msvc::util;
using namespace msvc::perf;

class EPollWatcher {
public:
	typedef SafeFunctor<void (bool, int, const StatePtr &)> FnEventCallback;

private:
	static const uint32_t EPOLL_ERROR = EPOLLHUP | EPOLLERR;
	static const uint32_t EPOLL_DEFAULT = EPOLLET | EPOLLONESHOT | EPOLL_ERROR;

	static PerfCounter *_iot;
	static boost::atomic_uint _idx;

public:
	static void Init();
private:
	typedef CallbackContext<FnEventCallback> CtxEventCallback;
	static void ThreadPoolWorker(const bool event, const int fd, const CtxEventCallback ctx);

private:
	int _epoll;
	PerfCounter *_perf;

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
