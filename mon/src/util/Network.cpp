#include "Network.h"
#include <ifaddrs.h>
#include <arpa/inet.h>

namespace msvc { namespace util {

using namespace std;

vector<string> GetLocalIpAddress()
{
	vector<string> ip;

	struct ifaddrs *ifas = 0;
	if (0 == ::getifaddrs(&ifas)) {
		for (struct ifaddrs *ifa = ifas; ifa != 0; ifa = ifa->ifa_next) {
			void *addr = 0;
			switch (ifa->ifa_addr->sa_family) {
			case AF_INET:
				addr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
				break;
			case AF_INET6:
				addr = &((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
				break;
			default:
				continue;
			}
			char addrs[INET6_ADDRSTRLEN];
			if (0 != ::inet_ntop(ifa->ifa_addr->sa_family, addr, addrs, sizeof(addrs))) {
				static const string IPv4_Loopback = "127.0.0.1";
				static const string IPv6_Loopback = "::1";
				if (IPv4_Loopback != addrs && IPv6_Loopback != addrs)
					ip.push_back(addrs);
			}
		}
	}
	if (ifas != 0)
		::freeifaddrs(ifas);
	return ip;
}

string ComposeEndpoint(const string &addr, const string &port)
{
	if (addr.find(':') != string::npos)
		return '[' + addr + "]:" + port;
	return addr + ':' + port;
}

pair<string, string> ParseEndpoint(const string &endpoint)
{
	pair<string, string> ep;
	const int i = endpoint.rfind(':');
	if (i == string::npos || i == 0 || i == endpoint.size() - 1) {
		ep.first = endpoint;
	} else {
		ep.first = endpoint.substr(0, i);
		ep.second = endpoint.substr(i + 1);
	}
	if (ep.first.size() > 1 && ep.first[0] == '[' && ep.first[ep.first.size() - 1] == ']') {
		ep.first.erase(0, 1);
		ep.first.erase(ep.first.size() - 1);
	}
	return ep;
}

}}
