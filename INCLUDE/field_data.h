#ifndef __FIELD_DATA_H
#define __FIELD_DATA_H

#include "filter_data.h"
namespace wh{

enum class FieldEditor
{
	Normal = 0,
	Type = 1
};
//-----------------------------------------------------------------------------
class Field
{
public:	
	class Filter
	{
	public:
		Filter()
			:mOp(foEq), mConn(fcAND), mEnable(false)
		{}

		Filter(const wxString& val, FilterOp fo = foEq
			, FilterConn fc = fcAND, bool enable = true)
			:mVal(val), mOp(fo), mConn(fc), mEnable(enable)
		{}

		void Set(const wxString& val, FilterOp fo = foEq
			, FilterConn fc = fcAND, bool enable = true)
		{
			mVal=val;
			mOp=fo;
			mConn=fc;
			mEnable = enable;
		}

		wxString	mVal;
		FilterOp	mOp;
		FilterConn	mConn;
		bool		mEnable;

		bool operator==(const Filter& f)const
		{
			return mVal == f.mVal
				&& mOp == f.mOp
				&& mEnable == f.mEnable
				&& mConn == f.mConn;
		}

		bool operator!=(const Filter& f)const
		{
			return !operator==(f);
		}
	};

	wxString	mTitle;
	wxString	mDbTitle;
	FieldType	mType = ftText;
	FieldEditor mEditor = FieldEditor::Normal;
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

	bool operator==(const Field& fld)const
	{
		if (mFilter.size() != fld.mFilter.size())
			return false;

		bool eqFilter = true;
		for (size_t i = 0; i < mFilter.size(); ++i)
		{
			if (mFilter[i] != fld.mFilter[i])
			{ 
				eqFilter = false;
				break;
			}
		}
		return eqFilter
			&& mTitle == fld.mTitle
			&& mDbTitle == fld.mDbTitle
			&& mType == fld.mType
			&& mEditor == fld.mEditor
			&& mGuiShow == fld.mGuiShow
			&& mGuiEdit == fld.mGuiEdit
			&& mInsert == fld.mInsert
			&& mUpdate == fld.mUpdate
			&& mKey == fld.mKey
			&& mSort == fld.mSort;
	}

	bool operator!=(const Field& fld)const
	{
		return !operator==(fld);
	}

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