#include "Uri.h"

namespace msvc { namespace util {

using namespace std;

Uri::Uri(const string &uri) : _uri(uri), _opts()
{
	try {
		_valid = ParseUriStr(uri);
	} catch (const exception &ex) {
		cerr << "error parsing uri: " << uri << " !!! " << ex.what();
	}
	if (_valid) {
		_identity.append(_scheme).append(":")
				 .append(_user).append("@")
				 .append(_host).append("$")
				 .append(_path);
	}
}

bool Uri::ParseUriStr(const string &uri)
{
	int offset = uri.find("://");
	if (offset < 1)
		return false;
	_scheme = uri.substr(0, offset);

	offset += 3;
	int endpos = uri.find(':', offset);
	if (endpos < offset)
		return false;
	_user = uri.substr(offset, endpos - offset);

	offset = endpos + 1;
	endpos = uri.find('@', offset);
	if (endpos < offset)
		return false;
	_pass = uri.substr(offset, endpos - offset);

	offset = endpos + 1;
	endpos = uri.find('/', offset);
	if (endpos < offset)
		return false;
	_host = uri.substr(offset, endpos - offset);

	offset = endpos + 1;
	endpos = uri.find('?', offset);
	if (endpos < offset) {
		_path = uri.substr(offset);
	} else {
		_path = uri.substr(offset, endpos - offset);

		while(endpos >= offset) {
			offset = endpos + 1;

			string optn, optv;
			endpos = uri.find_first_of("=&", offset);
			if (endpos < offset) {
				optn = uri.substr(offset);
			} else {
				optn = uri.substr(offset, endpos - offset);
				if (uri[endpos] == '=') {
					offset = endpos + 1;
					endpos = uri.find('&', offset);
					if (endpos < offset)
						optv = uri.substr(offset);
					else
						optv = uri.substr(offset, endpos - offset);
				}
			}
			_opts[optn] = optv;
		}
	}
	return true;
}

}}


