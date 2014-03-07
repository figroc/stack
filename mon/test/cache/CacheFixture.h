#ifndef MSVC_TEST_CACHE_FIXTURE_H
#define MSVC_TEST_CACHE_FIXTURE_H

#include "incl/cache/cache.h"

namespace msvc { namespace test {

using namespace msvc::cache;

class CacheFixture {

public:
	CacheFixture() {
		Cache::Init();
	};
	virtual ~CacheFixture() {

	};
};

}}

#endif //MSVC_TEST_CACHE_FIXTURE_H
