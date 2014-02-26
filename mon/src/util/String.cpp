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

}}

namespace msvc { namespace util { namespace _di {

using namespace std;

template<>
bool StringCast<bool>(const string &str, bool &val)
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

