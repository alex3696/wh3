#ifndef __MODEL_BROWSER_H
#define __MODEL_BROWSER_H

#include "ModelBrowserData.h"
#include "IModelWindow.h"
#include "ModelFilterList.h"

namespace wh{

//-----------------------------------------------------------------------------

enum class BrowserMode
{
	  Home = 0
	, ByType = 1
	, ByPath = 2
	, FilterByType = 10
	, FilterByPath = 20
	, FindByType = 100
	, FindByPath = 200
};

//-----------------------------------------------------------------------------
class ClsRec64 : public ICls64
{
public:
	ClsRec64(){}

	int64_t		mId;
	wxString	mTitle;
	ClsKind		mKind;
	wxString	mMeasure;

	bool SetId(const wxString& str){ return str.ToLongLong(&mId); }
	void SetId(const int64_t& val) { mId = val; }

	virtual const int64_t&  GetId()const override		{ return mId; }
	virtual const wxString& GetTitle()const override	{ return mTitle; };
	virtual       ClsKind	GetKind()const override		{ return mKind; };
	virtual const wxString& GetMeasure()const override	{ return mMeasure; };

	virtual const SpObjConstTable&		GetObjects()const override { throw; };
	virtual const SpPropValConstTable&	GetProperties()const override { throw; };

};

//-----------------------------------------------------------------------------
class ClsTree : public IPath64
{
	std::shared_ptr<ClsNode> mRoot;
	std::shared_ptr<ClsNode> mCurrent;
public:
	sig::signal<void(const ClsNode&)> sigBeforePathChange;
	sig::signal<void(const ClsNode&)> sigAfterPathChange;

	ClsTree();
	std::shared_ptr<ClsNode> GetCurrent()const { return mCurrent; }

	void Home();
	void Refresh();
	void Up();

	void SetId(const wxString& str);
	void SetId(const int64_t& val);

	int64_t  GetId()const;
	wxString GetIdAsString()const;

	
	std::shared_ptr<const ClsNode> AddValToCurrent(const std::shared_ptr<ICls64>& val);


	virtual wxString AsString()const override;
};




//-----------------------------------------------------------------------------
class ModelBrowser
{
	ClsTree		mClsPath;

	std::vector<std::shared_ptr<ClsNode>> mClsNodeCache;
	std::vector<std::shared_ptr<ClsRec64>> mClsValCache;



	SpClsTable	mClsAll;
	

public:
	ModelBrowser();
	~ModelBrowser();

	void DoRefresh();
	void DoActivate(const ClsNode& item);
	void DoUp();

	void DoAct();
	void DoMove();
	void DoFind(const wxString&);
	void DoAddType();
	void DoAddObject();
	void DoDeleteSelected();
	void DoUpdateSelected();
	void DoGroupByType(bool enable_group_by_type);
	void DoCollapseGroupByType(bool enable_collapse_by_type);
		
	sig::signal<void(bool)>	 sigGroupByType;
	sig::signal<void(bool)>	 sigCollapsedGroupByType;
	sig::signal<void(const NotyfyTable&)> sigSelected;
	
	sig::signal<void()> sigClear;
	sig::signal<void(const ClsNode&, const NotyfyTable&)> sigAfterInsert;
	sig::signal<void(const ClsNode&, const NotyfyTable&)> sigAfterUpdate;
	sig::signal<void(const ClsNode&, const NotyfyTable&)> sigBeforeDelete;

	sig::signal<void(const ClsNode&)> sigBeforePathChange;
	sig::signal<void(const ClsNode&)> sigAfterPathChange;

	sig::signal<void(const int)> sigModeChanged;


};
//-----------------------------------------------------------------------------
class ModelPageBrowser : public IModelWindow
{
	const wxIcon& mIco = ResMgr::GetInstance()->m_ico_type24;
	const wxString mTitle = "Object Browser";

	bool mFiltersVisible;
	bool mGroupedByType;
	bool mCollapsedGroup;

	ModelBrowser mModelBrowser;
public:
	ModelPageBrowser();

	ModelBrowser *const GetModelBrowser(){ return &mModelBrowser; }

	void DoShowFilters(bool show_filters);
	void DoEnableGroupByType(bool enable_group_by_type);
	void DoCollapsedGroupByType(bool enable_collapse_by_type);

	sig::signal<void(bool)>	 sigUpdateVisibleFilters;
	sig::signal<void(bool)>	 sigUpdateGroupByType;
	sig::signal<void(bool)>	 sigUpdateCollapsedGroupByType;


	// IModelWindow
	virtual const wxIcon& GetIcon()const override { return mIco; }
	virtual const wxString& GetTitle()const override { return mTitle; }
	virtual void UpdateTitle()override;
	virtual void Show()override;
	virtual void Load(const boost::property_tree::ptree& page_val)override;
	virtual void Save(boost::property_tree::ptree& page_val)override;

};
//-----------------------------------------------------------------------------




} //namespace wh{
#endif // __*_H