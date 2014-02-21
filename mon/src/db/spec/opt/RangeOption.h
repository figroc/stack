#ifndef DB_RANGE_OPTION_H
#define DB_RANGE_OPTION_H

namespace msvc { namespace db {

class RangeOption {
private:
	int _limit;
	int _offset;

public:
	RangeOption(const int limit = 0, const int offset = 0) : _limit(limit), _offset(offset) { }
	virtual ~RangeOption() { }

public:
	inline int limit() const { return _limit; }
	inline RangeOption &limit(const int limit) { _limit = limit; return *this; }

	inline int offset() const { return _offset; }
	inline RangeOption &offset(const int offset) { _offset = offset; return *this; }
};

}}

#endif /* DB_RANGE_OPTION_H */
