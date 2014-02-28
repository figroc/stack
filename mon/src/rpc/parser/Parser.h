#ifndef MSVC_RPC_PARSER_H
#define MSVC_RPC_PARSER_H

#include "../message/RpcMessage.h"
#include "../message/RpcRequest.h"
#include "../message/RpcResponse.h"
#include <memory>
#include <boost/signals2.hpp>

namespace msvc { namespace rpc { namespace _di {

using namespace msvc::rpc;

class Parser {
public:
	boost::signals2::signal<void(std::auto_ptr<RpcRequest>)> OnRequestParsed;
	boost::signals2::signal<void(std::auto_ptr<RpcResponse>)> OnResponseParsed;
	boost::signals2::signal<void()> OnError;

public:
	void TryParse(const BufferNode &data);
};

}}}

#endif
