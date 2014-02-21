#ifndef DB_OPR_PARAM_H
#define DB_OPR_PARAM_H

#include <vector>

#include "../doc/PropItem.h"

namespace msvc { namespace db {

class OprValue {
private:
	std::vector<PropValue> _value;

public:
	virtual ~OprValue() { }

public:
	inline int size() const { return _value.size(); }

	template<typename T>
	inline T getValue(const int i) const { return _value[i].getValue<T>(); }

	template<typename T>
	inline OprValue &operator<<(const T &value) { return add(value); }

	template<typename T>
	inline OprValue &add(const T &value) { _value.push_back(PropValue(value)); return *this; }
};

template<> inline OprValue &OprValue::add(const PropValue &value) { _value.push_back(value); return *this; }

// param sequence: data > modify > query
class OprParam {
private:
	std::vector<OprValue> _param;

public:
	virtual ~OprParam() { }

public:
	inline int size() const { return _param.size(); }

	template<typename T>
	inline T getValue(const int i) const { return _param[i].getValue<T>(0); }

	inline const OprValue &at(const int i) const { return _param[i]; }
	inline OprValue &at(const int i) { return _param[i]; }

	template<typename T>
	inline OprParam &operator<<(const T &value) { return add(value); }

	template<typename T>
	inline OprParam &add(const T &value) { _param.push_back(OprValue() << value); return *this; }
};

template<> inline OprParam &OprParam::add(const OprValue &value) { _param.push_back(value); return *this; }

}}

#endif /* DB_OPR_PARAM_H */
