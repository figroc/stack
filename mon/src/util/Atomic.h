#ifndef MSVC_UTIL_ATOMIC_H
#define MSVC_UTIL_ATOMIC_H

#include <boost/atomic.hpp>

namespace msvc { namespace util {

template<typename T>
struct Atomic {
typedef T (*opr)(boost::atomic<T> &, T, boost::memory_order);
static T Get(boost::atomic<T> &atomic, T dum, boost::memory_order order);
static T Set(boost::atomic<T> &atomic, T val, boost::memory_order order);
static T Add(boost::atomic<T> &atomic, T val, boost::memory_order order);
static T Sub(boost::atomic<T> &atomic, T val, boost::memory_order order);
};

template<typename T>
T Atomic<T>::Get(boost::atomic<T> &atomic, T dum, boost::memory_order order) {
	return atomic.load(order);
}
template<typename T>
T Atomic<T>::Set(boost::atomic<T> &atomic, T val, boost::memory_order order) {
	return atomic.exchange(val, order);
}
template<typename T>
T Atomic<T>::Add(boost::atomic<T> &atomic, T val, boost::memory_order order) {
	return atomic.fetch_add(val, order);
}
template<typename T>
T Atomic<T>::Sub(boost::atomic<T> &atomic, T val, boost::memory_order order) {
	return atomic.fetch_sub(val, order);
}

}}

#endif
