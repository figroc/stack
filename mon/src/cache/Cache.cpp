#include "Cache.h"
#include "util/CachePerfC.h"
#include "incl/util/util.h"
#include "incl/log/log.h"
#include "impl/redis/RedisUri.h"
#include "impl/redis/RedisClient.h"

namespace msvc { namespace cache {

using namespace std;
using namespace msvc::util;
using namespace msvc::log;

void Cache::Init()
{
	CachePerfC::Init();
}

auto_ptr<Cache> Cache::Get(const std::string &uri)
{
	try {
		Uri info(uri);
		if (info.valid()) {
			if (info.scheme() == RedisUri::URI_SCHEME)
				return RedisClient::Get(info);
		}
	} catch (const exception &ex) {
		LC_TRACE_ERROR("[Cache::Get] failed to get cache: %%", uri);
	}
	return auto_ptr<Cache>(new Cache());
}

void Cache::Drop(const string &key) { }
void Cache::Drop(const vector<string> &key) { }

string Cache::GetStr(const string &key, const string &def) { return def; }
vector<string> Cache::GetStr(const vector<string> &key) { return vector<string>(); }
void Cache::SetStr(const string &key, const string &value, const int expire) { }
void Cache::SetStr(const map<string, string> &list, const int expire) { }

vector<string> Cache::GetStrSet(const string &key) { return vector<string>(); }
void Cache::SetStrSet(const string &key, const string &value, const int expire) { }
void Cache::SetStrSet(const string &key, const vector<string> &value, const int expire) { }
void Cache::AddStrSet(const string &key, const string &value, const int expire) { }
void Cache::AddStrSet(const string &key, const vector<string> &value, const int expire) { }
void Cache::SubStrSet(const string &key, const string &value, const int expire) { }
void Cache::SubStrSet(const string &key, const vector<string> &value, const int expire) { }

}}
