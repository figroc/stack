#include "Cache.h"

namespace msvc { namespace cache {

using namespace std;

auto_ptr<Cache> Cache::Get(const std::string &uri)
{
	return auto_ptr<Cache>(new Cache());
}

string Cache::GetStr(const string &key, const string &def) { return def; }
vector<string> Cache::GetStr(const vector<string> &key) { return vector<string>(); }
void Cache::SetStr(const string &key, const string &value, const int expire) { }
void Cache::SetStr(const map<string, string> &list, const int expire) { }

}}
