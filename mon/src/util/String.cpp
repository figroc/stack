#include "String.h"

namespace msvc { namespace util {

using namespace std;
using namespace boost;

template<>
vector<string> StringSplit<string>(const string &str, const string &del, const bool cpt)
{
    vector<string> val;
    split(val, str, is_any_of(del), cpt ? token_compress_on : token_compress_off);
    return val;
}

}}

namespace msvc { namespace util { namespace _di {

using namespace std;
using namespace boost;

template<>
bool StringCast<bool>(const string &str, bool &val)
{
	string lower = str; to_lower(lower);
	if (lower == "true" || lower == "on" || lower == "enable") {
		val = true; return true;
	}
	if (lower == "false" || lower == "off" || lower == "disable") {
		val = false; return true;
	}
	return false;
}

}}}

