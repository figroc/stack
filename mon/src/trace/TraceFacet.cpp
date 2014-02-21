#include "TraceFacet.h"

namespace msvc { namespace log { namespace _di {

void Trace(TraceLevel level, const std::string &file, const int line, const std::string &format)
{
	static const std::string EMPTY;
	MSVC_LOG_DI_TRACE_FACET_TEMPLATE_BODY(EMPTY);
}

}}}
