#ifndef STACK_BUFFER_NODE_H
#define STACK_BUFFER_NODE_H

#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <boost/smart_ptr.hpp>
#include <boost/shared_array.hpp>

namespace msvc { namespace stack {

class BufferNode {
public:
	static const size_t CAPACITY = BUFSIZ;

private:
	size_t _capacity;
	boost::shared_array<char> _bytes;
	size_t _offset;
	size_t _size;
	size_t _action;

public:
	BufferNode(const size_t cap = CAPACITY) : _capacity(cap), _bytes(new char[cap]),
			_offset(0), _size(0), _action(0) {
		::bzero(_bytes.get(), cap);
	}

public:
	inline size_t capacity() const { return _capacity; }

	inline char *bytes() const { return _bytes.get(); }
	inline char *bytesOffset() const { return _bytes.get() + _offset; }

	inline size_t offset() const { return _offset; }
	inline BufferNode &offset(size_t offset) { _offset = offset; return *this; }
	inline size_t size() const { return _size; }
	inline BufferNode &size(size_t size) { _size = size; return *this; }

	inline size_t action() const { return _action; }
	inline BufferNode &action(size_t action) { _action = action; return *this; }
};

} } // namespace msvc::stack

#endif // STACK_BUFFER_NODE_H
