#ifndef __FILTERDATA_H
#define __FILTERDATA_H

#include "db_rec.h"

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

}//namespace wh
#endif // __*_H


