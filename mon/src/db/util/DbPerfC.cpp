#include "DbPerfC.h"

namespace msvc { namespace db {

using namespace std;
using namespace msvc::perf;

PerfCounter *DbPerfC::Pool::_free;
PerfCounter *DbPerfC::Pool::_used;
boost::mutex DbPerfC::_lock;
map<string, PerfCounter *> DbPerfC::_perf;

}}
