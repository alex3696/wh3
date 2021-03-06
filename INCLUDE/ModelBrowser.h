#ifndef __MODEL_BROWSER_H
#define __MODEL_BROWSER_H

#include "ModelBrowserData.h"
#include "ModelActTable.h"
#include "ModelPropTable.h"
#include "IModelWindow.h"
#include "ModelFilterList.h"
#include "ModelClsPath.h"
#include "ModelObjPath.h"

namespace wh{

class DbCache;
class ClsCache;
class ObjCache;
class ActCache;
class PropCache;

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
	int64_t		mParentId = 0;
	ClsCache*	mTable = nullptr;
public:
	ClsKind		mKind = ClsKind::Abstract;
	bool		mObjLoaded = false;

	// prop headers
	ConstPropTable		mFavObjProp;
	ConstClsFavActTable mFavActProp;
	// prop values
	FavAPropValueTable	mFavAPropValues;

	// prop header+value
	ConstPropValTable	mFavProp;
protected:
	//struct MakeSharedEnabler;
	void ParseActInfo(const boost::property_tree::wptree& favAPropValues);
	void ParsePropInfo(const boost::property_tree::wptree& favOPropValues);
public:
	ClsRec64(const int64_t id, ClsCache* table)
		:mId(id), mTable(table)
	{
	}
	ClsRec64(const int64_t id, const wxString& title, ClsKind kind
		, ClsCache* table)
		:mId(id), mTitle(title), mKind(kind)
		, mTable(table)
	{
	}

	void ParseFavProp(const wxString& favOPropValues);
	
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
	virtual		  int64_t	GetParentId()const override	{ return mParentId; }
	virtual const wxString& GetTitle()const override	{ return mTitle; };
	virtual       ClsKind	GetKind()const override		{ return mKind; };
	virtual const wxString& GetMeasure()const override	{ return mMeasure; };
	virtual const wxString& GetObjectsQty()const override { return mObjQty; };

	virtual const SpPropValConstTable&	GetProperties()const override { throw; };
	
	virtual void ClearObjTable() override;
	virtual const std::shared_ptr<const ObjTable> GetObjTable()const override;
	virtual const std::shared_ptr<const ClsTable> GetClsChilds()const override;

	virtual std::shared_ptr<const ICls64> GetParent()const override;// {return mParent.lock();}

	virtual const ConstPropValTable&	GetFavCPropValue()const override;
	virtual const ConstClsFavActTable&	GetFavAProp()const override;
	virtual const FavAPropValueTable&	GetFavAPropValue()const override;
	virtual const ConstPropTable&		GetFavOProp()const override;
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

	virtual size_t GetQty()const override 
	{
		return 0; // parse opath
	}
	virtual const IIdent64* GetItem(size_t)const override
	{
		return nullptr;
	}
	virtual const wxString GetItemString(size_t)const override
	{
		return wxEmptyString;
	}


};
//-----------------------------------------------------------------------------

class ObjRec64 : public IObj64
{
public:
	wxString	mTitle;
	wxString	mQty;
	wxString	mSavedQty;
	std::shared_ptr<ObjPath64> mPath;
	wxString	mLockUser;
	wxString	mLockTime;
	wxString	mLockSession;

	FavAPropValueTable	mFavAPropValueTable;
	ConstPropValTable	mFavOPropValueTable;
private:
	int64_t		mId;
	int64_t		mParentId;
	int64_t		mClsId;
	ObjCache*	mTable = nullptr;

	void ParseActInfo(const boost::property_tree::wptree& favAPropValues);
	void ParsePropInfo(const boost::property_tree::wptree& favOPropValues);
public:
	//ObjRec64() {}
	ObjRec64(const int64_t id
			, const int64_t parentId
			, const int64_t clsId
			, ObjCache* table)
		:mId(id), mParentId(parentId), mClsId(clsId), mTable(table)
	{
	}
	void ParseFavProp(const wxString& favOPropValues);



	bool SetId(const wxString& str) { return str.ToLongLong(&mId); }
	void SetId(const int64_t& val) { mId = val; }

	virtual const int64_t&  GetId()const override { return mId; }
	virtual const wxString& GetTitle()const override { return mTitle; };

	virtual wxString					GetQty()const override { return mQty; };

	bool SetClsId(const wxString& str) { return str.ToLongLong(&mClsId); }
	bool SetClsId(const int64_t& cid) { return mClsId=cid; }

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

	virtual int GetActPrevios(int64_t aid, wxDateTime& dt)const override;
	virtual int GetActNext(int64_t aid, wxDateTime& dt)const override;
	virtual int GetActLeft(int64_t aid, double& dt)const override;
	
	virtual const FavAPropValueTable&	GetFavAPropValue()const override;
	virtual const ConstPropValTable&	GetFavOPropValue()const override;

	virtual wxString GetLockUser()const override;
	virtual wxString GetLockTime()const override;
	virtual wxString GetLockSession()const override;

};

//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------

class ClsCache
{
	struct extr_parentId_ICls64
	{
		typedef int64_t result_type;
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
			, ordered_non_unique< extr_parentId_ICls64 >
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

	static const std::shared_ptr<ClsRec64> NullValue;

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
		return NullValue;
	}

	void Clear() { mCache.clear(); }

	void GetClsChilds(const int64_t& clsId, ICls64::ClsTable& table)const
	{
		table.clear();

		const auto& idxClsId = mCache.get<1>();

		Cache::nth_index<1>::type::const_iterator it0, it1;

		boost::tuples::tie(it0, it1) = idxClsId.equal_range(clsId);
		while (it0 != it1)
		{
			std::shared_ptr<const ICls64> cls = *it0;
			table.emplace_back(cls);
			++it0;
		}
	}

};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class ObjCache
{
	typedef ObjRec64 RowType;

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
		std::shared_ptr<RowType>,
		indexed_by
		<
			 ordered_unique < 
		                      composite_key
		                      <
		                       std::shared_ptr<RowType>
		                       , extr_id_IIdent64
		                       , extr_parentId_IObj64
							  > 
			                 >
			, ordered_non_unique< 
								 const_mem_fun
								 <
									RowType
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


	void GetObjByClsId(const int64_t& clsId, std::vector<const IIdent64*>& table)const
	{
		table.clear();

		const auto& idxClsId = mCache.get<1>();

		Cache::nth_index<1>::type::const_iterator it0, it1;

		boost::tuples::tie(it0, it1) = idxClsId.equal_range(clsId);
		while(it0!=it1)
		{
			std::shared_ptr<const IObj64> obj = *it0;
			table.emplace_back(obj.get());
			++it0;
		}
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
// ACT TABLE
//-----------------------------------------------------------------------------
class ActCache: public ModelActTable
{
private:
	DbCache*	mDbCache;
public:
	ActCache(DbCache* dbCache)
		:mDbCache(dbCache)
	{}
	DbCache* GetCache()const { return mDbCache; }
};

class PropCache : public ModelPropTable
{
private:
	DbCache*	mDbCache;
public:
	PropCache(DbCache* dbCache)
		:mDbCache(dbCache)
	{}
	DbCache* GetCache()const { return mDbCache; }
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
		, mActTable(this)
		, mPropTable(this)
	{
	
	}

	void Clear()
	{
		mObjTable.Clear();
		mClsTable.Clear();
		mActTable.Clear();
		mPropTable.Clear();
	}

	ClsCache mClsTable;
	ObjCache mObjTable;
	ActCache mActTable;
	PropCache mPropTable;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class ModelBrowser : public IModelWindow
{
public:
	enum class Mode
	{
		  ByType = 0
		, ByPath = 1
	};


private:
	static void ParseSearch(const wxString& ss, std::vector<wxString>& words);
	
	
	int			mMode;
	int64_t		mRootId;
	bool		mGroupByType;
	wxString	mSearchString;
	std::set<ObjectKey> mSetObjects;

	std::unique_ptr<ModelClsPath> mClsPath;
	std::unique_ptr<ModelObjPath> mObjPath;

	DbCache		mCache;
	
	void Load_ObjDir_ClsList();
	void Load_ObjDir_ObjList();
	void Load_ObjDir_ObjList(int64_t cid);
	//void Load_ObjDir_ObjList(int64_t cid);

	void Load_ClsDir_ClsList();
	void LoadSearch();
	void LoadSetObjects();


	void UpdateUntitledProperties();
	void UpdateUntitledActs();

	void ExecuteMoveObjects(const std::set<ObjectKey>& obj)const;
	void ExecuteActObjects(const std::set<ObjectKey>& obj)const;
public:
	ModelBrowser();
	~ModelBrowser();

	int			GetMode()const;
	int64_t		GetRootId()const;
	wxString	GetSearchString()const;
	bool		GetGroupedByType()const;
	wxString	GetObjectUpdatedQty(const ObjectKey& key);

	void SetMode(int);
	void SetRootId(int64_t);
	void SetSearchString(const wxString&);
	void SetGroupedByType(bool);

	const wxString GetRootTitle()const;

	void DoRefresh(bool sigBefore=true);
	void DoActivate(int64_t cid);
	void DoRefreshObjects(int64_t cid);
	void DoUp();
	void DoFind(const wxString&);
	void DoSetObjects(const std::set<ObjectKey>& obj);

	void DoGroupByType(bool enable_group_by_type);
	void DoToggleGroupByType();
	void DoSetMode(int);
	void Goto(int mode,int64_t id);

	void DoMove();
	void DoAct();
	void DoShowDetails();
	void DoDelete();
	bool DoSetQty(const ObjectKey&, const wxString&);

	sig::signal<void(std::vector<const IIdent64*>&)> sigGetSelection;
		
	using SigPathChange = sig::signal<void(const wxString&)>;
	SigPathChange sigAfterPathChange;
	
	using SigRefreshCls = 
		sig::signal	<void	(	const std::vector<const IIdent64*>&
								, const IIdent64*
								, const wxString&
								, bool
								, int
							)
					>;
	
	SigRefreshCls	sigBeforeRefreshCls;
	SigRefreshCls	sigAfterRefreshCls;


	sig::signal<void(Operation op
		, const std::vector<const IIdent64*>& )>	sigObjOperation;
	//sig::signal<void(Operation op
	//	, const std::vector<const ICls64*>&)>	sigClsOperation;


};
//-----------------------------------------------------------------------------
class ModelPageBrowser : public IModelWindow
{

	sig::scoped_connection connModelBrowser_AfterPathChange;

	std::shared_ptr<ModelBrowser> mModelBrowser;
public:
	ModelPageBrowser();
	ModelPageBrowser(int mode, int64_t rood_id, bool group, const wxString& ss);

	std::shared_ptr<ModelBrowser> GetModelBrowser(){ return mModelBrowser; }

	// IModelWindow
	virtual void UpdateTitle()override;
	virtual void Show()override;
	virtual void Load(const boost::property_tree::wptree& page_val)override;
	virtual void Save(boost::property_tree::wptree& page_val)override;

};
//-----------------------------------------------------------------------------




} //namespace wh{
#endif // __*_H