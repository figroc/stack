#include "RedisUri.h"
#include "incl/util/util.h"

namespace msvc { namespace cache {

using namespace std;
using namespace msvc::util;

const string RedisUri::URI_SCHEME = "redis";

void RedisUri::checkUri()
{
	_valid = _uri.valid() && _uri.scheme() == URI_SCHEME;
	if (_valid) {
		pair<string, string> ep = ParseEndpoint(_uri.host());
		_addr = ep.first;
		_valid = StringCastTry(ep.second, _port);
		if (_valid) {
			_userAtDb.append(_uri.scheme()).append(":")
					 .append(_uri.pass()).append("@")
					 .append(_uri.host()).append("$")
					 .append(_uri.path());
		}
	}
}

}}
