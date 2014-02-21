#ifndef DB_OPR_SPEC_H
#define DB_OPR_SPEC_H

#include <vector>

#include "../doc/PropItem.h"
#include "opt/QueryOption.h"
#include "opt/RangeOption.h"
#include "opt/SortOption.h"
#include "opt/ModifyOption.h"

namespace msvc { namespace db {

struct OprSpec {
protected:
	OprSpec() { }
public:
	virtual ~OprSpec() { }
};

struct DataSpec : public OprSpec {
public:
	std::vector<PropName> data;

public:
	virtual ~DataSpec() { }

public:
	inline DataSpec &addData(const PropName &name) { data.push_back(name); return *this; }

	inline DataSpec &operator<<(const PropName &name) { return addData(name); }
};

struct CriteriaSpec : public OprSpec {
public:
	QueryOption query;

protected:
	CriteriaSpec() { }
public:
	virtual ~CriteriaSpec() { }

public:
	inline CriteriaSpec &oprLogic(const LogicalOpType lop) { query.op(lop); return *this; }
	inline CriteriaSpec &addQuery(const QueryOption &opt) { query.add(opt); return *this; }
	inline CriteriaSpec &addQuery(const CriteriaOption &opt) { query.add(opt); return *this; }

	inline CriteriaSpec &operator<<(const LogicalOpType lop) { return oprLogic(lop); }
	inline CriteriaSpec &operator<<(const QueryOption &opt) { return addQuery(opt); }
	inline CriteriaSpec &operator<<(const CriteriaOption &opt) { return addQuery(opt); }
};

struct QuerySpec : public CriteriaSpec {
public:
	std::vector<PropName> data;
	std::vector<SortOption> sort;

public:
	virtual ~QuerySpec() { }

public:
	inline QuerySpec &addData(const PropName &name) { data.push_back(name); return *this; }
	inline QuerySpec &addSort(const SortOption &option) { sort.push_back(option); return *this; }

	inline QuerySpec &operator<<(const LogicalOpType lop) { oprLogic(lop); return *this; }
	inline QuerySpec &operator<<(const QueryOption &opt) { addQuery(opt); return *this; }
	inline QuerySpec &operator<<(const CriteriaOption &opt) { addQuery(opt); return *this; }

	inline QuerySpec &operator<<(const PropName &name) { return addData(name); }
	inline QuerySpec &operator<<(const SortOption &option) { return addSort(option); }
};

struct ModifySpec : public CriteriaSpec {
public:
	std::vector<ModifyOption> modify;

public:
	virtual ~ModifySpec() { }

public:
	inline ModifySpec &addModify(const ModifyOption &option) { modify.push_back(option); return *this; }

	inline ModifySpec &operator<<(const LogicalOpType lop) { oprLogic(lop); return *this; }
	inline ModifySpec &operator<<(const QueryOption &opt) { addQuery(opt); return *this; }
	inline ModifySpec &operator<<(const CriteriaOption &opt) { addQuery(opt); return *this; }

	inline ModifySpec &operator<<(const ModifyOption &option) { return addModify(option); }
};

struct RemoveSpec : public CriteriaSpec {
public:
	virtual ~RemoveSpec() { }

public:
	inline RemoveSpec &operator<<(const LogicalOpType lop) { oprLogic(lop); return *this; }
	inline RemoveSpec &operator<<(const QueryOption &opt) { addQuery(opt); return *this; }
	inline RemoveSpec &operator<<(const CriteriaOption &opt) { addQuery(opt); return *this; }
};

}}

#endif /* DB_OPR_SPEC_H */
