#include "SvcRole.h"

namespace msvc { namespace cfg {

using namespace std;

const SvcRole RoleSvc::PFS("svc.pfs");
const SvcRole RoleSvc::GFS("svc.gfs");
const SvcRole RoleSvc::QBG("svc.qbg");
const SvcRole RoleSvc::DSS("svc.dss");
const SvcRole RoleBus::QBD("bus.qb.data");
const SvcRole RoleBus::QBQ("bus.qb.qpid");
const SvcRole RoleBus::GWC("bus.gw.client");
const SvcRole RoleBus::GWS("bus.gw.server");
const SvcRole RoleCac::USR_R("cac.usr.r");
const SvcRole RoleCac::USR_W("cac.usr.w");
const SvcRole RoleCac::GRP_R("cac.grp.r");
const SvcRole RoleCac::GRP_W("cac.grp.w");
const SvcRole RoleDbs::CFG_R("dbs.cfg.r");
const SvcRole RoleDbs::CFG_W("dbs.cfg.w");
const SvcRole RoleDbs::USR_R("dbs.usr.r");
const SvcRole RoleDbs::USR_W("dbs.usr.w");
const SvcRole RoleDbs::GRP_R("dbs.grp.r");
const SvcRole RoleDbs::GRP_W("dbs.grp.w");
const SvcRole RoleDbs::MSG_R("dbs.msg.r");
const SvcRole RoleDbs::MSG_W("dbs.msg.w");
const SvcRole RoleDfs::USR_R("dfs.usr.r");
const SvcRole RoleDfs::USR_W("dfs.usr.w");

}}
