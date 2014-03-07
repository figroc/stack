#ifndef MSVC_TEST_DATABASE_FIXTURE_H
#define MSVC_TEST_DATABASE_FIXTURE_H

#include "incl/db/db.h"

namespace msvc { namespace test {

using namespace msvc::db;

class DatabaseFixture {

public:
	DatabaseFixture() {
		Database::Init();
	};
	virtual ~DatabaseFixture() {

	};
};

}}

#endif //MSVC_TEST_DATABASE_FIXTURE_H
