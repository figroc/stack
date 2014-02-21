#include "Database.h"
#include "util/DbPerfC.h"
#include "impl/mongo/MongoDbClient.h"

namespace msvc { namespace db {

void Database::Init()
{
	DbPerfC::Init();
}

std::auto_ptr<Database> Database::Get(const std::string &uri)
{
	return MongoDbClient::Get(uri);
}

}}


