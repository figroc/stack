#include "MongoDbUri.h"

namespace msvc { namespace db {

const std::string MongoDbUri::URI_SCHEMA = "mongodb://";
const std::string MongoDbUri::URI_REPLICA = "replicaSet";

MongoDbUri::MongoDbUri(const std::string &uri)	{
	_valid = ParseUriStr(uri);
	if (_valid) {
		_uri.append(uri);
		_userAtDb.append(_user).append("@").append(_host).append("$").append(_name);
	}
}

bool MongoDbUri::ParseUriStr(const std::string &uri)
{
	int offset = 0;
	if (0 != uri.find(URI_SCHEMA))
		return false;

	offset = URI_SCHEMA.size();
	int endpos = uri.find_first_of(":", offset);
	if (endpos <= offset)
		return false;
	_user = uri.substr(offset, endpos - offset);

	offset = endpos + 1;
	endpos = uri.find_first_of("@", offset);
	if (endpos <= offset)
		return false;
	_pass = uri.substr(offset, endpos - offset);

	offset = endpos + 1;
	endpos = uri.find_first_of("/", offset);
	if (endpos <= offset)
		return false;
	_host = uri.substr(offset, endpos - offset);

	offset = endpos + 1;
	endpos = uri.find_first_of("?", offset);
	if (endpos == offset)
		return false;
	if (endpos < offset) {
		_name = uri.substr(offset);
		return true;
	}
	_name = uri.substr(offset, endpos - offset);

	while(endpos >= offset) {
		char pr = uri[endpos];

		offset = endpos + 1;
		endpos = uri.find(URI_REPLICA, offset);

		if (endpos < offset)
			break;

		offset = endpos + URI_REPLICA.size();
		endpos = uri.find_first_of("&", offset);

		if ('?' == pr or '&' == pr) {
			std::string set;
			if (uri.size() > offset && '&' != uri[offset]) {
				if ('=' != uri[offset++])
					continue;
				if (uri.size() > offset && endpos != offset)
					set = endpos < offset
						? uri.substr(offset)
						: uri.substr(offset, endpos - offset);
			}
			_host.insert(0, set.append("/"));
			break;
		}
	}
	return true;
}

}}


