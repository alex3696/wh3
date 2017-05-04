#ifndef __FILTERDATA_H
#define __FILTERDATA_H

#include "_pch.h"

namespace wh{
//-------------------------------------------------------------------------
enum FilterConn
{
	fcAND,
	fcOR
};
//-------------------------------------------------------------------------
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
//-------------------------------------------------------------------------
enum FilterOp
{
	foEq,
	foNotEq,
	foLess,
	foMore,
	foLessEq,
	foMoreEq,
	foLike,
	foNotLike,
	foBetween,
	foNotBetween
};
//-------------------------------------------------------------------------
static wxString ToSqlString(FilterOp fo)
{
	switch (fo)
	{
	case wh::foEq:		return " = ";
	case wh::foNotEq:	return " <> ";
	case wh::foLess:	return " < ";
	case wh::foMore:	return " > ";
	case wh::foLessEq:	return " <= ";
	case wh::foMoreEq:	return " >= ";
	case wh::foLike:	return " ~~* ";
	case wh::foNotLike: return " !~~* ";
	case wh::foBetween: return " <= AND >= ";
	case wh::foNotBetween:return " < AND > ";
	default:break;
	}
	return wxEmptyString;
}
//-------------------------------------------------------------------------
const static std::vector<FilterOp> 
	gAllFilterOpVector 	= { foEq,
							foNotEq,
							foLess,
							foMore,
							foLessEq,
							foMoreEq,
							foLike,
							foNotLike,
							foBetween,
							foNotBetween };
//-------------------------------------------------------------------------
class AllFilterOpStringArray
{
	wxArrayString mStringArray;

	AllFilterOpStringArray()
	{
		for (const auto item : wh::gAllFilterOpVector)
			mStringArray.Add(ToSqlString(item));
	}
public:
	static AllFilterOpStringArray* GetInstance()
	{
		static AllFilterOpStringArray instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return &instance;
	}
	const wxArrayString& GetStringArray()
	{
		return mStringArray;
	}
};
//-------------------------------------------------------------------------

}//namespace wh
#endif // __*_H


