#ifndef __MODEL_BROWSER_H
#define __MODEL_BROWSER_H

#include "ModelBrowserData.h"
#include "IModelWindow.h"
#include "ModelFilterList.h"

namespace wh{

class DbCache;
class ClsCache;
class ObjCache;

//-----------------------------------------------------------------------------
class ClsRec64 : 
	public ICls64
	//, public  std::enable_shared_from_this<ClsRec64>
{
public:
	wxString	mTitle;
	wxString	mMeasure;
	wxString	mObjQty;
private:
	int64_t		mId;
	int64_t		mParentId;
	ClsCache*	mTable = nullptr;
public:
	ClsKind		mKind;
protected:
	//struct MakeSharedEnabler;
public:
	ClsRec64(const int64_t id, ClsCache* table)
		:mId(id), mTable(table)
	{
	}
	
	//static std::shared_ptr<ClsRec64> MakeShared()
	//{
	//	struct MakeSharedEnabler : public ClsRec64
	//	{
	//		MakeSharedEnabler() : ClsRec64() {}
	//	};
	//	return std::make_shared<MakeSharedEnabler>();
	//}
	//bool SetId(const wxString& str) { return str.ToLongLong(&mId); }
	//void SetId(const int64_t& val) { mId = val; }
	
	bool SetParentId(const wxString& str) { return str.ToLongLong(&mParentId); }
	void SetParentId(const int64_t& parentId) { mParentId = parentId; }

	virtual const int64_t&  GetId()const override		{ return mId; }
	virtual const int64_t& GetParentId()const override	{ return mParentId; }
	virtual const wxString& GetTitle()const override	{ return mTitle; };
	virtual       ClsKind	GetKind()const override		{ return mKind; };
	virtual const wxString& GetMeasure()const override	{ return mMeasure; };
	virtual const wxString& GetObjectsQty()const override { return mObjQty; };

	virtual const SpPropValConstTable&	GetProperties()const override { throw; };
	
	virtual void ClearObjTable() override;
	virtual const std::shared_ptr<const ObjTable> GetObjTable()const override;


	virtual std::shared_ptr<const ICls64> GetParent()const override;// {return mParent.lock();}


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
	wxString	mTitle;
	wxString	mQty;
	std::shared_ptr<ObjPath64> mPath;
private:
	int64_t		mId;
	int64_t		mParentId;
	int64_t		mClsId;
	ObjCache*	mTable = nullptr;
public:
	//ObjRec64() {}
	ObjRec64(const int64_t id
			, const int64_t parentId
			, const int64_t clsId
			, ObjCache* table)
		:mId(id), mParentId(parentId), mClsId(clsId), mTable(table)
	{
	}

	bool SetId(const wxString& str) { return str.ToLongLong(&mId); }
	void SetId(const int64_t& val) { mId = val; }

	virtual const int64_t&  GetId()const override { return mId; }
	virtual const wxString& GetTitle()const override { return mTitle; };

	virtual wxString					GetQty()const override { return mQty; };

	bool SetClsId(const wxString& str) { return str.ToLongLong(&mClsId); }

	virtual SpClsConst GetCls()const override;// { return mCls.lock(); }
	virtual int64_t GetClsId()const override  { return mClsId; }

	virtual std::shared_ptr<const IObjPath64> GetPath()const override
	{
		return mPath;
	}

	virtual const SpPropValConstTable&	GetProperties()const override { throw; }

	virtual std::shared_ptr<const IObj64> GetParent()const override;//{return mParent.lock();}
	virtual int64_t GetParentId()const override
	{
		return mParentId;
	}
	bool SetParentId(const wxString& str)
	{
		return str.ToLongLong(&mParentId);
	}

};



//-----------------------------------------------------------------------------

class ClsCache
{
	struct extr_parentId_ICls64
	{
		typedef const int64_t& result_type;
		inline result_type operator()(const std::shared_ptr<const ICls64>& r)const
		{
			return r->GetParentId();
		}
	};
	

	using Cache = boost::multi_index_container
	<
		std::shared_ptr<ClsRec64>,
		indexed_by
		<
			ordered_unique<  extr_id_IIdent64 >
			//, random_access<> //SQL order
			//, ordered_non_unique< extr_parentId_ICls64 >
			//, ordered_unique< extr_void_ptr_IIdent64 >
		>
	>;
	Cache		mCache;
	DbCache*	mDbCache;
public:
	ClsCache(DbCache* dbCache)
		:mDbCache(dbCache)
	{}
	DbCache* GetCache()const { return mDbCache; }

	static const std::shared_ptr<ClsRec64> mNullObj;

	using fnModify = std::function<void(const std::shared_ptr<ClsRec64>& obj)>;

	const std::shared_ptr<ClsRec64>& GetById(const int64_t& id, const fnModify& fn = nullptr)
	{
		auto& idxId = mCache.get<0>();
		auto it = idxId.find(id);
		if (idxId.end() == it)
		{
			if (fn)
			{
				auto cls = std::make_shared<ClsRec64>(id,this);
				fn(cls);
				auto ins_it = idxId.emplace(cls);
				if (ins_it.second)
					return *ins_it.first;
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

	void Clear() { mCache.clear(); }

	

};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class ObjCache
{
	struct extr_parentId_IObj64
	{
		typedef const int64_t result_type;
		inline result_type operator()(const std::shared_ptr<IObj64>& r)const
		{
			return r->GetParentId();
		}
	};

	using Cache = boost::multi_index_container
	<
		std::shared_ptr<ObjRec64>,
		indexed_by
		<
			 ordered_unique < 
		                      composite_key
		                      <
		                       std::shared_ptr<ObjRec64>
		                       , extr_id_IIdent64
		                       , extr_parentId_IObj64
							  > 
			                 >
			, ordered_non_unique< 
								 const_mem_fun
								 <
								  ObjRec64
								  , int64_t
								  , &ObjRec64::GetClsId
								 > 
								>
			//, random_access<> //SQL order	
			//, ordered_unique< extr_void_ptr_IIdent64 >
		>
	>;

	Cache		mCache;
	DbCache*	mDbCache;
public:
	ObjCache(DbCache* dbCache)
		:mDbCache(dbCache)
	{}
	DbCache* GetCache()const { return mDbCache; }

	static const std::shared_ptr<ObjRec64> mNullObj;

	using fnModify = std::function<void(const std::shared_ptr<ObjRec64>& obj)>;


	const std::shared_ptr<ObjRec64>& GetObjById(const int64_t& id, const int64_t& parentId
		, const fnModify& fn = nullptr)
	{
		auto& idxIdParentId = mCache.get<0>();
		auto it = idxIdParentId.find(boost::make_tuple(id, parentId));
		if (idxIdParentId.end() == it)
		{
			if (fn)
			{
				auto obj = std::make_shared<ObjRec64>(id, parentId,0,this);
				fn(obj);
				auto ins_it = idxIdParentId.emplace(obj);//return *mCache.emplace_back(obj).first;
				if(ins_it.second)
					return *ins_it.first;
			}
			//else
			//	return nullptr;
		}
		else
		{
			if (fn)
				idxIdParentId.modify(it, fn);
			return *it;
		}
		return mNullObj;
	}

	
	const std::shared_ptr<const ICls64::ObjTable> GetObjByClsId(const int64_t& clsId)const
	{
		std::shared_ptr<ICls64::ObjTable> table = std::make_shared<ICls64::ObjTable>();

		const auto& idxClsId = mCache.get<1>();

		Cache::nth_index<1>::type::const_iterator it0, it1;

		boost::tuples::tie(it0, it1) = idxClsId.equal_range(clsId);
		while(it0!=it1)
		{
			std::shared_ptr<const IObj64> obj = *it0;
			table->emplace_back(obj);
			++it0;
		}
		return table;
	}
	
	void DeleteObjByClsId(const int64_t& clsId)
	{
		auto& idxClsId = mCache.get<1>();
		idxClsId.erase(clsId);
	}

	void Clear() { mCache.clear(); }


};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class DbCache
{
public:
	DbCache()
		:mClsTable(this)
		, mObjTable(this)
	{
	
	}

	void Clear()
	{
		mObjTable.Clear();
		mClsTable.Clear();
	}

	ClsCache mClsTable;
	ObjCache mObjTable;
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class ClsTree
{
	std::shared_ptr<ICls64> mRoot;
	std::shared_ptr<ICls64> mCurrent;
	
	void Refresh();
public:
	ClsCache*	mClsCache;
	sig::signal<void(const ICls64&)> sigBeforePathChange;
	sig::signal<void(const ICls64&)> sigAfterPathChange;

	ClsTree(ClsCache* clsTable);

	void SetId(const wxString& str);
	void SetId(const int64_t& val);
	void Up();


	std::shared_ptr<ICls64> GetCurrent() { return mCurrent; }

};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class ModelBrowser
{
	bool mGroupByType;

	std::unique_ptr<ClsTree> mClsPath;

	DbCache		mCache;
	//ClsCache	mClsCache;
	//ObjCache	mObjCache;

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