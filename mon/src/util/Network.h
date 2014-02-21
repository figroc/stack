#ifndef UTIL_NETWORK_H
#define UTIL_NETWORK_H

#include <string>
#include <vector>
#include <utility>

namespace msvc { namespace util {

std::vector<std::string> GetLocalIpAddress();
std::string ComposeEndpoint(const std::string &addr, const std::string &port);
std::pair<std::string, std::string> ParseEndpoint(const std::string &endpoint);

}}

#endif //UTIL_NETWORK_H
