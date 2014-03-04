#ifndef DB_OPR_SPEC_H
#define DB_OPR_SPEC_H

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <boost/atomic.hpp>

#include "../doc/PropItem.h"
#include "opt/QueryOption.h"
#include "opt/RangeOption.h"
#include "opt/SortOption.h"
#include "opt/ModifyOption.h"

namespace msvc { namespace db {

class OprSpec {
private:
	static boost::atomic_uintmax_t VERSION;

private:
	uintmax_t _version;

protected:
	OprSpec() { update(); }
public:
	OprSpec(const OprSpec &spec) { update(); }
	OprSpec &operator=(const OprSpec &spec) { update(); return *this; }
	virtual ~OprSpec() { }

public:
	inline uintmax_t version() const { return _version; };

protected:
	inline void update() { _version = VERSION.fetch_add(1, boost::memory_order_relaxed); };
};

class DataSpec : public OprSpec {
public:
	std::vector<PropName> data;

public:
	virtual ~DataSpec() { }

public:
	inline DataSpec &addData(const PropName &name) { data.push_back(name); update(); return *this; }

	inline DataSpec &operator<<(const PropName &name) { return addData(name); }
};

class CriteriaSpec : public OprSpec {
public:
	QueryOption query;

protected:
	CriteriaSpec() { }
public:
	virtual ~CriteriaSpec() { }

public:
	inline CriteriaSpec &oprLogic(const LogicalOpType lop) { query.op(lop); update(); return *this; }
	inline CriteriaSpec &addQuery(const QueryOption &opt) { query.add(opt); update(); return *this; }
	inline CriteriaSpec &addQuery(const CriteriaOption &opt) { query.add(opt); update(); return *this; }

	inline CriteriaSpec &operator<<(const LogicalOpType lop) { return oprLogic(lop); }
	inline CriteriaSpec &operator<<(const QueryOption &opt) { return addQuery(opt); }
	inline CriteriaSpec &operator<<(const CriteriaOption &opt) { return addQuery(opt); }
};

class QuerySpec : public CriteriaSpec {
public:
	std::vector<PropName> data;
	std::vector<SortOption> sort;

public:
	virtual ~QuerySpec() { }

public:
	inline QuerySpec &addData(const PropName &name) { data.push_back(name); update(); return *this; }
	inline QuerySpec &addSort(const SortOption &option) { sort.push_back(option); update(); return *this; }

	inline QuerySpec &operator<<(const LogicalOpType lop) { oprLogic(lop); return *this; }
	inline QuerySpec &operator<<(const QueryOption &opt) { addQuery(opt); return *this; }
	inline QuerySpec &operator<<(const CriteriaOption &opt) { addQuery(opt); return *this; }

	inline QuerySpec &operator<<(const PropName &name) { return addData(name); }
	inline QuerySpec &operator<<(const SortOption &option) { return addSort(option); }
};

class ModifySpec : public CriteriaSpec {
public:
	std::vector<ModifyOption> modify;

public:
	virtual ~ModifySpec() { }

public:
	inline ModifySpec &addModify(const ModifyOption &option) { modify.push_back(option); update(); return *this; }

	inline ModifySpec &operator<<(const LogicalOpType lop) { oprLogic(lop); return *this; }
	inline ModifySpec &operator<<(const QueryOption &opt) { addQuery(opt); return *this; }
	inline ModifySpec &operator<<(const CriteriaOption &opt) { addQuery(opt); return *this; }

	inline ModifySpec &operator<<(const ModifyOption &option) { return addModify(option); }
};

class RemoveSpec : public CriteriaSpec {
public:
	virtual ~RemoveSpec() { }

public:
	inline RemoveSpec &operator<<(const LogicalOpType lop) { oprLogic(lop); return *this; }
	inline RemoveSpec &operator<<(const QueryOption &opt) { addQuery(opt); return *this; }
	inline RemoveSpec &operator<<(const CriteriaOption &opt) { addQuery(opt); return *this; }
};

}}

#endif /* DB_OPR_SPEC_H */
