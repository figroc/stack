#include "String.h"

namespace msvc { namespace util {

using namespace std;

template<>
vector<string> StringSplit<string>(const string &str, const string &del, const bool cpt)
{
    vector<string> val;
    boost::split(val, str, boost::is_any_of(del), cpt ? boost::token_compress_on : boost::token_compress_off);
    return val;
}

int StringFind(const string &str, const char c, const size_t i)
{
	size_t n = (i < 0 || str.size() <= i) ? string::npos : str.find(c, i);
	if (n > 0) while (n != string::npos) {
		if ((str[n - 1] & 0x80) == 0) // str[n] is a standalone char
			break;
		if (n == str.size() - 1)
			n = string::npos;
		else
			n = str.find(c, n + 1);
	}
	return n;
}

string StringReplace(const string &str, const char c, const string &rpl, const size_t i)
{
	size_t n = StringFind(str, c, i);
	if (n == string::npos)
		return str;

	string rep;
	size_t p = 0;
	while (n != string::npos) {
		rep += str.substr(p, n - p);
		rep += rpl;
		p = ++n;
		n = StringFind(str, c, p);
	}
	if (p < str.size())
		rep += str.substr(p);
	return rep;
}

}}

namespace msvc { namespace util { namespace _di {

using namespace std;

template<>
bool StringCastPreempt<bool>(const string &str, bool &val)
{
	string lower = str; boost::to_lower(lower);
	if (lower == "true" || lower == "on" || lower == "enable") {
		val = true; return true;
	}
	if (lower == "false" || lower == "off" || lower == "disable") {
		val = false; return true;
	}
	return false;
}

}}}

