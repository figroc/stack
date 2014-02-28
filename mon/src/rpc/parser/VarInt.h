#ifndef MSVC_RPC_VAR_INT_H
#define MSVC_RPC_VAR_INT_H

namespace msvc { namespace rpc { namespace _di {

class VarInt {
public:
	VarInt();
	VarInt(const int len);

public:
	bool Feed(const char c);
	int Value();
};

}}}

#endif
