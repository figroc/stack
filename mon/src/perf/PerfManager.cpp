#include "PerfManager.h"

namespace msvc { namespace perf {

using namespace std;
using namespace boost;

const string PerfManager::SEPARATOR = "^";

mutex PerfManager::_lock;
map< string, shared_ptr<PerfCounter> > PerfManager::_perf;
map< string, shared_ptr< vector<string> > > PerfManager::_tree;

PerfCounter *PerfManager::Fetch(const string &name)
{
	PerfCounter *perf;
	{
		mutex::scoped_lock lock(_lock);
		map< string, shared_ptr<PerfCounter> >::iterator it = _perf.find(name);
		if (_perf.end() == it) {
			_perf.insert(make_pair(name,
				shared_ptr<PerfCounter>(perf = new PerfCounter(name))));
			UpdateTree(name);
		} else {
			perf = it->second.get();
		}
	}
	return perf;
}

vector<PerfCounter *> PerfManager::Query(const string &name)
{
	mutex::scoped_lock lock(_lock);
	vector<PerfCounter *> perf;
	if (name.empty()) {
		for (map< string, shared_ptr<PerfCounter> >::iterator
			 it = _perf.begin(); it != _perf.end(); ++it) {
			perf.push_back(it->second.get());
		}
	} else {
		map< string, shared_ptr< vector<string> > >::iterator ti = _tree.find(name);
		if (_tree.end() != ti) {
			for (int i = 0; i < ti->second->size(); ++i) {
				map< string, shared_ptr<PerfCounter> >::iterator it = _perf.find(ti->second->at(i));
				if (_perf.end() != it)
					perf.push_back(it->second.get());
			}
		}
	}
	return perf;
}

void PerfManager::UpdateTree(const string &name)
{
	UpdateTree(name, name);
	string::size_type n = name.find(SEPARATOR);
	while (string::npos != n) {
		const string par = name.substr(0, n);
		UpdateTree(par, name);
		n = name.find(SEPARATOR, n + 1);
	}
}

void PerfManager::UpdateTree(const string &prefix, const string &name)
{
	map< string, shared_ptr< vector<string> > >::iterator ti = _tree.find(prefix);
	if (_tree.end() == ti)
		ti = _tree.insert(make_pair(prefix, make_shared< vector<string> >())).first;

	shared_ptr< vector<string> > &leaf(ti->second);
	bool exist = false;
	for (int i = 0; i < leaf->size(); ++i) {
		if (name == leaf->at(i)) {
			exist = true;
			break;
		}
	}
	if (!exist)
		leaf->push_back(name);
}

}}
