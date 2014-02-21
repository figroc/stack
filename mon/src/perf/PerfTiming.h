#ifndef PERF_PERF_TIMING_H
#define PERF_PERF_TIMING_H

#include <boost/chrono.hpp>
#include <boost/smart_ptr.hpp>
#include "PerfCounter.h"

namespace msvc { namespace perf {

class PerfTiming {
private:
	PerfCounter *_target;
	boost::chrono::steady_clock::time_point _start;

public:
	PerfTiming(PerfCounter *const target) : _target(target) { Start(); };
	virtual ~PerfTiming() {	Done();	};
private:
	PerfTiming(const PerfTiming &);
	PerfTiming &operator=(const PerfTiming &);

public:
	inline void Start() { _start = boost::chrono::steady_clock::now(); };
	inline void Done(const bool release = true) { if (_target) {
		_target->Timing(boost::chrono::duration_cast<boost::chrono::milliseconds>(
				boost::chrono::steady_clock::now() - _start));
		if (release) _target = 0;
	}};
};

}}

#endif /* PERF_PERF_TIMING_H */
