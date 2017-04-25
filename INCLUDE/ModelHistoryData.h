#ifndef __MODEL_DATAHISTORY_H
#define __MODEL_DATAHISTORY_H

#include "globaldata.h"
#include "db_rec.h"

namespace wh{

//-----------------------------------------------------------------------------
enum class FilterKind
{
	  Value			= 0 // = < > ! <= >=
	  , Bool		= 1 //
	  , ArrOneValue = 2 //

	  , ArrAnyValue	= 10 //
	
	
	, Interval		= 20 
	
};

class IFilter
{
	wxString	mTitle;
	wxString	mSysTitle;
public:
	const wxString& GetSysTitle()const	{ return mSysTitle; }
	const wxString& GetTitle()const		{ return mTitle; }

	virtual wxString AsString()const = 0;

};


class FilterValue : public IFilter
{
	wxString	mValue;
public:
	virtual FilterKind	GetKind()const		{ return FilterKind::Value;  }
	virtual wxString	AsString()const override
	{ 
		return wxString::Format(" %s=%s"
			, GetSysTitle(), mValue);
	}

	const wxString& GetValue()const	{ return mValue; }
};

class FilterValueInterval : public FilterValue
{
	wxString	mEndValue;
public:
	virtual FilterKind	GetKind()const		{ return FilterKind::Interval; }
	virtual wxString	AsString()const override
	{
		return wxString::Format(" %s BETWEEN %s AND %s"
			, GetSysTitle(), GetBeginValue(), mEndValue);
	}
	const wxString& GetBeginValue()const{ return GetValue(); }
	const wxString& GetEndValue()const	{ return mEndValue; }
};

class FilterArrAnyValue : public IFilter
{
protected:
	std::vector<wxString>	mArr;
public:
	virtual FilterKind	GetKind()const		{ return FilterKind::ArrAnyValue; }
	virtual wxString	AsString()const override
	{
		wxString ret;
		for (const auto& item : mArr)
		{
			ret += wxString::Format(" OR %s=%s"
				, GetSysTitle(), item);
		}
		ret.Replace("OR", "", false);
		return ret;
	}
};

class FilterArrOneValue : public FilterArrAnyValue
{
	size_t					mSelected;
public:
	virtual FilterKind	GetKind()const		{ return FilterKind::ArrOneValue; }
	virtual wxString	AsString()const override
	{
		wxString ret;
		ret += wxString::Format(" %s=%s"
			, GetSysTitle(), mArr[mSelected]);
		return ret;
	}
};


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
	virtual const wxString& GetQty()const = 0;
	
	virtual const IObjPath&		GetPath()const = 0;
	virtual const PropValTable& GetPropValTable()const = 0;
	
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

	virtual const IObjPath& GetDstPath(const size_t row)const = 0;
};
	

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
//-----------------------------------------------------------------------------
} //namespace wh{
#endif // __IMVP_H