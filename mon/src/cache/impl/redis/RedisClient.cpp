#include "RedisClient.h"
#include "RedisHelper.h"
#include "incl/util/util.h"
#include <boost/lexical_cast.hpp>

namespace msvc { namespace cache {

using namespace std;
using namespace msvc::util;

boost::shared_ptr<RedisConnPool> RedisClient::_pool(RedisConnPool::Create());

auto_ptr<Cache> RedisClient::Get(const RedisUri &uri)
{
	return auto_ptr<Cache>(new RedisClient(uri));
}

void RedisClient::Drop(const string &key)
{
	RedisHelper::Call(_conn->ptr(), "DEL", key);
	RedisHelper::Reply(_conn->ptr());
}

string RedisClient::GetStr(const string &key, const string &def)
{
	RedisHelper::Call(_conn->ptr(), "GET", key);
	PerfCounter *perf = _conn->perf();
	redis_reply_ptr rpl = RedisHelper::Reply(_conn->ptr());
	bool hit = rpl && (rpl->type == REDIS_REPLY_STRING);
	_conn->perf()->HitOrMiss(hit);
	return hit ? string(rpl->str, rpl->len) : def;
}

vector<string> RedisClient::GetStr(const vector<string> &key)
{
	RedisHelper::Call(_conn->ptr(), "MGET", key);
	return GetStrArrayReply();
}

void RedisClient::SetStr(const string &key, const string &val, const int expire)
{
	if (expire > 0)
		RedisHelper::Call(_conn->ptr(), "SETEX", key, expire, val);
	else
		RedisHelper::Call(_conn->ptr(), "SET", key, val);
	RedisHelper::Reply(_conn->ptr());
}

void RedisClient::SetStr(const map<string, string> &list, const int expire)
{
	if (!list.empty()) {
		RedisHelper::Call(_conn->ptr(), "MSET", list);
		if (expire > 0) {
			for (map<string, string>::const_iterator it = list.begin(); it != list.end(); ++it)
				RedisHelper::Call(_conn->ptr(), "EXPIRE", it->first, expire);
		} else {
			for (map<string, string>::const_iterator it = list.begin(); it != list.end(); ++it)
				RedisHelper::Call(_conn->ptr(), "PERSIST", it->first);
		}
		RedisHelper::Reply(_conn->ptr());
		for (int i = 0; i < list.size(); ++i) {
			RedisHelper::Reply(_conn->ptr());
		}
	}
}

vector<string> RedisClient::GetStrSet(const string &key)
{
	RedisHelper::Call(_conn->ptr(), "SMEMBERS", key);
	return GetStrArrayReply();
}

void RedisClient::SetStrSet(const string &key, const string &value, const int expire)
{
	static const string PRE = "DEL";
	static const string OPR = "SADD";
	SetOperation(PRE, OPR, key, value, expire);
}

void RedisClient::SetStrSet(const string &key, const vector<string> &value, const int expire)
{
	static const string PRE = "DEL";
	static const string OPR = "SADD";
	SetOperation(PRE, OPR, key, value, expire);
}

void RedisClient::AddStrSet(const string &key, const string &value, const int expire)
{
	static const string PRE = "";
	static const string OPR = "SADD";
	SetOperation(PRE, OPR, key, value, expire);
}

void RedisClient::AddStrSet(const string &key, const vector<string> &value, const int expire)
{
	static const string PRE = "";
	static const string OPR = "SADD";
	SetOperation(PRE, OPR, key, value, expire);
}

void RedisClient::SubStrSet(const string &key, const string &value, const int expire)
{
	static const string PRE = "";
	static const string OPR = "SREM";
	SetOperation(PRE, OPR, key, value, expire);
}

void RedisClient::SubStrSet(const string &key, const vector<string> &value, const int expire)
{
	static const string PRE = "";
	static const string OPR = "SREM";
	SetOperation(PRE, OPR, key, value, expire);
}

vector<string> RedisClient::GetStrArrayReply()
{
	redis_reply_ptr rpl = RedisHelper::Reply(_conn->ptr());
	bool hit = rpl && rpl->type == REDIS_REPLY_ARRAY;
	_conn->perf()->HitOrMiss(hit);

	vector<string> val;
	if (hit) {
		for (int i = 0; i < rpl->elements; ++i) {
			redisReply *inner = rpl->element[i];
			if (inner && inner->type == REDIS_REPLY_STRING)
				val.push_back(string(rpl->str, rpl->len));
		}
	}
	return val;
}

template<typename T>
void RedisClient::SetOperation(const string &pre, const string &opr,
		const string &key, const T &value, const int expire)
{
	RedisHelper::Call(_conn->ptr(), "MULTI");
	if (!pre.empty())
		RedisHelper::Call(_conn->ptr(), pre, key);
	RedisHelper::Call(_conn->ptr(), opr, key, value);
	if (expire > 0)
		RedisHelper::Call(_conn->ptr(), "EXPIRE", key, expire);
	else
		RedisHelper::Call(_conn->ptr(), "PERSIST", key);
	RedisHelper::Call(_conn->ptr(), "EXEC");

	RedisHelper::Reply(_conn->ptr());
	if (!pre.empty())
		RedisHelper::Reply(_conn->ptr());
	RedisHelper::Reply(_conn->ptr());
	RedisHelper::Reply(_conn->ptr());
	RedisHelper::Reply(_conn->ptr());
}

}}
