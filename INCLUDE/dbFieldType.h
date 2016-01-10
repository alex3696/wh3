#ifndef __DBFIELDTYPE_H
#define __DBFIELDTYPE_H

#include "_pch.h"

namespace wh{


enum FieldType
{
	ftText = 0,
	ftName = 1,

	ftLong = 10,
	ftDouble = 11,

	ftDate = 20,

	ftLink = 30,

	ftFile = 40,

	ftJSON = 50
	
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
								{ ftLong, "Число(целое)" },
								{ ftDouble, "Число(дробное)" },
								{ ftDate, "Дата" },
								{ ftLink, "Ссылка" },
								{ ftFile, "Файл" },
								{ ftJSON, "JSON" },
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

}//namespace wh{
#endif // __****_H