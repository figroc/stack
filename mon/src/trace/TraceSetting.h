#ifndef MSVC_LOG_TRACE_SETTING_H
#define MSVC_LOG_TRACE_SETTING_H

#include "TraceType.h"
#include "incl/conf/conf.h"
#include <string>

namespace msvc { namespace log { namespace _di {

using namespace msvc::log;
using namespace msvc::cfg;

class TraceSetting {
public:
	inline static int Level() {
		static const std::string LC_CFG_TL = "log.level";
		return SvcSetting::Integer(LC_CFG_TL, LC_TL_ERROR);
	};

	inline static std::string Dir() {
		static const std::string LC_CFG_DIR_KEY = "log.dir";
		static const std::string LC_CFG_DIR_DEF = "log/";
		return SvcSetting::String(LC_CFG_DIR_KEY, LC_CFG_DIR_DEF);
	}

private:
	TraceSetting();
};

}}}

#endif
