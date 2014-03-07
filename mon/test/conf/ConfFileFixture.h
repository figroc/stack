#ifndef MSVC_TEST_CONF_FILE_FIXTURE_H
#define MSVC_TEST_CONF_FILE_FIXTURE_H

#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>
#include "incl/util/util.h"
#include "incl/conf/conf.h"

namespace msvc { namespace test {

using namespace std;
using namespace msvc::cfg;

namespace {
  const string _SVC_CONF_ = "svc.conf";
  const string _APP_CONF_ = "app.conf";

  const string _CONF_PERF_ = "perf";
  const string _CONF_URI_ = "uri";
  const string _CONF_LOC_RR_ = "loc=rr";
  const string _CONF_LOC_MOD_ = "loc=mod";

  const string _CONF_STR_ = "str";
  const string _VAL_STR_ ="some";
  const string _CONF_STR_VAL_ = _CONF_STR_ + " = " + _VAL_STR_;


  const string _CONF_INT_ = "int";
  const long _VAL_INT_ = 12345;
  const string _CONF_INT_VAL_ = _CONF_INT_ + "=12345";

  const string _CONF_BTR_ = "btr";
  const bool _VAL_BTR_ = true;
  const string _CONF_BTR_VAL_ = _CONF_BTR_ + "=true";

  const string _CONF_BFA_ = "bfa";
  const bool _VAL_BFA_ = false;
  const string _CONF_BFA_VAL_ = _CONF_BFA_ + "=false";

  const string _CONF_SVC_PFS_ = "[svc.pfs]";
  const string _CONF_SVC_GFS_ = "[svc.gfs]";
  const string _CONF_CAC_USR_R_ = "[cac.usr.r]";

  const int _SIZE_OF_URI_ = 2;

  const string _VAL_SVC_PFS_URI_[] = { ComposeEndpoint(GetLocalIpAddress()[0], "5000"), "192.168.0.2:5000" };
  const string _VAL_SVC_PFS_PERF_ = ComposeEndpoint(GetLocalIpAddress()[0], "5005");

  const string _VAL_SVC_GFS_URI_[] = { ComposeEndpoint(GetLocalIpAddress()[0], "6000"), "192.168.1.2:6000" };
  const string _VAL_SVC_GFS_PERF_ = ComposeEndpoint(GetLocalIpAddress()[0], "6008");

  const string _VAL_CAC_USR_R_URI_[] = { ComposeEndpoint(GetLocalIpAddress()[0], "8000"), "10.0.0.2:8000" };
  const string _VAL_CAC_USR_R_PERF_ = ComposeEndpoint(GetLocalIpAddress()[0], "7990");

  const string _CONF_GLOBAL_PERF_SHIFT_ = _CONF_PERF_ + "=+8";
  const string _CONF_SVC_PFS_PERF_SHIFT_ = _CONF_PERF_ + "=+5";
  const string _CONF_CAC_USR_R_PERF_SHIFT_ = _CONF_PERF_ + "=-10";

  const string _CONF_SVC_PFS_URI_[] = { _CONF_URI_ + '=' + _VAL_SVC_PFS_URI_[0], _CONF_URI_ + '=' + _VAL_SVC_PFS_URI_[1] };
  const string _CONF_SVC_GFS_URI_[] = { _CONF_URI_ + '=' + _VAL_SVC_GFS_URI_[0], _CONF_URI_ + '=' + _VAL_SVC_GFS_URI_[1] };
  const string _CONF_CAC_USR_R_URI_[] = { _CONF_URI_ + '=' + _VAL_CAC_USR_R_URI_[0], _CONF_URI_ + '=' + _VAL_CAC_USR_R_URI_[1] };
}

class ConfFileFixture {

public:
	ConfFileFixture() {
		SvcLocatorFixture();
		SvcSettingFixture();
		CfgLoader::Init(RoleSvc::PFS);
	};
	virtual ~ConfFileFixture() {
		remove(_SVC_CONF_.c_str());
		remove(_APP_CONF_.c_str());
	};

private:
	inline static void SvcLocatorFixture() {
		ofstream f(_SVC_CONF_.c_str()); f
		<< _CONF_SVC_PFS_ << endl
		<< _CONF_LOC_RR_ << endl
		<< _CONF_SVC_PFS_URI_[0] << endl
		<< _CONF_SVC_PFS_URI_[1] << endl
		<< _CONF_SVC_GFS_ << endl
		<< _CONF_LOC_RR_ << endl
		<< _CONF_SVC_GFS_URI_[0] << endl
		<< _CONF_SVC_GFS_URI_[1] << endl
		<< _CONF_CAC_USR_R_ << endl
		<< _CONF_LOC_MOD_ << endl
		<< _CONF_CAC_USR_R_URI_[0] << endl
		<< _CONF_CAC_USR_R_URI_[1] << endl;
		f.close();
	};

	inline static void SvcSettingFixture() {
		ofstream f(_APP_CONF_.c_str()); f
		<< _CONF_GLOBAL_PERF_SHIFT_ << endl
		<< _CONF_STR_VAL_ << endl
		<< _CONF_INT_VAL_ << endl
		<< _CONF_BTR_VAL_ << endl
		<< _CONF_BFA_VAL_ << endl
		<< _CONF_SVC_PFS_ << endl
		<< _CONF_SVC_PFS_PERF_SHIFT_ << endl
		<< _CONF_CAC_USR_R_ << endl
		<< _CONF_CAC_USR_R_PERF_SHIFT_ << endl;
		f.close();
	};
};

}}

#endif //MSVC_TEST_CONF_FILE_FIXTURE_H
