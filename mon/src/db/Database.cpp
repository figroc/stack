#include "Database.h"
#include "util/DbPerfC.h"
#include "impl/mongo/MongoDbUri.h"
#include "impl/mongo/MongoDbClient.h"
#include "impl/mysql/MySqlDbUri.h"
#include "impl/mysql/MySqlDbClient.h"
#include "incl/util/util.h"
#include <stdexcept>

namespace msvc { namespace db {

using namespace std;
using namespace msvc::util;

void Database::Init()
{
	DbPerfC::Init();
}

auto_ptr<Database> Database::Get(const std::string &uri)
{
	Uri dburi(uri);
	if (dburi.valid()) {
		if (dburi.scheme() == MySqlDbUri::URI_SCHEME)
			return MySqlDbClient::Get(dburi);
		if (dburi.scheme() == MongoDbUri::URI_SCHEME)
			return MongoDbClient::Get(dburi);
	}
	throw logic_error(string("invalid uri: ") + uri);
}

}}


