#include "LogClient.h"
#include <boost/thread.hpp>
#include "incl/conf/conf.h"

namespace msvc { namespace log {

using namespace std;
using namespace msvc::cfg;
using namespace msvc::log::_di;

boost::atomic_bool LogClient::_init(false);
TraceQueue< pair<string, string> > LogClient::_queue;
map< string, boost::shared_ptr<TraceFile> > LogClient::_files;

void LogClient::Init()
{
	if (!_init.exchange(true, boost::memory_order_seq_cst)) {
		_dir = SvcSetting::String("log.evt", "evt/");
		boost::thread(&LogClient::Writer).detach();
	}
}

void LogClient::Append(const string &name, const string &message)
{
	_queue.Push(make_pair(name, message));
}

void LogClient::Writer()
{
	while (true) try {
		boost::this_thread::sleep_for(boost::chrono::milliseconds(300));

		auto_ptr< pair<string, string> > msg = _queue.Pop();
		while (msg.get() != 0) {
			map< string, boost::shared_ptr<TraceFile> >::iterator
				it = _files.find(msg->first);
			if (it == _files.end()) {
				it = _files.insert(make_pair(msg->first, boost::make_shared<TraceFile>(msg->first, ".csv"))).first;
			}
			it->second->Write(msg->second);
			msg = _queue.Pop();
		}
	} catch (const exception &ex) {
		cerr << "[LogClient::Writer] error while writing event log: " << ex.what() << endl;
	}
}

}}
