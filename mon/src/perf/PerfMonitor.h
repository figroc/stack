#ifndef PERF_PERF_MONITOR_H
#define PERF_PERF_MONITOR_H

#include <string>
#include <utility>
#include <boost/atomic.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/network/include/http/server.hpp>

namespace msvc { namespace perf {

class PerfMonitor {
public:
	typedef boost::network::http::server<PerfMonitor> HttpServer;
	typedef HttpServer::options HttpOption;
	typedef HttpServer::request HttpRequest;
	typedef HttpServer::response HttpResponse;
	typedef HttpResponse::status_type HttpStatus;

private:
	static boost::atomic_bool _init;
	static boost::shared_ptr<PerfMonitor> _monitor;
	static boost::shared_ptr<HttpOption> _option;
	static boost::shared_ptr<HttpServer> _server;

public:
	static void Start(const std::string &endpoint);
	static void Stop();

private:
	static void SendResponse(HttpResponse &response, const HttpStatus status);
	static void SendResponse(HttpResponse &response, const std::string &content);

private:
	PerfMonitor() { };
	PerfMonitor(const PerfMonitor &);
	PerfMonitor &operator=(const PerfMonitor &);

public:
    void operator() (const HttpRequest &request, HttpResponse &response);
    void log(const std::string &info);

private:
    std::string ListPerf(const std::string &name);
    std::string QueryPerf(const std::string &name);
};

}}

#endif /* PERF_PERF_MONITOR_H */
