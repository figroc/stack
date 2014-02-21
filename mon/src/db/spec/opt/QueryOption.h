#ifndef DB_QUERY_OPTION_H
#define DB_QUERY_OPTION_H

#include <vector>
#include <boost/smart_ptr.hpp>

#include "../../doc/PropItem.h"

namespace msvc { namespace db {

enum CompareOpType { COT_EQ, COT_LT, COT_GT, COT_LTE, COT_GTE, COT_IN };
enum LogicalOpType { LOT_AND, LOT_OR };

class ICriteria {
private:
	bool _compose;

protected:
	ICriteria(const bool comp) : _compose(comp) { }

public:
	virtual ~ICriteria() { }

public:
	inline bool compose() const { return _compose; }
};

class CriteriaOption : public ICriteria, public PropName {
private:
	CompareOpType _op;

public:
	CriteriaOption(const std::string &name, const PropValueType type, const CompareOpType cop = COT_EQ)
		: ICriteria(false), PropName(name, type), _op(cop) { }
	virtual ~CriteriaOption() { }

public:
	inline CompareOpType op() const { return _op; }
};

class QueryOption : public ICriteria {
private:
	LogicalOpType _op;
	std::vector< boost::shared_ptr<ICriteria> > _opts;

public:
	QueryOption(const LogicalOpType lop = LOT_AND) : ICriteria(true), _op(lop) { }
	virtual ~QueryOption() { }

public:
	inline int size() const { return _opts.size(); }
	inline LogicalOpType op() const { return _op; }

	inline const ICriteria &at(const int i) const { return *_opts[i]; }
	inline const ICriteria &operator[](const int i) const { return *_opts[i]; }

	inline QueryOption &op(const LogicalOpType lop) { _op = lop; return *this; }
	inline QueryOption &add(const QueryOption &opt) {
		_opts.push_back(boost::shared_ptr<ICriteria>(new QueryOption(opt)));
		return *this;
	}
	inline QueryOption &add(const CriteriaOption &opt) {
		_opts.push_back(boost::shared_ptr<ICriteria>(new CriteriaOption(opt)));
		return *this;
	}

	inline QueryOption &operator<<(const LogicalOpType lop) { return op(lop); }
	inline QueryOption &operator<<(const QueryOption &opt) { return add(opt); }
	inline QueryOption &operator<<(const CriteriaOption &opt) { return add(opt); }
};

}}

#endif /* DB_QUERY_OPTION_H */
