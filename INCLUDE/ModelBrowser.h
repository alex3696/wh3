#ifndef __MODEL_BROWSER_H
#define __MODEL_BROWSER_H

#include "ModelBrowserData.h"
#include "IModelWindow.h"
#include "ModelFilterList.h"

namespace wh{

//-----------------------------------------------------------------------------
class ClsRec64 : 
	public ICls64
	, public  std::enable_shared_from_this<ClsRec64>
{
protected:
	ClsRec64() {}
	struct MakeSharedEnabler;
public:
	
	static std::shared_ptr<ClsRec64> MakeShared()
	{
		struct MakeSharedEnabler : public ClsRec64
		{
			MakeSharedEnabler() : ClsRec64() {}
		};
		return std::make_shared<MakeSharedEnabler>();
	}


	int64_t		mId;
	wxString	mTitle;
	wxString	mMeasure;
	wxString	mObjQty;
	ClsKind		mKind;



	bool SetId(const wxString& str){ return str.ToLongLong(&mId); }
	void SetId(const int64_t& val) { mId = val; }

	virtual const int64_t&  GetId()const override		{ return mId; }
	virtual const wxString& GetTitle()const override	{ return mTitle; };
	virtual       ClsKind	GetKind()const override		{ return mKind; };
	virtual const wxString& GetMeasure()const override	{ return mMeasure; };
	virtual const wxString& GetObjectsQty()const override { return mObjQty; };

	virtual const SpPropValConstTable&	GetProperties()const override { throw; };

	

	std::shared_ptr<ObjTable>	mObjTable;
	virtual void ClearObjTable() override
	{
		mObjTable.reset();
	}
	virtual void AddObj(const std::shared_ptr<const IObj64>& new_obj) override
	{
		if (!mObjTable)
			mObjTable = std::make_shared<ObjTable>();
		mObjTable->emplace_back(new_obj);
	}
	virtual const std::shared_ptr<const ObjTable> GetObjTable()const override
	{
		return mObjTable;
	}
	//void RefreshObjects();

	std::weak_ptr<const ICls64>		mParent;
	std::shared_ptr<ChildsTable>	mChild;

	virtual std::shared_ptr<const ICls64> GetParent()const override
	{
		return mParent.lock();
	}
	virtual void SetParent(const std::shared_ptr<const ICls64>& parent) override
	{
		mParent = parent;
	}
	virtual void ClearChilds() override
	{
		mChild.reset();
	}
	virtual void AddChild(const std::shared_ptr<ICls64>& new_child)override
	{
		if (!mChild)
			mChild = std::make_shared<ChildsTable>();
		mChild->emplace_back(new_child);
	}
	virtual const std::shared_ptr<const ChildsTable> GetChilds()const override
	{
		return mChild;
	}
	void RefreshChilds();

};
//-----------------------------------------------------------------------------
class ObjPath64 : public IObjPath64
{
public:
	wxString	mStrPath;
	
	virtual wxString AsString()const override
	{
		return mStrPath;
	}


};
//-----------------------------------------------------------------------------

class ObjRec64 : public IObj64
{
public:
	ObjRec64() {}
	
	int64_t		mId;
	wxString	mTitle;
	wxString	mQty;
	
	std::shared_ptr<ObjPath64> mPath;

	std::weak_ptr<const ICls64>	mCls;

	bool SetId(const wxString& str) { return str.ToLongLong(&mId); }
	void SetId(const int64_t& val) { mId = val; }

	virtual const int64_t&  GetId()const override { return mId; }
	virtual const wxString& GetTitle()const override { return mTitle; };

	virtual wxString					GetQty()const override { return mQty; };

	virtual SpClsConst GetCls()const override { return mCls.lock(); }

	virtual std::shared_ptr<const IObjPath64> GetPath()const override
	{
		return mPath;
	}

	virtual const SpPropValConstTable&	GetProperties()const override { throw; }


	std::weak_ptr<const IObj64>		mParent;
	std::shared_ptr<ChildsTable>	mChild;


	virtual std::shared_ptr<const IObj64> GetParent()const override
	{
		return mParent.lock();
	}
	virtual void SetParent(const std::shared_ptr<const IObj64>& parent) override
	{
		mParent = parent;
	}
	virtual void ClearChilds() override
	{
		mChild.reset();
	}
	virtual void AddChild(const std::shared_ptr<IObj64>& new_child)override
	{
		if (!mChild)
			mChild = std::make_shared<ChildsTable>();
		mChild->emplace_back(new_child);
	}
	virtual const std::shared_ptr<const ChildsTable> GetChilds()const override
	{
		return mChild;
	}

};



//-----------------------------------------------------------------------------
class ClsTree 
{
	std::shared_ptr<ICls64> mRoot;
	std::shared_ptr<ICls64> mCurrent;

	void Refresh();
public:
	sig::signal<void(const ICls64&)> sigBeforePathChange;
	sig::signal<void(const ICls64&)> sigAfterPathChange;

	ClsTree();
	
	void SetId(const wxString& str);
	void SetId(const int64_t& val);
	void Up();
	

	std::shared_ptr<ICls64> GetCurrent() { return mCurrent; }

};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class ClsCache
{
public:
	using Cache = boost::multi_index_container
		<
		std::shared_ptr<ClsRec64>,
		indexed_by
		<
		random_access<> //SQL order
		, ordered_unique< extr_id_IIdent64 >
		, ordered_unique< extr_void_ptr_IIdent64 >
		>
		>;
	using fnModify = std::function<void(const std::shared_ptr<ClsRec64>& obj)>;


	const std::shared_ptr<ClsRec64>& GetById(const int64_t& id, const fnModify& fn = nullptr)
	{
		auto& idxId = mCache.get<1>();
		auto it = idxId.find(id);
		if (idxId.end() == it)
		{
			if (fn)
			{
				auto obj = ClsRec64::MakeShared(); //std::make_shared<ClsRec64>();
				fn(obj);
				return *mCache.emplace_back(obj).first;
			}
			//else
			//	return nullptr;
		}
		else
		{
			if (fn)
				idxId.modify(it, fn);
			return *it;
		}
		return mNullObj;
	}
private:
	Cache mCache;
	std::shared_ptr<ClsRec64> mNullObj;

};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class ObjCache
{
public:
	using Cache = boost::multi_index_container
	<
		std::shared_ptr<ObjRec64>,
		indexed_by
		<
			random_access<> //SQL order
			, ordered_unique< extr_id_IIdent64 >
			, ordered_unique< extr_void_ptr_IIdent64 >
		>
	>;
	using fnModify = std::function<void(const std::shared_ptr<ObjRec64>& obj)>;


	const std::shared_ptr<ObjRec64>& GetObjById(const int64_t& id, const fnModify& fn = nullptr)
	{
		auto& idxId = mCache.get<1>();
		auto it = idxId.find(id);
		if (idxId.end() == it)
		{
			if (fn)
			{
				auto obj = std::make_shared<ObjRec64>();
				fn(obj);
				return *mCache.emplace_back(obj).first;
			}
			//else
			//	return nullptr;
		}
		else
		{
			if (fn)
				idxId.modify(it, fn);
			return *it;
		}
		return mNullObj;
	}
private:
	Cache mCache;
	std::shared_ptr<ObjRec64> mNullObj;

};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ModelBrowser
{
	bool mGroupByType;

	ClsTree		mClsPath;

	ClsCache	mClsCache;
	ObjCache	mObjCache;

	void DoRefreshObjects(const std::shared_ptr<ICls64>& cls);
public:
	ModelBrowser();
	~ModelBrowser();

	void DoRefresh();
	void DoActivate(const ICls64* cls);
	void DoUp();


	void DoAct();
	void DoMove();
	void DoFind(const wxString&);
	void DoAddType();
	void DoAddObject();
	void DoDeleteSelected();
	void DoUpdateSelected();
	void DoGroupByType(bool enable_group_by_type);
		
	sig::signal<void(bool)>	 sigGroupByType;
	
	sig::signal<void(const ICls64&)> sigBeforePathChange;
	sig::signal<void(const ICls64&)> sigAfterPathChange;
	
	using SigRefreshCls = 
		sig::signal<void(const std::vector<const ICls64*>&, const ICls64*)>;
	
	SigRefreshCls	sigBeforeRefreshCls;
	SigRefreshCls	sigAfterRefreshCls;


	sig::signal<void(Operation op
		, const std::vector<const IObj64*>& )>	sigObjOperation;
	//sig::signal<void(Operation op
	//	, const std::vector<const ICls64*>&)>	sigClsOperation;


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