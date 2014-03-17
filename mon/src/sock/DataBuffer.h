#ifndef STACK_DATA_BUFFER_H
#define STACK_DATA_BUFFER_H

#include <boost/asio/buffer.hpp>
#include <boost/asio/streambuf.hpp>

namespace msvc { namespace sock {

typedef boost::shared_ptr<boost::asio::streambuf> DataBuffer;

}}

#endif
