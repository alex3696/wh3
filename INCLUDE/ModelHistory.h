#ifndef __MODEL_HISTORY_H
#define __MODEL_HISTORY_H

#include "ModelHistoryData.h"
#include "IModelWindow.h"

namespace wh{
//-----------------------------------------------------------------------------
class ClsRec
{
public:
	wxString	mId;
	wxString	mTitle;
	wxString	mKind;
	wxString	mMeasure;
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
class ObjRec
{
public:
	wxString mId;
	wxString mTitle;

	std::shared_ptr<const ClsRec> mCls;
	
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
static ActRec mMoveActRec;

class LogDetails
{
public:
	virtual ~LogDetails(){}
	wxString mSrcPath;
	//wxString mLId;
	virtual const ActRec&		GetActRec()const	{ return mMoveActRec; };
	virtual const wxString&		GetSrcPath()const	{ return mSrcPath; };
	virtual const wxString&		GetDstPath()const	{ return wxEmptyString2; };
	virtual const wxString&		GetQty()const		{ return wxEmptyString2; };
	virtual const PropValTable&	GetProperties()const = 0;
};


class LogActRec : public LogDetails
{
public:
	//wxString mLId;
	std::shared_ptr<ActRec>			mActRec;
	std::shared_ptr<PropValTable>	mProperties;

	virtual const ActRec&	GetActRec()const override	 { return *mActRec;	};
	virtual const PropValTable& GetProperties()const override{ return *mProperties; };

};


class LogMovRec : public LogDetails
{
public:
	wxString mDstPath;
	wxString mQty;
	wxString mPropLId;
	std::shared_ptr<PropValTable>	mProperties;

	virtual const wxString& GetDstPath()const override	{ return mDstPath; };
	virtual const wxString& GetQty()const override		{ return mQty; };
	virtual const PropValTable& GetProperties()const override{ return *mProperties; };

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



class ModelHistory
{
	unsigned int mRowsPerPage;

	ClsTable	mCls;
	ObjTable	mObj;
	PropTable	mProp;
	ActTable	mAct;
	LogTable	mLog;

public:
	ModelHistory();

	void Load();
	
	void SetRowsPerPage(size_t rpp){ mRowsPerPage = rpp; }
	size_t GetRowsPerPage()const{ return mRowsPerPage; }

	sig::signal<void(const std::shared_ptr<const ModelHistoryTableData>&)>	 sigAfterLoad;
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
	ModelPageHistory(std::shared_ptr<rec::PageHistory> data);

	ModelHistory& GetModelHistory(){ return mDataModel; }

	void Update();


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