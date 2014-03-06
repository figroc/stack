#include "PerfCounter.h"
#include "incl/util/util.h"

namespace msvc { namespace perf {

using namespace std;
using namespace msvc::util;

const PerfCounter::atomic_op_f PerfCounter::SET_ADD[] = {
		&Atomic<uintmax_t>::Set, &Atomic<uintmax_t>::Add
	};

void PerfCounter::IncRate(atomic_rate_t (&array)[RATE_DURATION], const size_t count)
{
	const uintmax_t now = time(NULL);
	atomic_rate_t &rate(array[now % RATE_DURATION]);
	atomic_op_f opr = SET_ADD[now == rate.first.exchange(now, boost::memory_order_release)];
	opr(rate.second, count, boost::memory_order_consume);
}

double PerfCounter::AvgRate(const atomic_rate_t (&array)[RATE_DURATION])
{
	const time_t now = time(NULL);
	size_t total(0), abase(0);
	for (int i = 0; i < RATE_DURATION; ++i) {
		const atomic_rate_t &rate(array[i]);
		if (now - rate.first.load(boost::memory_order_relaxed) <= RATE_DURATION) {
			total += rate.second.load(boost::memory_order_relaxed);
			++abase;
		}
	}
	if (0 == abase)
		return 0.0;
	return (double)total / (double)abase;
}

void PerfCounter::Increment(const size_t count)
{
	_hint |= HINT_INC;
	_current.fetch_add(count, boost::memory_order_relaxed);
	IncRate(_rateInc, count);
}

void PerfCounter::Decrement(const size_t count)
{
	_hint |= HINT_DEC;
	_current.fetch_sub(count, boost::memory_order_relaxed);
	IncRate(_rateDec, count);
}

void PerfCounter::Timing(const boost::chrono::milliseconds &time)
{
	_hint |= HINT_TIME;
	uintmax_t timed = _timing.second.fetch_add(1, boost::memory_order_relaxed);
	double fall = (double)timed / (double)(timed + 1);
	double incr = (double)time.count() / (double)(timed + 1);
	uintmax_t origin = _timing.first.load(boost::memory_order_release);
	uintmax_t average = (uintmax_t)(origin * fall + incr);
	_timing.first.fetch_add(average - origin, boost::memory_order_consume);
}

void PerfCounter::HitOrMiss(const bool hit)
{
	_hint |= HINT_HIT;
	if (hit) _hit.first.fetch_add(1, boost::memory_order_relaxed);
	_hit.second.fetch_add(1, boost::memory_order_relaxed);
}

}}
