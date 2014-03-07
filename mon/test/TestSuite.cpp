#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include "conf/ConfFileFixture.h"
#include "log/LogFixture.h"
#include "db/DatabaseFixture.h"
#include "cache/CacheFixture.h"

namespace msvc { namespace test {

BOOST_GLOBAL_FIXTURE(ConfFileFixture);
BOOST_GLOBAL_FIXTURE(LogFixture);
BOOST_GLOBAL_FIXTURE(DatabaseFixture);
BOOST_GLOBAL_FIXTURE(CacheFixture);

}}
