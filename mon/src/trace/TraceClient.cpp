#include "TraceClient.h"
#include "TraceFormat.h"
#include <iostream>
#include "incl/util/util.h"
#include "incl/conf/conf.h"

namespace msvc { namespace log {

using namespace std;
using namespace boost;
using namespace msvc::util;
using namespace msvc::cfg;

atomic_bool TraceClient::_init(false);

void TraceClient::Init()
{
	if (!_init.exchange(true, memory_order_relaxed)) {

	}
}

void TraceClient::Record(const string &message)
{

}

}}
