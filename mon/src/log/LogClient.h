#ifndef MSVC_LOG_LOG_CLIENT_H
#define MSVC_LOG_LOG_CLIENT_H

#include "../trace/TraceQueue.h"
#include "../trace/TraceFile.h"
#include <string>
#include <map>
#include <utility>
#include <boost/smart_ptr.hpp>
#include <boost/atomic.hpp>

namespace msvc { namespace log {

class LogClient {
private:
	static boost::atomic_bool _init;
	static std::string _dir;
	static msvc::log::_di::TraceQueue< std::pair<std::string, std::string> > _queue;
	static std::map< std::string, boost::shared_ptr<msvc::log::_di::TraceFile> > _files;

public:
	static void Init();
	static void Append(const std::string &name, const std::string &message);

private:
	static void Writer();

private:
	LogClient();
};

}}

#endif
