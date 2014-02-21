#ifndef CFG_CFG_LOADER_H
#define CFG_CFG_LOADER_H

#include "Conf.h"
#include "SvcRole.h"
#include <boost/atomic.hpp>

namespace msvc { namespace cfg {

class CfgLoader {
private:
	static boost::atomic_bool _init;
	static boost::mutex _lock;
	static std::map< std::string, boost::shared_ptr<Conf> > _conf;

public:
	static void Init(const SvcRole &role);
	static Conf *GetConf(const std::string &name);

private:
	static void FileWatcher();
	static std::string GetFileName(const std::string &name);
	static time_t GetFileTime(const std::string &name);
	static std::auto_ptr<Conf> CreateConf(const std::string &name);
	static std::auto_ptr<conf_value_map> LoadFromFile(const std::string &name);

private:
	CfgLoader();
};

}}

#endif /* CFG_CFG_LOADER_H */
