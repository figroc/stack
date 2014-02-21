#include "SvcSetting.h"
#include "SvcLocator.h"
#include <iostream>
#include <vector>
#include <set>
#include <boost/lexical_cast.hpp>
#include "incl/util/util.h"

namespace msvc { namespace cfg {

using namespace std;
using namespace boost;
using namespace msvc::util;

SvcRole SvcSetting::_role;
Conf *SvcSetting::_conf = 0;

void SvcSetting::Init(const SvcRole &role)
{
	_role = role;
	_conf = CfgLoader::GetConf("app");
}

string SvcSetting::UriSvc(const SvcRole &role)
{
	string uri = _conf->GetValue<string>(role.name() + ".uri");
	if (uri.empty()) {
		vector<string> me = GetLocalIpAddress();

		static const string NO_USER;
		string svc;
		set<string> check;
		while (true) {
			svc = SvcLocator::GetUri(role, NO_USER);
			if (svc.size() < 1)
				break;
			if (check.find(svc) != check.end())
				break;
			check.insert(svc);

			pair<string, string> ep = ParseEndpoint(svc);
			for (int i = 0; i < me.size(); ++i) {
				if (me[i] == ep.first)
					return svc;
			}
		}
	}
	return uri;
}

string SvcSetting::UriAux(const string &name)
{
	string uri = String(name);
	if (!uri.empty() && (uri[0] == '+' || uri[0] == '-')) {
		try {
			int port = lexical_cast<int>(uri);
			string base = UriSvc(_role);
			if (base.size() < 1) {
				cerr << "no base SvcRole defined for " << name << '(' << uri << ')' << endl;
				uri.clear();
			} else {
				pair<string, string> endpoint = ParseEndpoint(base);
				try {
					port += lexical_cast<int>(endpoint.second);
					uri = ComposeEndpoint(endpoint.first, lexical_cast<string>(port));
				} catch (const bad_lexical_cast &ex) {
					cerr << "invalid base SvcRole uri " << base << ": "
					     << ex.what() << endl;
					uri.clear();
				}
			}
		} catch (const bad_lexical_cast &ex) {
			cerr << "invalid UriAux for " << name << '(' << uri << "): "
			     << ex.what() << endl;
			uri.clear();
		}
	}
	return uri;
}

}}
