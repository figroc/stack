#ifndef MSVC_RPC_RPC_MESSAGE_H
#define MSVC_RPC_RPC_MESSAGE_H

#include "../sock/DataBuffer.h"
#include <string>
#include <boost/uuid/uuid.hpp>

namespace msvc { namespace rpc {

using namespace msvc::sock;

class RpcMessage {
private:
    boost::uuids::uuid _id;
    std::string _source;
    std::string _from;
    std::string _to;
    std::string _scope;
    std::string _action;
    DataBuffer _body;
    
protected:
	RpcMessage() : _id(), _source(), _from(), _to(), _scope(), _action(),
        _body(new DataBuffer::element_type()) { };
public:
	virtual ~RpcMessage() { };

public:
	inline boost::uuids::uuid &id() { return _id; };
	inline const boost::uuids::uuid &id() const { return _id; };

	inline std::string &source() { return _source; };
	inline const std::string &source() const { return _source; };

	inline std::string &from() { return _from; };
	inline const std::string &from() const { return _from; };

	inline std::string &to() { return _to; };
	inline const std::string &to() const { return _to; };

	inline std::string &scope() { return _scope; };
	inline const std::string &scope() const { return _scope; };

	inline std::string &action() { return _action; };
	inline const std::string &action() const { return _action; };

	inline DataBuffer &body() { return _body; };
	inline const DataBuffer &body() const { return _body; };

};

}}

#endif
