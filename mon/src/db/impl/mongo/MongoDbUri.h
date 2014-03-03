#ifndef DB_MONGODB_URI_H
#define DB_MONGODB_URI_H

#include "../DbUri.h"
#include <string>

namespace msvc { namespace db {

class MongoDbUri {
public:
	static const std::string URI_SCHEME;
private:
	static const std::string URI_REPLICA;

private:
	DbUri _uri;
	bool _valid;
	std::string _host;

public:
	MongoDbUri(const std::string &uri) : _uri(uri) { checkUri(); };
	MongoDbUri(const DbUri &uri) : _uri(uri) { checkUri(); };

public:
	inline bool valid() const { return _valid; }
	inline const std::string &user() const { return _uri.user(); }
	inline const std::string &pass() const { return _uri.pass(); }
	inline const std::string &host() const { return _host; }
	inline const std::string &name() const { return _uri.name(); }

	inline const std::string &uri() const { return _uri.uri(); }
	inline const std::string &userAtDb() const { return _uri.userAtDb(); }

private:
	void checkUri();
};
}}

#endif /* DB_MONGODB_URI_H */
