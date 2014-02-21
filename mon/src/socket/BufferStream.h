#ifndef STACK_BUFFER_STREAM_H
#define STACK_BUFFER_STREAM_H

#include <boost/smart_ptr.hpp>
#include "BufferNode.h"

namespace msvc { namespace stack {

class BufferStream	{
private:
	static const size_t CAPACITY = BufferNode::CAPACITY * 8;

private:
	char _buffer[CAPACITY];
	int _begin;
	int _end;

public:
	BufferStream() : _begin(0), _end(0) { ::bzero(_buffer, CAPACITY); };

	inline size_t size() const { return _end < _begin ? (CAPACITY - (_begin - _end)) : _end - _begin; }
	inline size_t remain() const { return _end < _begin ? (_begin - _end) : (CAPACITY - (_end - _begin)); }

	void Append(const BufferNode &buffer);

	inline int PeekByte() const { return size() > 0 ? (unsigned char)(_buffer[_begin]) : -1; }
	inline void Advance(size_t count) { _begin = (_begin + count) % CAPACITY; }

	int ReadByte();
	int ReadShort();

	BufferNode ExtractString(const size_t len);
};

} } // namespace msvc::stack

#endif // STACK_BUFFER_STREAM_H
