#ifndef __FILTERMODEL_H
#define __FILTERMODEL_H

#include "filter_data.h"
#include "TModelArray.h"

namespace wh{

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
		| ModelOption::EnableNotifyFromChild)
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


//-----------------------------------------------------------------------------

class MConditionGroup
	: public IModel
{
public:
	MConditionGroup(const char option = ModelOption::EnableNotifyFromChild)
		:IModel(option)
		, mName(new TModelData<wxString>)
		, mDesc(new TModelData<wxString>)
		, mConn(new TModelData<FilterConn>)
		, mReqAll(new TModelData<bool>)
		, mConditionArr(new MFilterArray)
	{
		mConn->SetData(fcAND, true);
		//mDesc->SetData("��������", true);
		//mName->SetData("������ ��������", true);

		Insert(mName);
		Insert(mDesc);
		Insert(mConn);
		Insert(mReqAll);
		Insert(mConditionArr);
	}
protected:
	std::shared_ptr<TModelData<wxString>>	mName;
	std::shared_ptr<TModelData<wxString>>	mDesc;
	std::shared_ptr<TModelData<FilterConn>>	mConn;
	std::shared_ptr<TModelData<bool>>		mReqAll;
	std::shared_ptr<MFilterArray>			mConditionArr;
	

};
//-------------------------------------------------------------------------
class MCondition
	: public IModel
{
public:
	MCondition(const char option =  ModelOption::EnableNotifyFromChild)
		: IModel(option)
	{}

	virtual std::shared_ptr<IModel> CreateChild()override
	{
		auto child = std::make_shared < MConditionGroup >();
		return child;
	}
	//wxString GetSqlString()const { return wxEmptyString; }
	//void BuildByFields(fields);
};

//-------------------------------------------------------------------------
}//namespace wh
#endif // __*_H