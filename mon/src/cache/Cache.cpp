#include "Cache.h"
#include "util/CachePerfC.h"

namespace msvc { namespace cache {

using namespace std;

void Cache::Init()
{
	CachePerfC::Init();
}

auto_ptr<Cache> Cache::Get(const std::string &uri)
{
	return auto_ptr<Cache>(new Cache());
}

void Cache::Drop(const string &key) { }

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
