#ifndef MSVC_UTIL_SYSTEM_H
#define MSVC_UTIL_SYSTEM_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <boost/thread.hpp>

namespace msvc { namespace util {

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 64
#endif //HOST_NAME_MAX

inline std::string GetHostName()
{
	char name[HOST_NAME_MAX];
	return (0 == ::gethostname(name, HOST_NAME_MAX))
		? std::string(name) : std::string();
}

inline int GetProcessId() {
	return ::getpid();
}

inline int GetThreadId() {
	return ::syscall(SYS_gettid);
}

}}

#endif //MSVC_UTIL_SYSTEM_H
