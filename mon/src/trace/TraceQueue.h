#ifndef MSVC_LOG_TRACE_QUEUE_H
#define MSVC_LOG_TRACE_QUEUE_H

#include <string>
#include <memory>
#include <boost/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/lambda/bind.hpp>

namespace msvc { namespace log { namespace _di {

template<typename T>
class TraceQueue {
private:
	boost::lockfree::queue<T *> _buffer;

public:
	TraceQueue() : _buffer(10000) { };
	~TraceQueue() {
		_buffer.consume_all(
			boost::lambda::bind(
					boost::lambda::delete_ptr(),
					boost::lambda::_1));
	};

public:
	inline void Push(const T &message) {
		T *str = new T(message);
		if (!_buffer.push(str))
			delete str;
	};
	inline std::auto_ptr<T> Pop() {
		T *str = 0;
		_buffer.pop(str);
		return std::auto_ptr<T>(str);
	};
};

}}}

#endif
