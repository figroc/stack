#ifndef MSVC_CACHE_REDIS_URI_H
#define MSVC_CACHE_REDIS_URI_H

#include "incl/util/util.h"

namespace msvc { namespace cache {

using namespace msvc::util;

class RedisUri {
public:
	static const std::string URI_SCHEME;

private:
	Uri _uri;
	bool _valid;
	std::string _addr;
	int _port;
	std::string _userAtDb;

public:
	explicit RedisUri(const std::string &uri) : _uri(uri) { checkUri(); };
	explicit RedisUri(const Uri &uri) : _uri(uri) { checkUri(); };

public:
	inline const std::string &uri() const { return _uri.uri(); };

	inline bool valid() const { return _valid; };
	inline const std::string &pass() const { return _uri.pass(); };
	inline const std::string &name() const { return _uri.path(); };
	inline const std::string &host() const { return _uri.host(); };
	inline const std::string &addr() const { return _addr; };
	inline int port() const { return _port; };

	inline const std::string &userAtDb() const { return _userAtDb; };

private:
	void checkUri();
};

}}

#endif
