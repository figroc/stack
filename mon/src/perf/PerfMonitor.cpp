#include "PerfMonitor.h"
#include "PerfManager.h"
#include <sstream>
#include <boost/thread.hpp>
#include <boost/functional.hpp>
#include "incl/util/util.h"

namespace msvc { namespace perf {

using namespace std;
using namespace msvc::util;
namespace netu = boost::network::uri;
namespace http = boost::network::http;
namespace util = boost::network::utils;

boost::atomic_bool PerfMonitor::_init(false);
boost::shared_ptr<PerfMonitor> PerfMonitor::_monitor;
boost::shared_ptr<PerfMonitor::HttpOption> PerfMonitor::_option;
boost::shared_ptr<PerfMonitor::HttpServer> PerfMonitor::_server;

void PerfMonitor::Start(const string &endpoint)
{
	if (!endpoint.empty() && !_init.exchange(true, boost::memory_order_relaxed)) {
		const pair<string, string> listening = ParseEndpoint(endpoint);
		_monitor.reset(new PerfMonitor());
		_option.reset(new HttpOption(*_monitor));
		_option->address(listening.first);
		_option->port(listening.second);
		_server.reset(new HttpServer(*_option));
		_server->listen();
		boost::thread(boost::bind(&HttpServer::run, _server)).detach();
	}
}

void PerfMonitor::Stop()
{
	if (_init.exchange(false, boost::memory_order_relaxed)) {
		_server->stop();
	}
}

void PerfMonitor::SendResponse(HttpResponse &response, const HttpStatus status)
{
	static const HttpResponse::header_type NO_CONTENT = {"Content-Length", "0"};

	response.status = status;
	response.headers.push_back(NO_CONTENT);
}

void PerfMonitor::SendResponse(HttpResponse &response, const string &content)
{
	static const HttpResponse::header_type MIME_JSON = {"Content-Type", "application/json"};
	static const HttpResponse::header_type NO_CACHE = {"Cache-Control", "no-store"};
	static const HttpResponse::header_type ALLOW_ORIGIN = {"Access-Control-Allow-Origin", "*"};
	const HttpResponse::header_type length = {"Content-Length", boost::lexical_cast<string>(content.size())};

	response.status = HttpResponse::ok;
	response.content = content;
	response.headers.push_back(length);
	response.headers.push_back(MIME_JSON);
	response.headers.push_back(NO_CACHE);
	response.headers.push_back(ALLOW_ORIGIN);
}

void PerfMonitor::operator() (const HttpRequest &request, HttpResponse &response)
{
	static const string URI_PADDING = "http://cpp-netlib.org";
	static const string URI_LIST_PERF = "/perf/list.do";
	static const string URI_QUERY_PERF = "/perf/query.do";

	if (request.method != "GET") {
		SendResponse(response, HttpResponse::not_supported);
	} else {
		const netu::uri target(URI_PADDING + request.destination);
		const string t_path = netu::decoded_path(target);
		const string t_query = netu::decoded_query(target);
		if (t_path == URI_QUERY_PERF)
			SendResponse(response, QueryPerf(t_query));
		else if (t_path == URI_LIST_PERF)
			SendResponse(response, ListPerf(t_query));
		else
			SendResponse(response, HttpResponse::not_found);
	}
}

void PerfMonitor::log(const string &info)
{
	//LC_TRACE_DEBUG("PerfMonitor: %s", info.c_str());
}

string PerfMonitor::ListPerf(const string &name)
{
	vector<PerfCounter *> counters = PerfManager::Query(name);
	if (counters.size() < 1)
		return "{\"r\":404,\"l\":[]}";

	stringstream ss;
	ss << "{\"r\":200,\"l\":[\r\n";
	for (int i = 0; i < counters.size(); ++i) {
		if (i > 0) ss << "\"},\r\n";
		ss << "{\"n\":\"" << counters[i]->name();
	}
	ss << "\"}\r\n]}";
	return ss.str();
}

string PerfMonitor::QueryPerf(const string &name)
{
	vector<PerfCounter *> counters = PerfManager::Query(name);
	if (counters.size() < 1)
		return "{\"r\":404,\"l\":[]}";

	stringstream ss;
	ss << "{\"r\":200,\"l\":[\r\n";
	for (int i = 0; i < counters.size(); ++i) {
		if (i > 0) ss << "},\r\n";

		const PerfCounter *perf = counters[i];
		ss << "{\"n\":\"" << perf->name() << '"';

		const int hint = perf->hint();
		if (hint & PerfCounter::HINT_COUNT)
			ss << ",\"c\":" << perf->Current();
		if (hint & PerfCounter::HINT_INC)
			ss << ",\"i\":" << perf->RateOfInc();
		if (hint & PerfCounter::HINT_DEC)
			ss << ",\"d\":" << perf->RateOfDec();
		if (hint & PerfCounter::HINT_TIME)
			ss << ",\"t\":" << perf->AvgTime();
	}
	ss << "}\r\n]}";
	return ss.str();
}

}}
