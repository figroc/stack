#include <algorithm>
#include <boost/thread/thread.hpp>
#include "EPollManager.h"

namespace msvc { namespace stack {

using namespace std;
using namespace boost;

const size_t EPollManager::EPM_WATCHERS = max<size_t>(2, thread::hardware_concurrency());
const size_t EPollManager::EPM_OFFSET[2] = { 0, 1 };
const uint32_t EPollManager::EPM_EVENT[2] = { EPOLLIN, EPOLLOUT };
atomic_bool EPollManager::_init(false);
scoped_array<EPollWatcher> EPollManager::_watcher;

void EPollManager::Init()
{
	if (!_init.exchange(true, memory_order_seq_cst)) {
		EPollWatcher::Init();
		_watcher.reset(new EPollWatcher[EPM_WATCHERS]);
		for (int i = 0; i < EPM_WATCHERS; ++i) {
			thread(bind(&EPollWatcher::WaitOnEvent, _watcher.get() + i)).detach();
		}
	}
}

void EPollManager::Stop()
{
	if (_init.exchange(false, memory_order_seq_cst)) {
		_watcher.reset();
	}
}

bool EPollManager::Watch(const EPollMode mode, const int fd,
		const FnEventCallback &callback, const StatePtr &state)
{
	const size_t index = (EPM_OFFSET[mode] + (size_t)fd) % EPM_WATCHERS;
	return _watcher[index].WatchEvent(EPM_EVENT[mode], fd, callback, state);
}

}}
