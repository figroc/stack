#ifndef DB_PROP_ITEM_H
#define DB_PROP_ITEM_H

#include <cstring>
#include <string>
#include <time.h>
#include <stdexcept>
#include <boost/uuid/uuid.hpp>

namespace msvc { namespace db {

enum PropValueType { PVT_STRING, PVT_INTEGER, PVT_TIME, PVT_UUID };

class PropName {
private:
	std::string _name;
	PropValueType _type;

public:
	PropName(const std::string &name, const PropValueType type) : _name(name), _type(type) {
		if (_name.find('$') != std::string::npos)
			throw std::invalid_argument("prop name cannot contain '$'");
	}
	virtual ~PropName() { }

public:
	inline const std::string &name() const { return _name; }
	inline PropValueType type() const { return _type; }
};

class PropValue {
private:
	std::string _str;
	union {
		int _int;
		time_t _time;
		boost::uuids::uuid _uid;
	} _val;

public:
	PropValue() : _str(), _val() { }
	template<typename T> explicit PropValue(const T &value) : _str(value), _val() { }
	virtual ~PropValue() { }

public:
	template<typename T> inline T getValue() const { return _str; }
	template<typename T> inline void setValue(const T &value) { _str = value; }
};

template<> inline PropValue::PropValue(const int &value) : _str(), _val() { _val._int = value; }
template<> inline PropValue::PropValue(const time_t &value) : _str(), _val() { _val._time = value; }
template<> inline PropValue::PropValue(const boost::uuids::uuid &value) : _str(), _val() { _val._uid = value; }

template<> inline int PropValue::getValue() const { return _val._int; }
template<> inline time_t PropValue::getValue() const { return _val._time; }
template<> inline boost::uuids::uuid PropValue::getValue() const { return _val._uid; }

template<> inline void PropValue::setValue(const int &value) { _val._int = value; }
template<> inline void PropValue::setValue(const time_t &value) { _val._time = value; }
template<> inline void PropValue::setValue(const boost::uuids::uuid &uid) { _val._uid = uid; }

class PropItem : public PropName, public PropValue {
public:
	PropItem(const PropName &name, const PropValue &value)
		: PropName(name), PropValue(value) { }
	template<typename T> PropItem(const std::string &pname, const T &value)
		: PropName(pname, PVT_STRING), PropValue(value) { }
	virtual ~PropItem() { }
};

template<> inline PropItem::PropItem(const std::string &pname, const int &value)
	: PropName(pname, PVT_INTEGER), PropValue(value) { }
template<> inline PropItem::PropItem(const std::string &pname, const time_t &value)
	: PropName(pname, PVT_TIME), PropValue(value) { }
template<> inline PropItem::PropItem(const std::string &pname, const boost::uuids::uuid &value)
	: PropName(pname, PVT_UUID), PropValue(value) { }

template<> inline PropValue::PropValue(const PropItem &value) : _str(value._str), _val() { _val._uid = value._val._uid; }

}}

#endif /* DB_PROP_ITEM_H */
