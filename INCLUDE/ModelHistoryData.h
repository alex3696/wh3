#ifndef __MODEL_DATAHISTORY_H
#define __MODEL_DATAHISTORY_H

#include "globaldata.h"
#include "db_rec.h"

namespace wh{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ICls
{
public:
	virtual const wxString& GetId()const = 0;
	virtual const wxString& GetTitle()const = 0;
	virtual const wxString& GetKind()const = 0;
	virtual const wxString& GetMeasure()const = 0;
};
//-----------------------------------------------------------------------------
class IProp
{
public:
	virtual const wxString& GetId()const = 0;
	virtual const wxString& GetTitle()const = 0;
	virtual const wxString& GetKind()const = 0;
};
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
		, random_access<> //SQL order
		//, ordered_unique< extr_ptitle_PropValRec >
		
	>
>;
//-----------------------------------------------------------------------------
class IObjPath
{
public:
	virtual wxString AsString()const = 0;
};
//-----------------------------------------------------------------------------
class IObj
{
public:
	virtual const ICls&		GetCls()const = 0;

	virtual const wxString& GetId()const = 0;
	virtual const wxString& GetTitle()const = 0;
	
	
	
	
	
};
//-----------------------------------------------------------------------------
class IAct
{
public:
	virtual const wxString& GetId()const = 0;
	virtual const wxString& GetTitle()const = 0;
	virtual const wxString& GetColour()const = 0;
};
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

} //namespace wh{
#endif // __*_H