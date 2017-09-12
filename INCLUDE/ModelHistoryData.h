#ifndef __MODEL_DATAHISTORY_H
#define __MODEL_DATAHISTORY_H

#include "globaldata.h"
#include "db_rec.h"

namespace wh{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class IDbIdent
{
public:
	virtual const wxString& GetId()const = 0;
	virtual const wxString& GetTitle()const = 0;
};

struct extr_id_IDbIdent
{
	typedef const wxString& result_type;
	inline result_type operator()(const std::shared_ptr<IDbIdent>& r)const
	{
		return r->GetId();
	}
};

struct extr_title_IDbIdent
{
	typedef const wxString& result_type;
	inline result_type operator()(const std::shared_ptr<IDbIdent>& r)const
	{
		return r->GetTitle();
	}
};

using DbIdentTable =
boost::multi_index_container
<
	std::shared_ptr<IDbIdent>,
	indexed_by
	<
		random_access<> //SQL order
		, ordered_unique< extr_id_IDbIdent >
		, ordered_unique< extr_title_IDbIdent >
	>
>;
//-----------------------------------------------------------------------------
class ICls : public IDbIdent
{
public:
	virtual const wxString& GetKind()const = 0;
	virtual const wxString& GetMeasure()const = 0;
};

using ClsTable =
boost::multi_index_container
<
	std::shared_ptr<ICls>,
	indexed_by
	<
		random_access<> //SQL order
		, ordered_unique< extr_id_IDbIdent >
	>
>;
//-----------------------------------------------------------------------------
class IProp : public IDbIdent
{
public:
	virtual const wxString& GetKind()const = 0;

	FieldType GetType()const
	{
		return ToFieldType(GetKind());
	}
};

using PropTable =
boost::multi_index_container
<
	std::shared_ptr<IProp>,
	indexed_by
	<
		random_access<> //SQL order
		, ordered_unique< extr_id_IDbIdent >
		//, ordered_unique< extr_title_IProp >
		
	>
>;
//-----------------------------------------------------------------------------
class IPropVal
{
public:
	virtual const IProp&	GetProp()const = 0;
	virtual const wxString& GetValue()const = 0;
};
//-----------------------------------------------------------------------------
struct extr_pid_PropValRec
{
	typedef const wxString& result_type;
	inline result_type operator()(const std::shared_ptr<IPropVal>& r)const
	{
		return r->GetProp().GetId();
	}
};
struct extr_ptitle_PropValRec
{
	typedef const wxString& result_type;
	inline result_type operator()(const std::shared_ptr<IPropVal>& r)const
	{
		return r->GetProp().GetTitle();
	}
};

using PropValTable =
boost::multi_index_container
<
	std::shared_ptr<IPropVal>,
	indexed_by
	<
		  ordered_unique< extr_pid_PropValRec >
		//, random_access<> //SQL order
		, ordered_non_unique< extr_ptitle_PropValRec >
		
	>
>;
//-----------------------------------------------------------------------------
class IObjPath
{
public:
	virtual wxString AsString()const = 0;
};
//-----------------------------------------------------------------------------
class IObj : public IDbIdent
{
public:
	virtual const ICls&		GetCls()const = 0;
	
	virtual const PropValTable& GetProperties()const = 0;
	virtual const IObjPath&	GetPath()const = 0;
	
	
};
struct extr_oid_IObj
{
	typedef const wxString& result_type;
	inline result_type operator()(const std::shared_ptr<IObj>& r)const
	{
		return r->GetId();
	}
};

struct extr_cid_IObj
{
	typedef const wxString& result_type;
	inline result_type operator()(const std::shared_ptr<IObj>& r)const
	{
		return r->GetCls().GetId();
	}
};
//struct extr_oid_cid_IObj
//{
//	typedef std::pair<const wxString&, const wxString&> result_type;
//	inline result_type operator()(const std::shared_ptr<IObj>& r)const
//	{
//		return std::make_pair(r->GetId(), r->GetCls().GetId());
//	}
//};

using ObjTable =
boost::multi_index_container
<
	std::shared_ptr<IObj>,
	indexed_by
	<
		random_access<> //SQL order
		, ordered_unique< extr_id_IDbIdent >
		,ordered_non_unique< extr_cid_IObj >
		//, ordered_unique< extr_oid_cid_IObj >
	>
>;
//-----------------------------------------------------------------------------
class IAct : public IDbIdent
{
public:
	virtual const wxString& GetColour()const = 0;
	virtual const PropTable& GetPropList()const = 0;
};

using ActTable =
boost::multi_index_container
<
	std::shared_ptr<IAct>,
	indexed_by
	<
		random_access<> //SQL order
		, ordered_unique< extr_id_IDbIdent >
		, ordered_unique< extr_title_IDbIdent >
	>		
>;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class ModelHistoryTableData
{
public:
	virtual ~ModelHistoryTableData(){}
	virtual size_t size()const = 0;
	virtual const wxString& GetLogId(const size_t row)const = 0;
	virtual const wxString& GetUser(const size_t row)const = 0;
	virtual const wxString& GetDate(const size_t row)const = 0;

	virtual const IObj& GetObj(const size_t row)const = 0;
	virtual const IAct& GetAct(const size_t row)const = 0;
	virtual const PropValTable& GetActProperties(const size_t row)const = 0;
	virtual const PropValTable& GetProperties(const size_t row)const = 0;
	virtual const IObjPath&	GetPath(const size_t row)const = 0;
	virtual const IObjPath& GetDstPath(const size_t row)const = 0;
	virtual const wxString& GetQty(const size_t row)const = 0;

};
//-----------------------------------------------------------------------------
class ModelHistoryRecord
{
public:
	virtual const wxString& GetLogId()const = 0;
	virtual const wxString& GetUser()const = 0;
	virtual const wxString& GetDate()const = 0;

	virtual const IObj& GetObj()const = 0;
	virtual const IAct& GetAct()const = 0;
	virtual const PropValTable& GetActProperties()const = 0;
	virtual const PropValTable& GetProperties()const = 0;
	virtual const IObjPath&	GetPath()const = 0;
	virtual const IObjPath& GetDstPath()const = 0;
	virtual const wxString& GetQty()const = 0;

};
//-----------------------------------------------------------------------------
} //namespace wh{
#endif // __*_H