#ifndef UTIL_STRING_H
#define UTIL_STRING_H

#include <string>
#include <vector>
#include <utility>
#include <exception>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/type_traits.hpp>
#include <boost/algorithm/string.hpp>

namespace msvc { namespace util { namespace _di {

template<typename T>
inline bool StringCast(const std::string &str, T &val)
{
	return false;
}

template<>
bool StringCast<bool>(const std::string &str, bool &val);

}}}

namespace msvc { namespace util {

template<typename T>
T StringCast(const std::string &str, const T def = T())
{
	T r = def; StringCastTry(str, r); return r;
}

template<typename T>
bool StringCastTry(const std::string &str, T &val)
{
	if (str.empty())
		return false;

	if (!_di::StringCast(str, val)) try {
		val = boost::lexical_cast<T>(str);
	} catch (const boost::bad_lexical_cast &ex) {
		return false;
	}
	return true;
}

template<typename T>
std::vector<T> StringSplit(const std::string &str, const std::string &del, const bool cpt = true)
{
	std::vector<std::string> tokens = StringSplit<std::string>(str, del, cpt);
	std::vector<T> value;
	for (size_t i = 0; i < tokens.size(); ++ i)
		value.push_back(StringCast<T>(tokens[i]));
	return value;
}

template<>
std::vector<std::string> StringSplit<std::string>(const std::string &str, const std::string &del, const bool cpt);

}}

#endif /* UTIL_STRING_H */
