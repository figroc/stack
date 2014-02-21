#ifndef UTIL_WAITABLE_EVENT_H
#define UTIL_WAITABLE_EVENT_H

#include <boost/atomic.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

namespace msvc { namespace util {

template<typename Signaler>
class WaitableEvent {
private:
	boost::condition_variable _wait;
	boost::mutex _lock;
	boost::atomic_bool _signal;

public:
	WaitableEvent(const bool signal = false)
		: _wait(), _lock(), _signal(signal) { };
	virtual ~WaitableEvent() { };

public:
	inline void Wait() {
		boost::unique_lock<boost::mutex> lock(_lock);
		while (!Signaler::Check(_signal)) _wait.wait(lock);
	}
	inline void Set() {
		{ boost::lock_guard<boost::mutex> lock(_lock); _signal = true; }
		Signaler::Notify(_wait);
	}
	inline void Reset() {
		boost::lock_guard<boost::mutex> lock(_lock); _signal = false;
	}

public: // for unit test only
	inline bool __internal_check_signal_status__() { return _signal; }
};

class ManualResetSignaler {
public:
	inline static bool Check(boost::atomic_bool &signal) { return signal; };
	inline static void Notify(boost::condition_variable &wait) { wait.notify_all(); }
};

class AutoResetSignaler {
public:
	inline static bool Check(boost::atomic_bool &signal) { return signal.exchange(false, boost::memory_order_seq_cst); }
	inline static void Notify(boost::condition_variable &wait) { wait.notify_one(); }
};

typedef WaitableEvent<ManualResetSignaler> ManualResetEvent;
typedef WaitableEvent<AutoResetSignaler> AutoResetEvent;

}}


#endif /* UTIL_WAITABLE_EVENT_H */
