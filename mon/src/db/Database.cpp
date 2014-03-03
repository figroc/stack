#include "Database.h"
#include "impl/DbUri.h"
#include "util/DbPerfC.h"
#include "impl/mongo/MongoDbUri.h"
#include "impl/mongo/MongoDbClient.h"
#include "impl/mysql/MySqlDbUri.h"
#include "impl/mysql/MySqlDbClient.h"

namespace msvc { namespace db {

using namespace std;

void Database::Init()
{
	DbPerfC::Init();
}

auto_ptr<Database> Database::Get(const std::string &uri)
{
	DbUri dburi(uri);
	if (dburi.valid()) {
		if (dburi.scheme() == MySqlDbUri::URI_SCHEME)
			return MySqlDbClient::Get(dburi);
		if (dburi.scheme() == MongoDbUri::URI_SCHEME)
			return MongoDbClient::Get(dburi);
	}
	return auto_ptr<Database>();
}

}}


