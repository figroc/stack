#ifndef UTIL_SAFE_FUNCTOR_H
#define UTIL_SAFE_FUNCTOR_H

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/type_traits.hpp>

namespace msvc { namespace util {

// the safety is achieved by checking against callable() member function
template<typename F> class SafeFunctor;

template<typename R>
inline SafeFunctor<R (void)> make_functor(R (*const f)()) {
	return SafeFunctor<R (void)>(f);
}
template<typename T, typename R>
inline SafeFunctor<R (void)> make_functor(R (T::*const f)(), const boost::shared_ptr<T> &o, const bool pin) {
	return pin ? SafeFunctor<R (void)>(o, boost::bind(f, o))
	           : SafeFunctor<R (void)>(o, boost::bind(f, o.get()));
}
template<typename R, typename A1>
inline SafeFunctor<R (A1)> make_functor(R (*const f)(A1)) {
	return SafeFunctor<R (A1)>(f);
}
template<typename T, typename R, typename A1>
inline SafeFunctor<R (A1)> make_functor(R (T::*const f)(A1), const boost::shared_ptr<T> &o, const bool pin) {
	return pin ? SafeFunctor<R (A1)>(o, bind(f, o, _1))
	           : SafeFunctor<R (A1)>(o, bind(f, o.get(), _1));
}
template<typename R, typename A1, typename A2>
inline SafeFunctor<R (A1, A2)> make_functor(R (*const f)(A1, A2)) {
	return SafeFunctor<R (A1, A2)>(f);
}
template<typename T, typename R, typename A1, typename A2>
inline SafeFunctor<R (A1, A2)> make_functor(R (T::*const f)(A1, A2), const boost::shared_ptr<T> &o, const bool pin) {
	return pin ? SafeFunctor<R (A1, A2)>(o, bind(f, o, _1, _2))
	           : SafeFunctor<R (A1, A2)>(o, bind(f, o.get(), _1, _2));
}
template<typename R, typename A1, typename A2, typename A3>
inline SafeFunctor<R (A1, A2, A3)> make_functor(R (*const f)(A1, A2, A3)) {
	return SafeFunctor<R (A1, A2, A3)>(f);
}
template<typename T, typename R, typename A1, typename A2, typename A3>
inline SafeFunctor<R (A1, A2, A3)> make_functor(R (T::*const f)(A1, A2, A3), const boost::shared_ptr<T> &o, const bool pin) {
	return pin ? SafeFunctor<R (A1, A2, A3)>(o, bind(f, o, _1, _2, _3))
	           : SafeFunctor<R (A1, A2, A3)>(o, bind(f, o.get(), _1, _2, _3));
}
template<typename R, typename A1, typename A2, typename A3, typename A4>
inline SafeFunctor<R (A1, A2, A3, A4)> make_functor(R (*const f)(A1, A2, A3, A4)) {
	return SafeFunctor<R (A1, A2, A3, A4)>(f);
}
template<typename T, typename R, typename A1, typename A2, typename A3, typename A4>
inline SafeFunctor<R (A1, A2, A3, A4)> make_functor(R (T::*const f)(A1, A2, A3, A4), const boost::shared_ptr<T> &o, const bool pin) {
	return pin ? SafeFunctor<R (A1, A2, A3, A4)>(o, bind(f, o, _1, _2, _3, _4))
	           : SafeFunctor<R (A1, A2, A3, A4)>(o, bind(f, o.get(), _1, _2, _3, _4));
}
template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
inline SafeFunctor<R (A1, A2, A3, A4, A5)> make_functor(R (*const f)(A1, A2, A3, A4, A5)) {
	return SafeFunctor<R (A1, A2, A3, A4, A5)>(f);
}
template<typename T, typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
inline SafeFunctor<R (A1, A2, A3, A4, A5)> make_functor(R (T::*const f)(A1, A2, A3, A4, A5), const boost::shared_ptr<T> &o, const bool pin) {
	return pin ? SafeFunctor<R (A1, A2, A3, A4, A5)>(o, bind(f, o, _1, _2, _3, _4, _5))
	           : SafeFunctor<R (A1, A2, A3, A4, A5)>(o, bind(f, o.get(), _1, _2, _3, _4, _5));
}

class ScopedPtr {
public:
	bool _flip;
	boost::shared_ptr<void> _p;
public:
	ScopedPtr(const bool flip) : _flip(flip), _p() { };
	ScopedPtr(const boost::weak_ptr<void> &p) : _flip(false), _p(p) { };
	ScopedPtr(const boost::shared_ptr<void> &p) : _flip(false), _p(p) { };
	virtual ~ScopedPtr() { };
public:
	inline operator bool() const { return _flip || (bool)_p; };
	inline bool operator !() const { return !_flip && !_p; };
	inline void *owner() const { return _flip ? 0 : _p.get(); };
};

#define SAFE_FUNCTOR_BODY_IMPL \
private: \
	bool _raw; \
	boost::weak_ptr<void> _p; \
public: \
	SafeFunctor() : func_type(), _raw(true), _p() { }; \
	SafeFunctor(const raw_func_type f) : func_type(f), _raw(true), _p() { }; \
	template<typename T> \
	SafeFunctor(const boost::shared_ptr<T> &p, const MEM_FN_BIND_TYPE_RAW &b) : func_type(b), _raw(false), _p(p) { }; \
	template<typename T> \
	SafeFunctor(const boost::shared_ptr<T> &p, const MEM_FN_BIND_TYPE_PTR &b) : func_type(b), _raw(false), _p(p) { }; \
	virtual ~SafeFunctor() { }; \
public: \
	inline ScopedPtr callable() const { \
		if (_raw) \
			return ScopedPtr(!func_type::function::empty()); \
		if (_p.expired()) \
			return ScopedPtr(false); \
		try { \
			boost::shared_ptr<void> p(_p, boost::detail::sp_nothrow_tag()); \
			return (!p || func_type::function::empty()) ? ScopedPtr(false) : ScopedPtr(p); } \
		catch (const boost::bad_weak_ptr &) { \
			return ScopedPtr(false); \
		} \
	}; \

template<typename R>
class SafeFunctor<R (void)> : public boost::function<R (void)> {
private:
	typedef SafeFunctor<R (void)> self_type;
	typedef boost::function<R (void)> func_type;
	typedef R (*raw_func_type)(void);
#define MEM_FN_BIND_TYPE_RAW boost::_bi::bind_t<R, boost::_mfi::mf0<R, T>, typename boost::_bi::list_av_1< T* >::type>
#define MEM_FN_BIND_TYPE_PTR boost::_bi::bind_t<R, boost::_mfi::mf0<R, T>, typename boost::_bi::list_av_1< boost::shared_ptr<T> >::type>
	SAFE_FUNCTOR_BODY_IMPL
#undef MEM_FN_BIND_TYPE_RAW
#undef MEM_FN_BIND_TYPE_PTR
};

template<typename R, typename A1>
class SafeFunctor<R (A1)> : public boost::function<R (A1)> {
private:
	typedef SafeFunctor<R (A1)> self_type;
	typedef boost::function<R (A1)> func_type;
	typedef R (*raw_func_type)(A1);
#define MEM_FN_BIND_TYPE_RAW boost::_bi::bind_t<R, boost::_mfi::mf1<R, T, A1>, typename boost::_bi::list_av_2< T*, boost::arg<1> >::type>
#define MEM_FN_BIND_TYPE_PTR boost::_bi::bind_t<R, boost::_mfi::mf1<R, T, A1>, typename boost::_bi::list_av_2< boost::shared_ptr<T>, boost::arg<1> >::type>
	SAFE_FUNCTOR_BODY_IMPL
#undef MEM_FN_BIND_TYPE_RAW
#undef MEM_FN_BIND_TYPE_PTR
};

template<typename R, typename A1, typename A2>
class SafeFunctor<R (A1, A2)> : public boost::function<R (A1,A2)> {
private:
	typedef SafeFunctor<R (A1, A2)> self_type;
	typedef boost::function<R (A1, A2)> func_type;
	typedef R (*raw_func_type)(A1, A2);
#define MEM_FN_BIND_TYPE_RAW boost::_bi::bind_t<R, boost::_mfi::mf2<R, T, A1, A2>, typename boost::_bi::list_av_3< T*, boost::arg<1>, boost::arg<2> >::type>
#define MEM_FN_BIND_TYPE_PTR boost::_bi::bind_t<R, boost::_mfi::mf2<R, T, A1, A2>, typename boost::_bi::list_av_3< boost::shared_ptr<T>, boost::arg<1>, boost::arg<2> >::type>
	SAFE_FUNCTOR_BODY_IMPL
#undef MEM_FN_BIND_TYPE_RAW
#undef MEM_FN_BIND_TYPE_PTR
};

template<typename R, typename A1, typename A2, typename A3>
class SafeFunctor<R (A1, A2, A3)> : public boost::function<R (A1,A2,A3)> {
private:
	typedef SafeFunctor<R (A1, A2, A3)> self_type;
	typedef boost::function<R (A1, A2, A3)> func_type;
	typedef R (*raw_func_type)(A1, A2, A3);
#define MEM_FN_BIND_TYPE_RAW boost::_bi::bind_t<R, boost::_mfi::mf3<R, T, A1, A2, A3>, typename boost::_bi::list_av_4< T*, boost::arg<1>, boost::arg<2>, boost::arg<3> >::type>
#define MEM_FN_BIND_TYPE_PTR boost::_bi::bind_t<R, boost::_mfi::mf3<R, T, A1, A2, A3>, typename boost::_bi::list_av_4< boost::shared_ptr<T>, boost::arg<1>, boost::arg<2>, boost::arg<3> >::type>
	SAFE_FUNCTOR_BODY_IMPL
#undef MEM_FN_BIND_TYPE_RAW
#undef MEM_FN_BIND_TYPE_PTR
};

template<typename R, typename A1, typename A2, typename A3, typename A4>
class SafeFunctor<R (A1, A2, A3, A4)> : public boost::function<R (A1,A2,A3,A4)> {
private:
	typedef SafeFunctor<R (A1, A2, A3, A4)> self_type;
	typedef boost::function<R (A1, A2, A3, A4)> func_type;
	typedef R (*raw_func_type)(A1, A2, A3, A4);
#define MEM_FN_BIND_TYPE_RAW boost::_bi::bind_t<R, boost::_mfi::mf4<R, T, A1, A2, A3, A4>, typename boost::_bi::list_av_5< T*, boost::arg<1>, boost::arg<2>, boost::arg<3>, boost::arg<4> >::type>
#define MEM_FN_BIND_TYPE_PTR boost::_bi::bind_t<R, boost::_mfi::mf4<R, T, A1, A2, A3, A4>, typename boost::_bi::list_av_5< boost::shared_ptr<T>, boost::arg<1>, boost::arg<2>, boost::arg<3>, boost::arg<4> >::type>
	SAFE_FUNCTOR_BODY_IMPL
#undef MEM_FN_BIND_TYPE_RAW
#undef MEM_FN_BIND_TYPE_PTR
};

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
class SafeFunctor<R (A1,A2,A3,A4,A5)> : public boost::function<R (A1,A2,A3,A4,A5)> {
private:
	typedef SafeFunctor<R (A1, A2, A3, A4, A5)> self_type;
	typedef boost::function<R (A1, A2, A3, A4, A5)> func_type;
	typedef R (*raw_func_type)(A1, A2, A3, A4, A5);
#define MEM_FN_BIND_TYPE_RAW boost::_bi::bind_t<R, boost::_mfi::mf5<R, T, A1, A2, A3, A4, A5>, typename boost::_bi::list_av_6< T*, boost::arg<1>, boost::arg<2>, boost::arg<3>, boost::arg<4>, boost::arg<5> >::type>
#define MEM_FN_BIND_TYPE_PTR boost::_bi::bind_t<R, boost::_mfi::mf5<R, T, A1, A2, A3, A4, A5>, typename boost::_bi::list_av_6< boost::shared_ptr<T>, boost::arg<1>, boost::arg<2>, boost::arg<3>, boost::arg<4>, boost::arg<5> >::type>
	SAFE_FUNCTOR_BODY_IMPL
#undef MEM_FN_BIND_TYPE_RAW
#undef MEM_FN_BIND_TYPE_PTR
};
#undef SAFE_FUNCTOR_BODY_IMPL
}}

#endif /* UTIL_SAFE_FUNCTOR_H */
