#ifndef STACK_EVENT_ARGS_H
#define STACK_EVENT_ARGS_H

#include <boost/smart_ptr.hpp>
#include <boost/pointer_cast.hpp>

namespace msvc { namespace stack {

typedef boost::shared_ptr<void> StatePtr;

template<typename F>
class CallbackContext {
private:
	F _callback;
	StatePtr _state;

public:
	CallbackContext(
			const F &callback = F(),
			const StatePtr &state = StatePtr()
		) :	_callback(callback), _state(state) { };
	virtual ~CallbackContext() { };

public:
	inline const F &callback() const {
		return _callback;
	};
	inline const StatePtr &state() const {
		return _state;
	};

};

template<typename T>
class IoEventArgs {
private:
	int _error;
	T _data;

public:
	IoEventArgs(const T &data) : _error(0), _data(data) { };
	IoEventArgs(int error) : _error(error), _data() { };
	virtual ~IoEventArgs() { };

public:
	inline int GetError() const { return _error; };
	inline T GetData() const { return _data; };

};

} }

#endif // STACK_EVENT_ARGS_H
