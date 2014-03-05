#ifndef DB_MYSQL_DB_URI_H
#define DB_MYSQL_DB_URI_H

#include "incl/util/util.h"
#include <string>

namespace msvc { namespace db {

using namespace msvc::util;

class MySqlDbUri {
public:
	static const std::string URI_SCHEME;

private:
	Uri _uri;
	bool _valid;
	std::string _host;

public:
	MySqlDbUri(const std::string &uri) : _uri(uri) { checkUri(); };
	MySqlDbUri(const Uri &uri) : _uri(uri) { checkUri(); };

public:
	inline bool valid() const { return _valid; };
	inline const std::string &user() const { return _uri.user(); };
	inline const std::string &pass() const { return _uri.pass(); };
	inline const std::string &host() const { return _host; };
	inline const std::string &name() const { return _uri.path(); };

	inline const std::string &uri() const { return _uri.uri(); }
	inline const std::string &userAtDb() const { return _uri.identity(); }

private:
	void checkUri();
};

}}

#endif
