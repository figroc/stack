#ifndef DB_SORT_OPTION_H
#define DB_SORT_OPTION_H

#include <string>

namespace msvc { namespace db {

enum SortOpType { SOT_ASC, SOT_DESC };

class SortOption {
private:
	std::string _name;
	SortOpType _op;

public:
	SortOption(const std::string &name, const SortOpType sop = SOT_ASC) : _name(name), _op(sop) { }
	virtual ~SortOption() { }

public:
	inline const std::string &name() const { return _name; }
	inline SortOpType op() const { return _op; }
};

}}

#endif /* DB_SORT_OPTION_H */
