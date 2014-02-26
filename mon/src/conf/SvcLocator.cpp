#include "SvcLocator.h"
#include <utility>
#include <boost/functional/hash.hpp>

namespace msvc { namespace cfg {

using namespace std;

boost::mutex SvcLocator::_lock;
boost::shared_ptr<const SvcLocator::endpoint_map> SvcLocator::_uri;

void SvcLocator::Init()
{
	Conf *const conf = CfgLoader::GetConf("svc");
	ConfWatcher(conf);
	conf->WatchChange(ConfWatcher);
}

string SvcLocator::LocateService(const string &name, const string &user)
{
	boost::shared_ptr<const endpoint_map> uri;
	{
		boost::mutex::scoped_lock lock(_lock);
		uri = _uri;
	}
	if (!uri)
		return "";

	const endpoint_map::const_iterator it = uri->find(name);
	if (uri->end() == it)
		return "";

	const boost::shared_ptr<endpoint_type> &ep = it->second;
	if (!ep)
		return "";

	if (LT_ROUNDROBIN == ep->type)
		return GetRoundRobin(ep.get());

	if (user.empty())
		return GetRoundRobin(ep.get());
	return GetModLocated(ep.get(), user);
}

void SvcLocator::ConfWatcher(Conf *const conf)
{
	if (!conf)
		return;

	const boost::shared_ptr<const conf_value_map> pt = conf->GetAll();
	if (!pt)
		return;

	const boost::shared_ptr<endpoint_map> uri(new endpoint_map());

	vector<string> inv;
	for (conf_value_map::const_iterator it = pt->begin(); it != pt->end(); ++it) {
		int pos = it->first.find_last_of('.');
		if (pos > 0 && pos < it->first.size() - 1) {
			string name = it->first.substr(0, pos);
			string conf = it->first.substr(pos + 1);

			endpoint_map::iterator epi = uri->find(name);
			if (epi == uri->end()) {
				epi = uri->insert(make_pair(name, boost::make_shared<endpoint_type>())).first;
				epi->second->type = LT_MODULE;
				epi->second->counter = 0;
			}

			if (conf == "loc") {
				if (it->second.values<string>().size() != 1) {
					inv.push_back(name);
				} else if (it->second.values<string>()[0] == "rr") {
					epi->second->type = LT_ROUNDROBIN;
				} else if (it->second.values<string>()[0] == "mod") {
					epi->second->type = LT_MODULE;
				} else {
					inv.push_back(name);
				}
			} else if (conf == "uri") {
				epi->second->uri = it->second.values<string>();
			}
		}
	}

	for (int i = 0; i < inv.size(); ++i) {
		uri->erase(inv[i]);
		cerr << "[SvcLocator::ConfWatcher] invalid config for " << inv[i] << endl;
	}

	{
		boost::mutex::scoped_lock lock(_lock);
		_uri = uri;
	}
}

string SvcLocator::GetRoundRobin(endpoint_type *const ep)
{
	return ep->uri[ep->counter++ % ep->uri.size()];
}

string SvcLocator::GetModLocated(const endpoint_type *const ep, const string &user)
{
	return ep->uri[static_cast<unsigned int>(boost::hash<string>()(user)) % ep->uri.size()];
}

}}
