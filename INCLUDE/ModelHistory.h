#ifndef __MODEL_HISTORY_H
#define __MODEL_HISTORY_H

#include "ModelHistoryData.h"
#include "IModelWindow.h"
#include "ModelFilterList.h"
#include "ModelObjPropList.h"

namespace wh{
//-----------------------------------------------------------------------------
class ActRec: public IAct
{
public:
	wxString	mId;
	wxString	mTitle;
	wxString	mColour;

	virtual const wxString& GetId()const override		{ return mId; };
	virtual const wxString& GetTitle()const override	{ return mTitle; };
	virtual const wxString& GetColour()const override	{ return mColour; };

};

using ActTable =
boost::multi_index_container
<
	std::shared_ptr<ActRec>,
	indexed_by
	<
		random_access<> //SQL order
		, ordered_unique< BOOST_MULTI_INDEX_MEMBER(ActRec, wxString, mId)>
		, ordered_unique< BOOST_MULTI_INDEX_MEMBER(ActRec, wxString, mTitle)>
	>
>;
//-----------------------------------------------------------------------------

class PropRec: public IProp
{
public:
	wxString	mId;
	wxString	mTitle;
	wxString	mKind;
	//wxString	mVarArray;
	//wxString	mVarStrict;
	virtual const wxString& GetId()const override		{ return mId; };
	virtual const wxString& GetTitle()const override	{ return mTitle; };
	virtual const wxString& GetKind()const override		{ return mKind; };

};
//-----------------------------------------------------------------------------

class PropValRec: public IPropVal
{
public:
	std::shared_ptr<IProp>	mProp;
	wxString				mVal;
	virtual const IProp&	GetProp()const override		{ return *mProp; };
	virtual const wxString& GetValue()const override	{ return mVal; };

};


//-----------------------------------------------------------------------------
class ActPropRec
{
public:
	std::shared_ptr<IAct> mAct;
	std::shared_ptr<IProp> mProp;

};

struct extr_pid
{
	typedef const wxString& result_type;
	inline result_type operator()(const std::shared_ptr<ActPropRec>& r)const
	{
		return r->mProp->GetId();
	}
};

struct extr_aid
{
	typedef const wxString& result_type;
	inline result_type operator()(const std::shared_ptr<ActPropRec>& r)const
	{
		return r->mAct->GetId();
	}
};

struct extr_pid_aid_ActPropRec
{
	typedef std::pair<const wxString&,const wxString&> result_type;
	inline result_type operator()(const std::shared_ptr<ActPropRec>& r)const
	{
		return std::make_pair(r->mProp->GetId(), r->mAct->GetId());
	}
};


using ActPropTable =
boost::multi_index_container
<
	std::shared_ptr<ActPropRec>,
	indexed_by
	<
		 ordered_non_unique< extr_aid >
		,ordered_non_unique< extr_pid >
		, ordered_unique<extr_pid_aid_ActPropRec  >
		//,ordered_unique<composite_key< extr_pid	,extr_aid> >
		
	>
>;
//-----------------------------------------------------------------------------
class ClsRec : public ICls
{
public:
	wxString	mId;
	wxString	mTitle;
	wxString	mKind;
	wxString	mMeasure;

	virtual const wxString& GetId()const override		{ return mId; };
	virtual const wxString& GetTitle()const override	{ return mTitle; };
	virtual const wxString& GetKind()const override		{ return mKind; };
	virtual const wxString& GetMeasure()const override	{ return mMeasure; };
};
using ClsTable =
boost::multi_index_container
<
	std::shared_ptr<ClsRec>,
	indexed_by
	<
		random_access<> //SQL order
		, ordered_unique< BOOST_MULTI_INDEX_MEMBER(ClsRec, wxString, mId)>
	>
>;
//-----------------------------------------------------------------------------
class StringObjPath: public IObjPath
{
public:
	wxString mPath;
	virtual wxString AsString()const override		{ return mPath; }
};


class ObjRec: public IObj
{
public:
	wxString		mId;
	wxString		mTitle;
	
	std::shared_ptr<const ClsRec> mCls;

	virtual const ICls&		GetCls()const override		{ return *mCls; }
	virtual const wxString& GetId()const override		{ return mId; };
	virtual const wxString& GetTitle()const override	{ return mTitle; };
	
};
using ObjTable =
boost::multi_index_container
<
	std::shared_ptr<ObjRec>,
	indexed_by
	<
		random_access<> //SQL order
		, ordered_unique< BOOST_MULTI_INDEX_MEMBER(ObjRec, wxString, mId)>
	>
>;

//-----------------------------------------------------------------------------
const static ActRec			mMoveActRec;
const static StringObjPath	mEmptyObjPath;

class LogDetails
{
public:
	StringObjPath					mPath;
	std::shared_ptr<PropValTable>	mProperties;

	virtual ~LogDetails(){}
	
	std::shared_ptr<PropValTable>	mActProperties;
	wxString mPropLId;

			const PropValTable& GetProperties()const{ return *mProperties; };
		    const IObjPath&		GetPath()const 		{ return mPath; };
	virtual const ActRec&		GetActRec()const	{ return mMoveActRec; };
	virtual const IObjPath&		GetDstPath()const	{ return mEmptyObjPath; };
	virtual const wxString&		GetQty()const		{ return wxEmptyString2; };
			const wxString&		GetLId()const 		{ return mPropLId; };
			const PropValTable& GetActProperties()const{ return *mActProperties; };
			void SetActProperties(const std::shared_ptr<PropValTable>& act_prop)
			{
				mActProperties = act_prop;
			};
			
};


class LogActRec : public LogDetails
{
public:
	std::shared_ptr<ActRec>			mActRec;
	virtual const ActRec&	GetActRec()const override	 { return *mActRec;	};
};


class LogMovRec : public LogDetails
{
public:
	StringObjPath	mDstPath;
	wxString		mQty;
	virtual const wxString& GetQty()const override		{ return mQty; };
	virtual const IObjPath& GetDstPath()const override	{ return mDstPath; };
	
};

//-----------------------------------------------------------------------------

class LogRec
{
public:
	wxString mId;
	wxString mTimestamp;
	wxString mUser;
	
	std::shared_ptr<ObjRec>		mObj;
	std::shared_ptr<LogDetails>	mDetail;
	

};
using LogTable =
boost::multi_index_container
<
	std::shared_ptr<LogRec>,
	indexed_by
	<
		random_access<> //SQL order
		, ordered_unique< BOOST_MULTI_INDEX_MEMBER(LogRec, wxString, mId)>
	>
>;
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
class ModelHistory
{
	size_t mRowsLimit;
	size_t mRowsOffset;

	ClsTable		mCls;
	ObjTable		mObj;
	PropTable		mProp;
	ActTable		mAct;
	LogTable		mLog;
	ActPropTable	mActProp;

	std::shared_ptr<ModelFilterList> mModelFilterList = std::make_shared<ModelFilterList>();
	sig::scoped_connection connApply;
	wxString mWhere;

	wxString mSort;

	std::shared_ptr<ModelObjPropList> mModelObjPropList = std::make_shared<ModelObjPropList>();

	void LoadPropertyDetails(PropTable& prop_table);
	void LoadActProp(ActPropTable& act_prop_table);
	void PrepareProperties();
	
public:
	ModelHistory();

	void Load();
	void SetWhere(const wxString& where);
	
	void SetRowsLimit(size_t rpp)
	{ 
		mRowsLimit = rpp; 
		sigRowsLimit(mRowsLimit);
	}
	size_t GetRowsLimit()const{ return mRowsLimit; }
	void SetRowsOffset(size_t offset)
	{ 
		mRowsOffset = offset; 
		sigRowsOffset(mRowsOffset);
	}
	size_t GetRowsOffset()const{ return mRowsOffset; }

	std::shared_ptr<ModelFilterList> GetFilterList()
	{ 
		return mModelFilterList; 
	}

	std::shared_ptr<ModelObjPropList> GetObjPropList()
	{
		return mModelObjPropList;
	}


	sig::signal<void(const std::shared_ptr<const ModelHistoryTableData>&)>	 sigAfterLoad;

	sig::signal<void(const size_t&)>	sigRowsOffset;
	sig::signal<void(const size_t&)>	sigRowsLimit;
};
//---------------------------------------------------------------------------
class ModelPageHistory : public IModelWindow
{
	const wxIcon& mIco = ResMgr::GetInstance()->m_ico_history24;
	const wxString mTitle = "История 3";

	sig::scoped_connection connListItemChange;

	rec::PageHistory	mGuiModel;
	ModelHistory		mDataModel;
public:
	ModelPageHistory(const std::shared_ptr<rec::PageHistory>& data);

	ModelHistory& GetModelHistory(){ return mDataModel; }

	void Update();
	void PageForward();
	void PageBackward();

	// IModelWindow
	virtual const wxIcon& GetIcon()const override { return mIco; }
	virtual const wxString& GetTitle()const override { return mTitle; }
	virtual void UpdateTitle()override;
	virtual void Show()override;
	virtual void Load(const boost::property_tree::ptree& page_val)override;
	virtual void Save(boost::property_tree::ptree& page_val)override;

};
//---------------------------------------------------------------------------





} //namespace mvp{
#endif // __IMVP_H