#ifndef MSVC_CACHE_REDIS_CLIENT_H
#define MSVC_CACHE_REDIS_CLIENT_H

#include "../../Cache.h"
#include "RedisConnPool.h"

namespace msvc { namespace cache {

class RedisClient : public Cache {
private:
	static boost::shared_ptr<RedisConnPool> _pool;

public:
	static std::auto_ptr<Cache> Get(const RedisUri &uri);

private:
	RedisUri _uri;
	std::auto_ptr<RedisConnPool::ScopedConnection> _conn;

private:
	RedisClient(const RedisUri &uri) : _uri(uri) {
		_conn = _pool->Fetch(uri);
	}
	RedisClient(const RedisClient &client);
	RedisClient &operator=(const RedisClient &client);

public:
	virtual void Drop(const std::string &key);

	virtual std::string GetStr(const std::string &key, const std::string &def = std::string());
	virtual std::vector<std::string> GetStr(const std::vector<std::string> &key);
	virtual void SetStr(const std::string &key, const std::string &value, const int expire = -1);
	virtual void SetStr(const std::map<std::string, std::string> &list, const int expire = -1);

	virtual std::vector<std::string> GetStrSet(const std::string &key);
	virtual void SetStrSet(const std::string &key, const std::string &value, const int expire = -1);
	virtual void SetStrSet(const std::string &key, const std::vector<std::string> &value, const int expire = -1);
	virtual void AddStrSet(const std::string &key, const std::string &value, const int expire = -1);
	virtual void AddStrSet(const std::string &key, const std::vector<std::string> &value, const int expire = -1);
	virtual void SubStrSet(const std::string &key, const std::string &value, const int expire = -1);
	virtual void SubStrSet(const std::string &key, const std::vector<std::string> &value, const int expire = -1);

private:
	std::vector<std::string> GetStrArrayReply();
	template<typename T>
	void SetOperation(const std::string &pre, const std::string &opr,
			const std::string &key, const T &value, const int expire);
};

}}

#endif
