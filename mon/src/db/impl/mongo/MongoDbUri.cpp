#include "MongoDbUri.h"

namespace msvc { namespace db {

using namespace std;

const string MongoDbUri::URI_SCHEME = "mongodb";
const string MongoDbUri::URI_REPLICA = "replicaSet";

void MongoDbUri::checkUri() {
	_valid = _uri.valid() && (URI_SCHEME == _uri.scheme());
	if (_valid) {
		string set;
		if (_uri.option(URI_REPLICA, set))
			_host = set.append("/") + _uri.host();
		else
			_host = _uri.host();
	}
}

}}


