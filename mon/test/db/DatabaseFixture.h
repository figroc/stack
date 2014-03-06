#ifndef MSVC_TEST_DATABASE_FIXTURE_H
#define MSVC_TEST_DATABASE_FIXTURE_H

#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>
#include "../../incl/db/db.h"

namespace msvc { namespace test {

using namespace std;
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
