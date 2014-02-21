#include <errno.h>
#include <exception>
#include <stdexcept>
#include <string>
#include <boost/lexical_cast.hpp>
#include "EPollWatcher.h"
#include "ThreadPool.h"
#include "log/log_export.h"

namespace msvc { namespace stack {

using namespace std;
using namespace boost;
using namespace msvc::log;

shared_ptr<PerfCounter> EPollWatcher::_iot;
atomic_uint EPollWatcher::_idx(0);

void EPollWatcher::Init()
{
	_iot = PerfManager::Fetch("epoll^thread");
}

void EPollWatcher::ThreadPoolWorker(const bool event, const int fd, const CtxEventCallback ctx)
{
	PerfCounting counting(_iot);
	if (ctx.callback().callable()) try {
		ctx.callback()(event, fd, ctx.state());
	} catch (const std::exception &e) {
		LC_TRACE_WARN("error invoking epoll callback: %s", e.what());
	}
}

bool EPollWatcher::WatchEvent(const uint32_t event, const int fd,
		const FnEventCallback &callback, const StatePtr &state)
{
	pair<int, CtxEventCallback> *ctx = new pair<int, CtxEventCallback>(
			fd, CtxEventCallback(callback, state));

	struct epoll_event eve = {0};
	eve.events = EPOLL_DEFAULT | event;
	eve.data.fd = fd;
	eve.data.ptr = ctx;

	// the only viable usage is as the class Socket 
	//  or else there will be memory leaking all the time
	if (0 == ::epoll_ctl(_epoll, EPOLL_CTL_MOD, fd, &eve)) {
		_perf->Increment();
		return true;
	}
	if (ENOENT == errno) {
		if (0 == ::epoll_ctl(_epoll, EPOLL_CTL_ADD, fd, &eve)) {
			_perf->Increment();
			return true;
		}
	}

	delete ctx;
	return false;
}

void EPollWatcher::WaitOnEvent()
{
	static const int MAX_EVENT = 64;
	struct epoll_event events[MAX_EVENT] = {0};

	// DO NOT use 'this' pointer in the loop,
	//   it's designed for running on a separated thread,
	//   the 'this' object maybe get deleted during the looping
	const int epoll = _epoll;
	const shared_ptr<PerfCounter> perf = _perf;

	// maybe LEAKING:
	//   every events MUST get called on closing epoll fd,
	//   OR ELSE event.data.ptr will be lost forever,
	//   that also means we cannot just jump out of the looping
	while (true) {
		const int count = ::epoll_wait(epoll, events, MAX_EVENT, -1);
		if (count < 0) {
			const int err = errno;
			if (EINTR == err || EAGAIN == err)
				continue;
			LC_TRACE_FATAL("failed on epoll_wait: %s", strerror(err));
			break;
		}

		for (int i = 0; i < count; ++i) {
			scoped_ptr< pair<int, CtxEventCallback> >
				ctx(reinterpret_cast<pair<int, CtxEventCallback> *>(events[i].data.ptr));

			const bool error = events[i].events & EPOLL_ERROR;
			bool remove = error;
			if (ctx) {
				perf->Decrement();
				if (ctx->second.callback().callable()) try {
					ThreadPool::ScheduleIO(boost::bind(&EPollWatcher::ThreadPoolWorker,
							!error, ctx->first, ctx->second));
				} catch (const std::exception &e) {
					LC_TRACE_WARN("error scheduling epoll callback: %s", e.what());
				} else {
					remove = true;
				}
			} else {
				// events[i].data.fd is not the one EPOLL_CTL_ADD
				//   we have no way to remove it here
				remove = false;
			}
			if (remove) {
				::epoll_ctl(epoll, EPOLL_CTL_DEL, ctx->first, NULL);
			}
		}
	}
}

void EPollWatcher::Initialize()
{
	_epoll = ::epoll_create(102400);
	if (-1 == _epoll)
		throw runtime_error(string("failed to create epoll: %s") + ::strerror(errno));
	_perf = PerfManager::Fetch(string("epoll^event^")
				+ lexical_cast<string>(_idx.fetch_add(1, memory_order_relaxed))
			);
}

void EPollWatcher::TearDown()
{
	if (-1 != _epoll) {
		LC_TRACE_FATAL("closing epoll fd: %d(%ld)", _epoll, _perf->Current());
		::close(_epoll);
	}
	_epoll = -1;
}

}}
