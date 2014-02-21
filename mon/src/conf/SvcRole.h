#ifndef CFG_SVC_ROLE_H
#define CFG_SVC_ROLE_H

#include <string>

namespace msvc { namespace cfg {

class SvcRole {
	friend class RoleSvc;
	friend class RoleBus;
	friend class RoleCac;
	friend class RoleDbs;
	friend class RoleDfs;

private:
	std::string _name;

public:
	SvcRole() : _name() { };
private:
	SvcRole(const std::string &name) : _name(name) { };

public:
	inline bool global() const { return _name.empty(); };
	inline const std::string &name() const { return _name; };
};

class RoleSvc {
public:
	static const SvcRole PFS;
	static const SvcRole GFS;
	static const SvcRole QBG;
	static const SvcRole DSS;
private:
	RoleSvc();
};

class RoleBus {
public:
	static const SvcRole QBD;
	static const SvcRole QBQ;
	static const SvcRole GWC;
	static const SvcRole GWS;
private:
	RoleBus();
};

class RoleCac {
public:
	static const SvcRole USR_R;
	static const SvcRole USR_W;
	static const SvcRole GRP_R;
	static const SvcRole GRP_W;
private:
	RoleCac();
};

class RoleDbs {
public:
	static const SvcRole CFG_R;
	static const SvcRole CFG_W;
	static const SvcRole USR_R;
	static const SvcRole USR_W;
	static const SvcRole GRP_R;
	static const SvcRole GRP_W;
	static const SvcRole MSG_R;
	static const SvcRole MSG_W;
private:
	RoleDbs();
};

class RoleDfs {
public:
	static const SvcRole USR_R;
	static const SvcRole USR_W;
private:
	RoleDfs();
};

}}

#endif
