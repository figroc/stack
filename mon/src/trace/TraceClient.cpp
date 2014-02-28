#include "TraceClient.h"
#include "TraceFormat.h"
#include <iostream>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include "incl/conf/conf.h"
#include "incl/util/util.h"

namespace msvc { namespace log {

using namespace std;
using namespace msvc::log::_di;
using namespace msvc::util;
using namespace msvc::cfg;

boost::atomic_bool TraceClient::_init(false);
TraceQueue<string> TraceClient::_queue;
TraceFile TraceClient::_file("trace", ".log");

void TraceClient::Init()
{
	if (!_init.exchange(true, boost::memory_order_relaxed)) {
		boost::thread(&TraceClient::Recorder).detach();
	}
}

void TraceClient::Record(const string &message)
{
	_queue.Push(message);
}

void TraceClient::Recorder()
{
	while (true) try {
		boost::this_thread::sleep_for(boost::chrono::milliseconds(500));

		_file.Switch(SvcSetting::String("log.dir", "log/"));

		auto_ptr<string> msg = _queue.Pop();
		while (msg.get() != 0) {
			_file.Write(*msg);
			msg = _queue.Pop();
		}
	} catch (const exception &ex) {
		cerr << "[TraceClient::Recorder] error while recording trace: " << ex.what() << endl;
	}
}

}}
