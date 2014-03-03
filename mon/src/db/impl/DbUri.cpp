#include "DbUri.h"
#include "incl/log/log.h"

namespace msvc { namespace db {

using namespace std;
using namespace msvc::log;

DbUri::DbUri(const string &uri) : _uri(uri), _opts()
{
	try {
		_valid = ParseUriStr(uri);
	} catch (const exception &ex) {
		LC_TRACE_WARN("error parsing db uri: %%", uri, ex);
	}
	if (_valid)
		_userAtDb.append(_scheme).append(":")
				 .append(_user).append("@")
				 .append(_host).append("$").append(_name);
}

bool DbUri::ParseUriStr(const string &uri)
{
	int offset = uri.find("://");
	if (offset < 1)
		return false;
	_scheme = uri.substr(0, offset);

	offset += 3;
	int endpos = uri.find(':', offset);
	if (endpos <= offset)
		return false;
	_user = uri.substr(offset, endpos - offset);

	offset = endpos + 1;
	endpos = uri.find('@', offset);
	if (endpos <= offset)
		return false;
	_pass = uri.substr(offset, endpos - offset);

	offset = endpos + 1;
	endpos = uri.find('/', offset);
	if (endpos <= offset)
		return false;
	_host = uri.substr(offset, endpos - offset);

	offset = endpos + 1;
	endpos = uri.find('?', offset);
	if (endpos == offset)
		return false;
	if (endpos < offset) {
		_name = uri.substr(offset);
		return true;
	}
	_name = uri.substr(offset, endpos - offset);

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
	return true;
}

}}


