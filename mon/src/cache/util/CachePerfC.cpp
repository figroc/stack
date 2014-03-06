#include "CachePerfC.h"

namespace msvc { namespace cache {

using namespace std;
using namespace msvc::perf;

PerfCounter *CachePerfC::Pool::_free;
PerfCounter *CachePerfC::Pool::_used;
boost::mutex CachePerfC::_lock;
map<string, PerfCounter *> CachePerfC::_perf;

}}
