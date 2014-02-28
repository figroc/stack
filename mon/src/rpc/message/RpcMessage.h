#ifndef MSVC_RPC_RPC_MESSAGE_H
#define MSVC_RPC_RPC_MESSAGE_H

#include "../socket/BufferNode.h"
#include <string>
#include <boost/uuid/uuid.hpp>

namespace msvc { namespace rpc {

using namespace msvc::stack;

class RpcMessage {
protected:
	RpcMessage();
public:
	virtual ~RpcMessage();

public:
	boost::uuids::uuid &id();
	const boost::uuids::uuid &id() const;

	std::string &source();
	const std::string &source() const;

	std::string &from();
	const std::string &from() const;

	std::string &to();
	const std::string &to() const;

	std::string &scope();
	const std::string &scope() const;

	std::string &action();
	const std::string &action() const;

	BufferNode &body();
	const BufferNode &body() const;

protected:
	virtual BufferNode SerializeHeader() const = 0;
public:
	BufferNode Serialize() const;
};

}}

#endif
