#ifndef DB_MONGODB_URI_H
#define DB_MONGODB_URI_H

#include <string>

namespace msvc { namespace db {

class MongoDbUri {
private:
	static const std::string URI_SCHEMA;
	static const std::string URI_REPLICA;

private:
	std::string _uri;

	bool _valid;
	std::string _host;
	std::string _name;
	std::string _user;
	std::string _pass;

	std::string _userAtDb;

public:
	MongoDbUri(const std::string &uri);
	~MongoDbUri() { }

public:
	inline bool valid() const { return _valid; }
	inline const std::string &host() const { return _host; }
	inline const std::string &name() const { return _name; }
	inline const std::string &user() const { return _user; }
	inline const std::string &pass() const { return _pass; }

	inline const std::string &uri() const { return _uri; }
	inline const std::string &userAtDb() const { return _userAtDb; }

private:
	bool ParseUriStr(const std::string &uri);
};
}}

#endif /* DB_MONGODB_URI_H */
