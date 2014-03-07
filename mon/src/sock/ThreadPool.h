#ifndef STACK_THREAD_POOL_H
#define STACK_THREAD_POOL_H

#define TIME_UTC 1
#include <boost/threadpool.hpp>

namespace msvc { namespace sock {

class ThreadPool {
public:
	typedef boost::threadpool::pool::task_type task_type;

private:
	static boost::threadpool::pool _io;
	static boost::threadpool::pool _worker;

public:
	inline static bool ScheduleIO(const task_type &task) { return _io.schedule(task); }
	inline static bool ScheduleWorker(const task_type &task) { return _worker.schedule(task); }

private:
	ThreadPool();
};

} }

#endif // STACK_THREAD_POOL_H
