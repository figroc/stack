#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include "conf/ConfFileFixture.h"

namespace msvc { namespace test {

BOOST_GLOBAL_FIXTURE(ConfFileFixture);

}}
