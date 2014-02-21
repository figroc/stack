#ifndef CFG_SVC_LOCATOR_H
#define CFG_SVC_LOCATOR_H

#include "SvcRole.h"
#include "CfgLoader.h"
#include <boost/atomic.hpp>

namespace msvc { namespace cfg {

class SvcLocator {
	friend class CfgLoader;

private:
	enum LocatorType { LT_MODULE, LT_ROUNDROBIN };

	typedef struct Endpoint {
		LocatorType type;
		boost::atomic_uint counter;
		std::vector<std::string> uri;
	} endpoint_type;

	typedef std::map< std::string, boost::shared_ptr<endpoint_type> > endpoint_map;

private:
	static boost::mutex _lock;
	static boost::shared_ptr<const endpoint_map> _uri;

private:
	static void Init();

public:
	inline static std::string GetUri(const SvcRole &role, const std::string &user) { return LocateService(role.name(), user); }

private:
	static void ConfWatcher(Conf *const conf);
	static std::string LocateService(const std::string &name, const std::string &user);

	static std::string GetRoundRobin(endpoint_type *const ep);
	static std::string GetModLocated(const endpoint_type *const ep, const std::string &user);

private:
	SvcLocator();
};

}}

#endif /* CFG_SVC_LOCATOR_H */
