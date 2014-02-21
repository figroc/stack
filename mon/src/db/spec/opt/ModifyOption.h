#ifndef DB_MODIFY_OPTION_H
#define DB_MODIFY_OPTION_H

#include "../../doc/PropItem.h"

namespace msvc { namespace db {

enum ModifyOpType { MOT_SET, MOT_INC };

class ModifyOption : public PropName {
private:
	ModifyOpType _op;

public:
	ModifyOption(const std::string &name, const PropValueType type, const ModifyOpType mop = MOT_SET)
		: PropName(name, type), _op(mop) { }
	virtual ~ModifyOption() { }

public:
	inline ModifyOpType op() const { return _op; }
};

}}

#endif /* DB_MODIFY_OPTION_H */
