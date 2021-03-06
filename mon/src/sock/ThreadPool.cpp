#include <algorithm>
#include "ThreadPool.h"

namespace msvc { namespace sock {

using namespace std;
using namespace boost;
using namespace boost::threadpool;

pool ThreadPool::_io(max<size_t>(1, thread::hardware_concurrency()) * 5);
pool ThreadPool::_worker(max<size_t>(1, thread::hardware_concurrency()) * 5);

} }
