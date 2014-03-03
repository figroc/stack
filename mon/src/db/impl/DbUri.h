#ifndef DB_DB_URI_H
#define DB_DB_URI_H

#include <string>
#include <map>

namespace msvc { namespace db {

class DbUri {
private:
	std::string _uri;

	bool _valid;

	std::string _scheme;
	std::string _user;
	std::string _pass;
	std::string _host;
	std::string _name;

	std::map<std::string, std::string> _opts;

	std::string _userAtDb;

public:
	DbUri(const std::string &uri);
	~DbUri() { }

public:
	inline const std::string &uri() const { return _uri; }

	inline bool valid() const { return _valid; }
	inline const std::string &scheme() const { return _scheme; }
	inline const std::string &user() const { return _user; }
	inline const std::string &pass() const { return _pass; }
	inline const std::string &host() const { return _host; }
	inline const std::string &name() const { return _name; }

	inline bool optExists(const std::string &opt) const {
		return _opts.find(opt) != _opts.end();
	}
	inline std::string optValue(const std::string &opt, const std::string &def = std::string()) const {
		std::map<std::string, std::string>::const_iterator it = _opts.find(opt);
		return it == _opts.end() ? def : it->second;
	}
	inline bool option(const std::string &opt, std::string &val) const {
		std::map<std::string, std::string>::const_iterator it = _opts.find(opt);
		return it == _opts.end() ? false : (val = it->second, true);
	}

	inline const std::string &userAtDb() const { return _userAtDb; }

private:
	bool ParseUriStr(const std::string &uri);
};
}}

#endif /* DB_MONGODB_URI_H */
