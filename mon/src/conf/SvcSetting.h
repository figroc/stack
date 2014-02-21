#ifndef CFG_SVC_SETTING_H
#define CFG_SVC_SETTING_H

#include "SvcRole.h"
#include "CfgLoader.h"
#include <boost/lexical_cast.hpp>

namespace msvc { namespace cfg {

class SvcSetting {
	friend class CfgLoader;

private:
	static SvcRole _role;
	static Conf *_conf;

private:
	static void Init(const SvcRole &role);

public:
	static std::string UriSvc(const SvcRole &role);
	static std::string UriAux(const std::string &name);

	inline static bool Boolean(const std::string &name, const bool def = false) { return Get(name, def); };
	inline static long Integer(const std::string &name, const long def = 0L) { return Get(name, def); };
	inline static std::string String(const std::string &name, const std::string &def = std::string()) { return Get(name, def); }

	inline static std::vector<bool> Booleans(const std::string &name) { return Gets<bool>(name); }
	inline static std::vector<long> Integers(const std::string &name) { return Gets<long>(name); }
	inline static std::vector<std::string> Strings(const std::string &name) { return Gets<std::string>(name); }

	inline void WatchChange(void (*const callback)(Conf *conf)) { _conf->WatchChange(callback); }
	template<typename T> // caller must keep owner alive, we take weak_ptr from shared_ptr
	inline void WatchChange(const boost::shared_ptr<T> &owner, void (T::*const callback)(Conf *conf)) { _conf->WatchChange(owner, callback); }

private:
	template<typename T>
	inline static T Get(const std::string &name, const T def) {
		std::vector<T> val = Gets<T>(name);
		return val.empty() ? def : val.front();
	}
	template<typename T>
	inline static std::vector<T> Gets(const std::string &name) {
		if (_role.global())
			return _conf->GetValues<T>(name);
		std::vector<T> val = _conf->GetValues<T>(_role.name() + '.' + name);
		return val.empty() ? _conf->GetValues<T>(name) : val;
	}

public: // for unit test only
	inline static void __internal_switch_role__(const SvcRole &role) { _role = role; };

private:
	SvcSetting();
};

}}

#endif //CFG_SVC_SETTING_H
