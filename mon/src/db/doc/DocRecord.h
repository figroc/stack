#ifndef DB_DOC_RECORD_H
#define DB_DOC_RECORD_H

#include <string>
#include <vector>
#include <map>

#include <boost/smart_ptr.hpp>

#include "PropItem.h"

namespace msvc { namespace db {

class DocHeader {
private:
	std::map<std::string, int> _index;
	std::vector<PropName> _names;

public:
	virtual ~DocHeader() { }

public:
	inline int size() const { return _names.size(); }

	inline int index(const std::string &name) const {
		std::map<std::string, int>::const_iterator it = _index.find(name);
		return _index.end() == it ? -1 : it->second;
	}

	inline const PropName &operator[](const int i) const { return _names[i]; }
	inline const PropName &operator[](const std::string &name) const { return _names[index(name)]; }

	inline const PropName &at(const int i) const { return _names.at(i); }
	inline const PropName &at(const std::string &name) const { return _names.at(index(name)); }

	inline DocHeader &add(const PropName &prop) {
		_index[prop.name()] = _names.size();
		_names.push_back(prop);
		return *this;
	}
};

class DocRawRow {
private:
	boost::shared_ptr<DocHeader> _header;
	std::vector<PropValue> _values;

public:
	DocRawRow(const boost::shared_ptr<DocHeader> &header) : _header(header), _values() { }
	virtual ~DocRawRow () { }

public:
	inline int size() const { return _values.size(); }
	inline int index(const std::string &name) const { return _header->index(name); }

	inline const PropValue &operator[](const int i) const { return _values[i]; }
	inline const PropValue &operator[](const std::string &name) const { return _values[index(name)]; }

	inline const PropValue &at(const int i) const { return _values.at(i); }
	inline const PropValue &at(const std::string &name) const { return _values.at(index(name)); }

	template<typename T>
	inline DocRawRow &add(const T &value) { _values.push_back(PropValue(value)); return *this; }
	template<typename T>
	inline DocRawRow &set(const std::string &name, const T &value) { return set(index(name), value); }
	template<typename T>
	inline DocRawRow &set(const int pos, const T &value) {
		while (_values.size() < pos)
			_values.push_back(PropValue());
		if (_values.size() == pos)
			_values.push_back(PropValue(value));
		else
			_values[pos] = PropValue(value);
		return *this;
	}
};

class DocRecord {
private:
	boost::shared_ptr<DocHeader> _header;
	DocRawRow _data;

public:
	DocRecord() : _header(new DocHeader()), _data(_header) { }
	virtual ~DocRecord() { }

public:
	inline int size() const { return _header->size(); }

	inline const PropValue &operator[](const int i) const { return _data[i]; }
	inline const PropValue &operator[](const std::string &name) const { return _data[name]; }

	inline const PropValue &at(const int i) const { return _data.at(i); }
	inline const PropValue &at(const std::string &name) const { return _data.at(name); }

	inline DocRecord &add(const PropItem &prop) {
		_header->add(prop);
		_data.add(prop);
		return *this;
	}
	template<typename T>
	inline DocRecord &add(const std::string &name, const T &value) {
		PropItem prop(name, value);
		_header->add(prop);
		_data.add(prop);
		return *this;
	}
};

class DocTable {
private:
	boost::shared_ptr<DocHeader> _header;
	std::vector< boost::shared_ptr<DocRawRow> > _datas;

public:
	DocTable() : _header(new DocHeader()), _datas() { }
	virtual ~DocTable() { }

public:
	inline int size() const { return _datas.size(); }

	inline const boost::shared_ptr<DocRawRow> &operator[](const int i) const { return _datas[i]; }
	inline const boost::shared_ptr<DocRawRow> &at(const int i) const { return _datas.at(i); }

	inline const boost::shared_ptr<DocHeader> &header() const { return _header; }

	inline DocTable &addName(const PropName &name) { _header->add(name); return *this; }

	inline const boost::shared_ptr<DocRawRow> &addRow() {
		_datas.push_back(boost::make_shared<DocRawRow>(_header));
		return _datas.back();
	}
};

}}

#endif /* DB_DOC_RECORD_H */
