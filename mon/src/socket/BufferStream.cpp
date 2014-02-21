#include <stdexcept>
#include "BufferStream.h"

namespace msvc { namespace stack {

using namespace std;

void BufferStream::Append(const BufferNode &buffer)
{
	size_t size = buffer.size();
	if (size <= 0)
		return;

	if (size > remain())
		throw out_of_range("exceed buffer limit");

	size_t size1 = 0;
	size_t size2 = size;
	if (_end >= _begin) {
		size1 = std::min(size, CAPACITY - _end);
		size2 -= size1;
		::memcpy(_buffer + _end, buffer.bytesOffset(), size1);
		_end = (_end + size1) % CAPACITY;
	}
	if (size2 > 0) {
		::memcpy(_buffer, buffer.bytesOffset() + size1, size2);
		_end = size2;
	}
}

int BufferStream::ReadByte()
{
	int b = -1;
	if (size() > 0) {
		b = (unsigned char)(_buffer[_begin]);
		Advance(1);
	}
	return b;
}

int BufferStream::ReadShort()
{
	int s = -1;
	if (size() > 1) {
		union { unsigned char c[2]; unsigned short i; } d;
		d.c[1] = _buffer[_begin];
		Advance(1);
		d.c[0] = _buffer[_begin];
		Advance(1);
		s = d.i;
	}
	return s;
}

BufferNode BufferStream::ExtractString(const size_t len)
{
	size_t size1(0), size2(0);
	if (_end >= _begin) {
		size1 = std::min<size_t>(_end - _begin, len);
	} else {
		size1 = std::min<size_t>(CAPACITY - _begin, len);
		size2 = std::min<size_t>(len - size1, _end);
	}

	size_t size = size1 + size2;
	BufferNode ret(size + 1);
	if (size1 > 0)
		::memcpy(ret.bytesOffset(), _buffer + _begin, size1);
	if (size2 > 0)
		::memcpy(ret.bytesOffset() + size1, _buffer, size2);
	ret.size(size).bytesOffset()[size] = 0;
	Advance(size);
	return ret;
}

} } // namespace msvc::stack
