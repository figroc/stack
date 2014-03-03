#include "MySqlDbUri.h"

namespace msvc { namespace db {

using namespace std;

const string MySqlDbUri::URI_SCHEME = "mysql";

void MySqlDbUri::checkUri() {
	_valid = _uri.valid() && (URI_SCHEME == _uri.scheme());
	if (_valid) {
		_host = string("tcp://") + _uri.host();
	}
}

}}
