#ifndef STACK_THREAD_POOL_H
#define STACK_THREAD_POOL_H

#include <boost/threadpool.hpp>

using namespace boost::threadpool;

namespace msvc { namespace stack {

class ThreadPool {
public:
	typedef pool::task_type task_type;

private:
	static pool _io;
	static pool _worker;

public:
	inline static bool ScheduleIO(const task_type &task) { return _io.schedule(task); }
	inline static bool ScheduleWorker(const task_type &task) { return _worker.schedule(task); }

private:
	ThreadPool();
};

} }

#endif // STACK_THREAD_POOL_H
