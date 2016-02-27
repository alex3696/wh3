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
	case wh::foLike:	return " ~~* ";
	case wh::foNotLike: return " !~~* ";
	case wh::foBetween: return " BETWEEN ";
	case wh::foNotBetween:return " NOT BETWEEN ";
	default:break;
	}
	return wxEmptyString;
}

//-------------------------------------------------------------------------

}//namespace wh
#endif // __*_H


