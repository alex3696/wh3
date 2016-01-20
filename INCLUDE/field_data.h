#ifndef __FIELD_DATA_H
#define __FIELD_DATA_H

#include "filter_data.h"
namespace wh{


//-----------------------------------------------------------------------------
class Field
{
public:	
	class Filter
	{
	public:
		Filter(const wxString& val, FilterOp fo = foEq
			, FilterConn fc = fcAND, bool enable = true)
			:mVal(val), mOp(fo), mConn(fc), mEnable(enable)
		{}

		wxString	mVal;
		FilterOp	mOp;
		FilterConn	mConn;
		bool		mEnable;
	};

	wxString	mTitle;
	wxString	mDbTitle;
	FieldType	mType = ftText;
	bool		mGuiShow = true;
	bool		mGuiEdit = true;
	
	//bool		mSelect = true;
	bool		mInsert = true;
	bool		mUpdate = true;
	bool		mKey = false;

	int			mSort = 0;
	std::vector<Filter> mFilter;

	Field(){}
	Field(const char* name, FieldType ft, bool show)
		:mTitle(name), mType(ft), mGuiShow(show)
	{}


	Field(const wxString& name, FieldType ft, bool show, const wxString& dbname=wxEmptyString)
		:mTitle(name), mType(ft), mGuiShow(show), mDbTitle(dbname)
	{}

	wxString GetSqlFilter(unsigned int i)const
	{
		wxString str;
		if (mFilter.size() > i)
			return str;
		const auto& filter = mFilter[i];
		
		if (filter.mEnable)
			str << ToSqlString(filter.mConn) << " " << mDbTitle <<
			ToSqlString(filter.mOp) << "'" << filter.mVal << "' ";
		return str;
	};

};
//-----------------------------------------------------------------------------
const static std::vector<Field> gEmptyFieldVec;

class FieldsInfo
{
public:
	virtual ~FieldsInfo()
	{
	}

	virtual const std::vector<Field>& GetFieldVector()const
	{
		return gEmptyFieldVec;
	}
	unsigned int GetFieldQty()const
	{
		return GetFieldVector().size();
	}
	const wxString& GetFieldName(unsigned int col)const
	{
		return GetFieldVector()[col].mTitle;
	}
	const FieldType& GetFieldType(unsigned int col)const
	{
		return GetFieldVector()[col].mType;
	}
	virtual bool GetFieldValue(unsigned int col, wxVariant &variant)
	{
		return false;
	}

};


















}//namespace wh{
#endif // __****_H