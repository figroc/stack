#ifndef MSVC_CACHE_CACHE_H
#define MSVC_CACHE_CACHE_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace msvc { namespace cache {

class Cache {
public:
	static void Init();
	static std::auto_ptr<Cache> Get(const std::string &uri);

protected:
	Cache() { };
private:
	Cache(const Cache &cache);
	Cache &operator=(const Cache &cache);
public:
	virtual ~Cache() { };

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

};

}}

#endif
