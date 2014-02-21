#include "DbPerfC.h"

namespace msvc { namespace db {

using namespace std;
using namespace boost;
using namespace msvc::perf;

shared_ptr<PerfCounter> DbPerfC::Pool::_free;
shared_ptr<PerfCounter> DbPerfC::Pool::_used;
mutex DbPerfC::_lock;
map< string, shared_ptr<PerfCounter> > DbPerfC::_perf;

}}
