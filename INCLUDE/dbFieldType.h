#ifndef __DBFIELDTYPE_H
#define __DBFIELDTYPE_H

#include "_pch.h"

namespace wh{


enum FieldType
{
	ftText = 0,
	ftName = 1,
	ftTextArray = 2,

	ftLong = 100,
	ftDouble = 101,

	ftDateTime = 200,
	ftDate = 201,
	ftTime = 202,

	ftLink = 300,

	ftFile = 400,

	ftJSON = 500,

	ftBool = 600
	
};
struct FieldTypeItem
{
	FieldTypeItem(FieldType dt, const wxString& str)
		:mType(dt), mTitle(str)
	{}

	FieldType	mType;
	wxString	mTitle;
};
//-----------------------------------------------------------------------------
using FieldTypeArray =
	boost::multi_index_container
	<
	FieldTypeItem,
		indexed_by
		<
			random_access<>
			, ordered_unique< BOOST_MULTI_INDEX_MEMBER(FieldTypeItem, FieldType, mType)>
			, ordered_unique< BOOST_MULTI_INDEX_MEMBER(FieldTypeItem, wxString, mTitle)>
		>
	>;

static FieldTypeArray gFieldTypeArray = {
								{ ftText, "Текст" },
								{ ftName, "Имя" },
								{ ftTextArray, "Текст[]" },
								{ ftLong, "Число(целое)" },
								{ ftDouble, "Число(дробное)" },
								{ ftDateTime, "Дата Время" },
								{ ftDate, "Дата" },
								{ ftTime, "Время" },
								{ ftLink, "Ссылка" },
								{ ftFile, "Файл" },
								{ ftJSON, "JSON" },
								{ ftBool, "Логический" },
								};



//-----------------------------------------------------------------------------
static wxString ToText(FieldType type)
{
	const auto& dbTypeIdx = gFieldTypeArray.get<1>();
	auto it = dbTypeIdx.find(type);
	if (dbTypeIdx.end() == it)
	{
		struct dbtype_error : virtual exception_base {};
		BOOST_THROW_EXCEPTION(dbtype_error() << wxstr("Unknown DbType"));
	}
	return it->mTitle;
}
//-----------------------------------------------------------------------------
static FieldType ToFieldType(const wxString& str)
{
	unsigned long val;
	if (str.ToCULong(&val))
		return (FieldType)val;
	const auto& dbTitleIdx = gFieldTypeArray.get<2>();
	auto it = dbTitleIdx.find(str);
	if (dbTitleIdx.end() == it)
	{
		struct dbtype_error : virtual exception_base {};
		BOOST_THROW_EXCEPTION(dbtype_error() << wxstr("Unknown Type String"));
	}
	return it->mType;
}
//-----------------------------------------------------------------------------

static int GetColumnWidthBy(FieldType ft)
{
	switch (ft)
	{
	case wh::ftText:	return	-1;		break;
	case wh::ftName:	return	150;	break;
	case wh::ftLong:	return	100;	break;
	case wh::ftDouble:	return	100;	break;
	case wh::ftDateTime:return	100;	break;
	case wh::ftDate:	return	80;		break;
	case wh::ftTime:	return	80;		break;
	case wh::ftLink:	return	80;		break;
	case wh::ftFile:	return	80;		break;
	case wh::ftJSON:	return	150;	break;
	default:			return	80;		break;
	}
	return -1;
}
//-----------------------------------------------------------------------------

}//namespace wh{
#endif // __****_H