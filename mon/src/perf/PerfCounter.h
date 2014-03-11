#ifndef PERF_PERF_COUNTER_H
#define PERF_PERF_COUNTER_H

#include <utility>
#include <boost/atomic.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/chrono.hpp>
#include "incl/util/util.h"

namespace msvc { namespace perf {

using namespace msvc::util;

class PerfCounter {
	friend class PerfManager;

public:
	static const int HINT_NONE = 0;
	static const int HINT_INC = 1;
	static const int HINT_DEC = 2;
	static const int HINT_COUNT = 3;
	static const int HINT_TIME = 4;
	static const int HINT_HIT = 8;
	static const int HINT_ALL = 15;

private:
	typedef boost::atomic_intmax_t atomic_count_t;
	typedef std::pair<boost::atomic_uintmax_t, boost::atomic_uintmax_t> atomic_rate_t;
	typedef std::pair<boost::atomic_uintmax_t, boost::atomic_uintmax_t> atomic_avg_t;
	typedef Atomic<uintmax_t>::opr atomic_op_f;

	static const size_t RATE_DURATION = 5;
	static const atomic_op_f SET_ADD[];

private:
	static void IncRate(atomic_rate_t (&array)[RATE_DURATION], const size_t count);
	static double AvgRate(const atomic_rate_t (&array)[RATE_DURATION]);

private:
	std::string _name;
	int _hint;
	atomic_count_t _current;
	atomic_rate_t _rateInc[RATE_DURATION];
	atomic_rate_t _rateDec[RATE_DURATION];
	atomic_avg_t _timing;
	atomic_avg_t _hit;

private:
	PerfCounter(const std::string &name) : _name(name), _hint(HINT_NONE),
		_current(), _rateInc(), _rateDec(), _timing(), _hit() { };
public:
	virtual ~PerfCounter() { };

public:
	inline std::string name() const { return _name; }
	inline int hint() const { return _hint; }

	void Increment(const size_t count = 1);
	void Decrement(const size_t count = 1);
	void Timing(const boost::chrono::milliseconds &time);
	void HitOrMiss(const bool hit);

	inline long long Current() const { return (long long)_current.load(boost::memory_order_relaxed); };

	inline double RateOfInc() const { return AvgRate(_rateInc); };
	inline double RateOfDec() const { return -AvgRate(_rateDec); };

	inline double AvgTime() const { return _timing.first.load(boost::memory_order_relaxed); };
	inline double HitRatio() const { return 100.0 * _hit.first.load(boost::memory_order_relaxed) / _hit.second.load(boost::memory_order_relaxed); };
};

}}

#endif /* PERF_PERF_COUNTER_H */
