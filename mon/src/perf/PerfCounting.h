#ifndef PERF_PERF_COUNTING_H
#define PERF_PERF_COUNTING_H

#include "PerfCounter.h"

namespace msvc { namespace perf {

class PerfCounting {
private:
	PerfCounter *_target;

public:
	PerfCounting(PerfCounter *const target, const bool autoStart = true)
		: _target(target) { if (autoStart) Start(); };
	virtual ~PerfCounting() { Done(); };
private:
	PerfCounting(const PerfCounting &);
	PerfCounting &operator=(const PerfCounting &);

public:
	inline void Start() { if (_target) _target->Increment(); };
	inline void Done(const bool release = true) { if (_target) {
		_target->Decrement(); if (release) _target = 0;
	}};
};

}}

#endif /* PERF_PERF_COUNTING_H */
