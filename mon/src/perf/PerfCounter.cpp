#include "PerfCounter.h"
#include "incl/util/util.h"

namespace msvc { namespace perf {

using namespace std;
using namespace boost;
using namespace msvc::util;

const PerfCounter::atomic_op_f PerfCounter::SET_ADD[] = {
		&Atomic<boost::uintmax_t>::Set, &Atomic<boost::uintmax_t>::Add
	};

void PerfCounter::IncRate(atomic_rate_t (&array)[RATE_DURATION], const size_t count)
{
	const uintmax_t now = time(NULL);
	atomic_rate_t &rate(array[now % RATE_DURATION]);
	atomic_op_f opr = SET_ADD[now == rate.first.exchange(now, memory_order_release)];
	opr(rate.second, count, memory_order_consume);
}

double PerfCounter::AvgRate(const atomic_rate_t (&array)[RATE_DURATION])
{
	const time_t now = time(NULL);
	size_t total(0), average(0);
	for (int i = 0; i < RATE_DURATION; ++i) {
		const atomic_rate_t &rate(array[i]);
		if (now - rate.first.load(memory_order_relaxed) <= RATE_DURATION) {
			total += rate.second.load(memory_order_relaxed);
			++average;
		}
	}
	if (0 == average)
		return 0.0;
	return (double)total / (double)average;
}

void PerfCounter::Increment(const size_t count)
{
	_hint |= HINT_INC;
	_current.fetch_add(count, memory_order_relaxed);
	IncRate(_rateInc, count);
}

void PerfCounter::Decrement(const size_t count)
{
	_hint |= HINT_DEC;
	_current.fetch_sub(count, memory_order_relaxed);
	IncRate(_rateDec, count);
}

void PerfCounter::Timing(const chrono::milliseconds &time)
{
	_hint |= HINT_TIME;
	uintmax_t timed = _timing.second.fetch_add(1, memory_order_relaxed);
	double fall = (double)timed / (double)(timed + 1);
	double incr = (double)time.count() / (double)(timed + 1);
	uintmax_t origin = _timing.first.load(memory_order_release);
	uintmax_t average = (uintmax_t)(origin * fall + incr);
	_timing.first.fetch_add(average - origin, memory_order_consume);
}

}}
