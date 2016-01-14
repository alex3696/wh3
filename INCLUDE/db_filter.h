#ifndef __DB_FILTER_H
#define __DB_FILTER_H

#include "db_rec.h"
#include "TModel.h"

namespace wh{
//-------------------------------------------------------------------------
enum FilterConn
{
	fcAND,
	fcOR
};

static wxString ToSqlString(FilterConn fc)
{
	switch (fc)
	{
	case wh::fcAND: return " AND ";
	case wh::fcOR:	return " OR ";
	default:break;
	}
	return wxEmptyString;
};

enum FilterOp
{
	foEq,
	foNotEq,
	foLess,
	foMore,
	foLike,
	foNotLike,
	foBetween,
	foNotBetween

};

static wxString ToSqlString(FilterOp fo)
{
	switch (fo)
	{
	case wh::foEq:		return " = ";
	case wh::foNotEq:	return " <> ";
	case wh::foLess:	return " < ";
	case wh::foMore:	return " > ";
	case wh::foLike:	return " LIKE ";
	case wh::foNotLike: return " NOT LIKE ";
	case wh::foBetween: return " BETWEEN ";
	case wh::foNotBetween:return " NOT BETWEEN ";
	default:break;
	}
	return wxEmptyString;
}


//-------------------------------------------------------------------------
struct FilterData
{
	wxString	mVal;
	wxString	mFieldName;
	FieldType	mFieldType;
	FilterOp	mOp = foEq;
	FilterConn	mConn = fcAND;
	bool		mIsEnabled=false;

	FilterData(){}

	FilterData(const wxString& val, const wxString& fname
		, FieldType ft = ftText, FilterOp fo = foEq, FilterConn fc = fcAND, bool enable = true)
		:mVal(val), mFieldName(fname), mFieldType(ft), mOp(fo), mConn(fc), mIsEnabled(enable)
	{}

	wxString GetSqlString()const 
	{ 
		wxString str;
		if (mIsEnabled)
			str << " " << ToSqlString(mConn) << " " << mFieldName <<
				ToSqlString(mOp) << "'" << mVal << "' ";
		return str; 
	};
};
//-------------------------------------------------------------------------
class MFilter
	: public TModelData<FilterData>
{
public:
	MFilter(const char option = ModelOption::EnableParentNotify)
		:TModelData<FilterData>(option)
	{}

	MFilter(const FilterData& fd,const char option = ModelOption::EnableParentNotify)
		:TModelData<FilterData>(option)
	{
		SetData(fd, true);
	}

};
//-------------------------------------------------------------------------
class MFilterArray
	: public TModelArray<MFilter>
{
public:
	MFilterArray(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild
		| ModelOption::CommitSave)
		: TModelArray<MFilter>(option)
	{}

	
	//std::shared_ptr<MFilter> AddFilter(const FilterData& fd);
	wxString GetSqlString()const
	{
		wxString str;
		for (size_t i = 0; i < GetChildQty(); i++)
		{
		//#ifdef __DEBUG const auto& data = at(i)->GetData();	#endif // DEBUG
			str += at(i)->GetData().GetSqlString();
		}
			
		return str;
	}
};

//-------------------------------------------------------------------------

}//namespace wh
#endif // __*_H


