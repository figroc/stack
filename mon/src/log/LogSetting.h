#ifndef MSVC_LOG_LOG_SETTING_H
#define MSVC_LOG_LOG_SETTING_H

#include "incl/conf/conf.h"
#include <string>

namespace msvc { namespace log { namespace _di {

using namespace msvc::log;
using namespace msvc::cfg;

class LogSetting {
public:
	inline static std::string Dir() {
		static const std::string LC_CFG_DIR_KEY = "log.evt";
		static const std::string LC_CFG_DIR_DEF = "evt/";
		return SvcSetting::String(LC_CFG_DIR_KEY, LC_CFG_DIR_DEF);
	}

private:
	LogSetting();
};

}}}

#endif
