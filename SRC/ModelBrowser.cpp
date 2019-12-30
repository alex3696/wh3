#include "_pch.h"
#include "ModelBrowser.h"

#include "dlg_act_view_Frame.h"
#include "MoveObjPresenter.h"


using namespace wh;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


const std::shared_ptr<ClsRec64>  ClsCache::NullValue = std::shared_ptr<ClsRec64>(nullptr);
const std::shared_ptr<ObjRec64>  ObjCache::mNullObj = std::shared_ptr<ObjRec64>(nullptr);



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//virtual 
bool ClsRec64::IsSelected()const// override
{
	if (mTable->GetCache()->mClsSelection.empty())
		return false;
	const auto cid = this->GetId();
	auto& idxId = mTable->GetCache()->mClsSelection;
	auto it = idxId.find(cid);
	return (idxId.end() != it);
}
//-----------------------------------------------------------------------------
void ClsRec64::ParseFavProp(const wxString& str)
{
	mFavProp.clear();
	mFavAPropValues.clear();
	mFavActProp.clear();
	if (str.empty())
		return;

	std::wstringstream ss(str.ToStdWstring());
	boost::property_tree::wptree all_fav_prop;
	boost::property_tree::read_json(ss, all_fav_prop);

	auto find_it = all_fav_prop.find(L"fav_act");
	if (all_fav_prop.not_found() != find_it)
	{
		ParseActInfo(find_it->second);
	}
	find_it = all_fav_prop.find(L"fav_cprop");
	if (all_fav_prop.not_found() != find_it)
	{
		ParsePropInfo(find_it->second);
	}
}
//-----------------------------------------------------------------------------
void ClsRec64::ParseActInfo(const boost::property_tree::wptree& favAPropValues)
{

	if (favAPropValues.empty())
		return;
	auto cache = this->mTable->GetCache();

	auto begin = favAPropValues.begin();
	auto end = favAPropValues.end();
	while (begin != end)
	{
		auto act_curr = begin->second.begin();
		auto act_end = begin->second.end();
		std::wstring str_aid = begin->first.data();
		int64_t aid = std::stoll(str_aid);
		while (act_curr != act_end)
		{
			std::wstring str_info = act_curr->first.data();
			FavAPropInfo info = StrInt2FavAPropInfo(str_info);
			std::wstring info_val = act_curr->second.data();

			if (!info_val.empty())
			{
				const ActCache::fnModify fn_act_upsert
					= [](const std::shared_ptr<IAct64>& act) {};
				auto act = cache->mActTable.InsertOrUpdate(aid, fn_act_upsert);
				auto favAProp = std::make_shared<const FavAProp>(act, info);
				this->mFavActProp.emplace(favAProp);

				auto favAPropVal
					= std::make_shared<const FavAPropValue>(favAProp, info_val);
				this->mFavAPropValues.emplace(favAPropVal);
			}
			else
			{
				auto isd = act_curr;;
			}
			++act_curr;
		}

		++begin;
	}//while (begin != end)
}
//-----------------------------------------------------------------------------
void ClsRec64::ParsePropInfo(const boost::property_tree::wptree& favOPropValues)
{
	if (favOPropValues.empty())
		return;
	auto cache = this->mTable->GetCache();

	auto prop_curr = favOPropValues.begin();
	auto prop_end = favOPropValues.end();
	while (prop_curr != prop_end)
	{
		wxString pid_str = prop_curr->first.data();
		wxString val_str = prop_curr->second.data();
		int64_t pid;
		if (pid_str.ToLongLong(&pid))
		{
			PropCache::fnModify fn = [](const std::shared_ptr<IProp64>&) {};
			const auto& prop = cache->mPropTable.InsertOrUpdate(pid, fn);

			auto pv = std::make_shared<PropVal>(prop, val_str);
			mFavProp.emplace(pv);
		}
		++prop_curr;
	}//while (prop_curr != prop_end)

}
//-----------------------------------------------------------------------------
//virtual 
std::shared_ptr<const ICls64> ClsRec64::GetParent()const //override
{
	return mTable ? mTable->GetById(mParentId) : nullptr;
}
//-----------------------------------------------------------------------------
//virtual 
void ClsRec64::ClearObjTable() //override
{
	mTable->GetCache()->mObjTable.DeleteObjByClsId(GetId());
}
//-----------------------------------------------------------------------------
//virtual 
const std::shared_ptr<const ICls64::ObjTable> ClsRec64::GetObjTable()const //override;
{
	//return mObjTable;
	return mTable->GetCache()->mObjTable.GetObjByClsId(GetId());
}
//-----------------------------------------------------------------------------
//virtual 
const std::shared_ptr<const ICls64::ClsTable> ClsRec64::GetClsChilds()const //override;
{
	auto table = std::make_shared<ICls64::ClsTable>();
	mTable->GetClsChilds(this->mId, *table);
	return table;
}
//-----------------------------------------------------------------------------
const ConstPropValTable& ClsRec64::GetFavCPropValue() const
{
	return mFavProp;
}
//-----------------------------------------------------------------------------
const ConstClsFavActTable& ClsRec64::GetFavAProp() const
{
	return mFavActProp;
}
//-----------------------------------------------------------------------------
const FavAPropValueTable& ClsRec64::GetFavAPropValue() const
{
	return mFavAPropValues;
}
//-----------------------------------------------------------------------------
const ConstPropTable& ClsRec64::GetFavOProp() const
{
	return mFavObjProp;
}







//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void ObjRec64::ParseFavProp(const wxString& str)
{
	mFavAPropValueTable.clear();
	mFavOPropValueTable.clear();
	if (str.empty())
		return;

	std::wstringstream ss(str.ToStdWstring());
	boost::property_tree::wptree all_fav_prop;
	boost::property_tree::read_json(ss, all_fav_prop);

	auto find_it = all_fav_prop.find(L"fav_act");
	if (all_fav_prop.not_found() != find_it)
	{
		ParseActInfo(find_it->second);
	}
	find_it = all_fav_prop.find(L"fav_oprop");
	if (all_fav_prop.not_found() != find_it)
	{
		ParsePropInfo(find_it->second);
	}
}
//-----------------------------------------------------------------------------
//virtual 
bool ObjRec64::IsSelected()const// override
{
	if (mTable->GetCache()->mObjSelection.empty())
		return false;
	const auto okey = ObjectKey(this->GetId(), this->GetParentId());
	auto& idxId = mTable->GetCache()->mObjSelection;
	auto it = idxId.find(okey);
	return (idxId.end() != it);
}
//-----------------------------------------------------------------------------
void ObjRec64::ParseActInfo(const boost::property_tree::wptree& favAPropValues)
{
	
	if (favAPropValues.empty())
		return;
	auto cache = this->mTable->GetCache();

	auto begin = favAPropValues.begin();
	auto end = favAPropValues.end();
	while (begin != end)
	{
		auto act_curr = begin->second.begin();
		auto act_end = begin->second.end();
		std::wstring str_aid = begin->first.data();
		int64_t aid = std::stoll(str_aid);
		while (act_curr != act_end)
		{
			std::wstring str_info = act_curr->first.data();
			FavAPropInfo info = StrInt2FavAPropInfo(str_info);
			std::wstring info_val = act_curr->second.data();

			if (!info_val.empty())
			{
				const ActCache::fnModify fn_act_upsert
					= [](const std::shared_ptr<IAct64>& act) {};
				auto act = cache->mActTable.InsertOrUpdate(aid, fn_act_upsert);
				auto favAProp = std::make_shared<const FavAProp>(act, info);


				const ClsCache::fnModify upd_cls
					= [&favAProp, &info_val, &cache](const std::shared_ptr<ClsRec64>& cls)
				{
					auto ins_it = cls->mFavActProp.emplace(favAProp);
					switch (favAProp->mInfo)
					{
					case FavAPropInfo::PeriodInterval:
					case FavAPropInfo::PeriodSec:
					case FavAPropInfo::PeriodDay:{
						auto favAPropVal = std::make_shared<const FavAPropValue>(favAProp, info_val);
						cls->mFavAPropValues.emplace(favAPropVal);
					}break;
					default:break;
					}

				};
				cache->mClsTable.GetById(GetClsId(), upd_cls);

				auto favAPropVal
					= std::make_shared<const FavAPropValue>(favAProp, info_val);
				this->mFavAPropValueTable.emplace(favAPropVal);
			}
			else 
			{
				auto isd = act_curr;;
			}
			++act_curr;
		}

		++begin;
	}//while (begin != end)
}
//-----------------------------------------------------------------------------
void ObjRec64::ParsePropInfo(const boost::property_tree::wptree& favOPropValues)
{
	if (favOPropValues.empty())
		return;
	auto cache = this->mTable->GetCache();

	auto prop_curr = favOPropValues.begin();
	auto prop_end = favOPropValues.end();
	while (prop_curr != prop_end)
	{
		wxString pid_str = prop_curr->first.data();
		wxString val_str = prop_curr->second.data();
		int64_t pid;
		if (pid_str.ToLongLong(&pid))
		{
			PropCache::fnModify fn = [](const std::shared_ptr<IProp64>&) {};
			const auto& prop = cache->mPropTable.InsertOrUpdate(pid, fn);

			auto pv = std::make_shared<PropVal>(prop, val_str);
			mFavOPropValueTable.emplace(pv);
		}
		++prop_curr;
	}//while (prop_curr != prop_end)	

}
//-----------------------------------------------------------------------------
//virtual 
SpClsConst ObjRec64::GetCls()const //override 
{ 
	return mTable ? mTable->GetCache()->mClsTable.GetById(mClsId) : nullptr;
}
//-----------------------------------------------------------------------------
//virtual 
std::shared_ptr<const IObj64> ObjRec64::GetParent()const //override
{
	return mTable ? mTable->GetObjById(mId, mParentId) : nullptr;
}
//-----------------------------------------------------------------------------
//virtual 
int ObjRec64::GetActPrevios(int64_t aid, wxDateTime& dt)const //override
{
	const auto& idxFAV = mFavAPropValueTable.get<0>();
	auto fit = idxFAV.find(boost::make_tuple(aid, FavAPropInfo::PreviosDate));
	if (idxFAV.end() == fit)
		return 0;

	const wxString& str_value = (*fit)->mValue;
	
	if (str_value == "null")
		return -1;

	if (!dt.ParseISOCombined(str_value, ' '))
		if (!dt.ParseISOCombined(str_value, 'T'))
			if (!dt.ParseDate(str_value))
				return 0;

	return dt.IsValid()? 1 : 0;
}
//-----------------------------------------------------------------------------
//virtual 
int ObjRec64::GetActNext(int64_t aid, wxDateTime& next)const //override
{
	const auto& idxFAV = mFavAPropValueTable.get<0>();
	auto fit = idxFAV.find(boost::make_tuple(aid, FavAPropInfo::NextDate));
	if (idxFAV.end() == fit)
		return 0;

	const wxString& str_value = (*fit)->mValue;

	if (str_value == "null")
		return -1;

	if (!next.ParseISOCombined(str_value, ' '))
		if (!next.ParseISOCombined(str_value, 'T'))
			if (!next.ParseDate(str_value))
				return 0;

	return next.IsValid() ? 1 : 0;
}
//-----------------------------------------------------------------------------
//virtual 
int ObjRec64::GetActLeft(int64_t aid, double& left)const //override
{
	const auto& idxFAV = mFavAPropValueTable.get<0>();
	auto fit = idxFAV.find(boost::make_tuple(aid, FavAPropInfo::LeftDay));
	if (idxFAV.end() == fit)
		return 0;

	const wxString& str_value = (*fit)->mValue;

	if (str_value == "null")
		return -1;

	if(wxString(str_value).ToCDouble(&left))
		return true;

	return false;
}
//-----------------------------------------------------------------------------
const FavAPropValueTable& ObjRec64::GetFavAPropValue() const
{
	return mFavAPropValueTable;
}
//-----------------------------------------------------------------------------
const ConstPropValTable& ObjRec64::GetFavOPropValue() const
{
	return mFavOPropValueTable;
}
//-----------------------------------------------------------------------------
//virtual 
wxString ObjRec64::GetLockUser()const
{
	return mLockUser;
}
//-----------------------------------------------------------------------------
//virtual 
wxString ObjRec64::GetLockTime()const 
{
	return mLockTime;
}
//-----------------------------------------------------------------------------
//virtual 
wxString ObjRec64::GetLockSession()const
{
	return mLockSession;
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int	ModelBrowser::GetMode()const
{
	return mMode;
}
//-----------------------------------------------------------------------------
int64_t ModelBrowser::GetRootId()const
{
	return mRootId;
}
//-----------------------------------------------------------------------------
wxString ModelBrowser::GetSearchString()const
{
	return mSearchString;
}
//-----------------------------------------------------------------------------
bool ModelBrowser::GetGroupedByType()const
{
	return mGroupByType;
}
//-----------------------------------------------------------------------------
void ModelBrowser::SetMode(int mode)
{
	mMode = mode;
}
//-----------------------------------------------------------------------------
void ModelBrowser::SetRootId(int64_t id)
{
	mRootId = id;
}
//-----------------------------------------------------------------------------
void ModelBrowser::SetSearchString(const wxString& ss)
{
	mSearchString = ss;
}
//-----------------------------------------------------------------------------
void ModelBrowser::SetGroupedByType(bool group)
{
	mGroupByType = group;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ModelBrowser::ModelBrowser()
	: mClsPath (std::make_unique<ModelClsPath>())
	, mObjPath(std::make_unique<ModelObjPath>())
	, mMode(0)
	, mRootId(1)
	, mGroupByType(true)
{
}
//-----------------------------------------------------------------------------
ModelBrowser::~ModelBrowser()
{
	
}
//-----------------------------------------------------------------------------
void ModelBrowser::Load_ObjDir_ClsList()
{
	wxString root_obj_id = mObjPath->GetCurrent()->GetIdAsString();
	std::vector<const IIdent64*> toinsert;
	wxString query = wxString::Format(
		"SELECT cls.id, cls.title, cls.kind, cls.measure"
		"     , sum_qty, cls.pid, fav_prop_info"
		" FROM ( "
		"	SELECT cls_id AS cid, cls_kind AS ckind, SUM(qty) AS sum_qty "
		"	FROM obj_items obj "
		"	WHERE obj.pid = %s "
		"	GROUP BY cls_id, cls_kind )obj "
		" INNER JOIN cls_fav_info cls ON cls.id = cid AND cls.kind = ckind "
		" ORDER BY(substring(title, '^[0-9]{1,9}')::INT, title) ASC "
		, root_obj_id);
	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table)
	{
		unsigned int rowQty = table->GetRowCount();
		size_t row = 0;
		const ClsCache::fnModify fn = [&table, &row](const std::shared_ptr<ClsRec64>& cls)
		{
			//cls->SetId(table->GetAsString(0, row));
			table->GetAsString(1, row, cls->mTitle);
			ToClsKind(table->GetAsString(2, row), cls->mKind);
			table->GetAsString(3, row, cls->mMeasure);
			table->GetAsString(4, row, cls->mObjQty);
			cls->SetParentId(table->GetAsString(5, row));
			cls->mObjLoaded = false;

			cls->mFavObjProp.clear();
			//cls->mFavActProp.clear();
			//cls->mFavAPropValues.clear();
			//cls->mFavProp.clear();
			cls->ParseFavProp(table->GetAsString(6, row));
		};


		for (; row < rowQty; row++)
		{
			int64_t id;
			if (!table->GetAsString(0, row).ToLongLong(&id))
				throw;
			const std::shared_ptr<ClsRec64>& curr = mCache.mClsTable.GetById(id, fn);

			toinsert.emplace_back(curr.get());
		}

	}//if (table)
	UpdateUntitledActs();
	UpdateUntitledProperties();

	whDataMgr::GetDB().Commit();
	sigAfterRefreshCls(toinsert, mObjPath->GetCurrent().get(), mSearchString, mGroupByType, mMode);

}
//-----------------------------------------------------------------------------
void ModelBrowser::Load_ObjDir_ObjList()
{
	TEST_FUNC_TIME;
	mCache.mObjTable.Clear();

	const auto oid = mObjPath->GetCurrent()->GetIdAsString();

	wxString query = wxString::Format(
			"SELECT obj.id, obj.title, obj.qty, obj.pid"
			"		, acls.id, acls.title AS ctitle , acls.measure "
			"		, fav_prop_info"
			"		, lock_user, lock_time, lock_session "
			" FROM obj_fav_info obj "
			" LEFT JOIN lock_obj ON lock_obj.oid = obj.id  AND (CURRENT_TIMESTAMP-lock_time)<'10 min' "
			" INNER JOIN acls ON acls.id = obj.cls_id AND acls.kind = obj.cls_kind "
			" WHERE obj.id>0  AND obj.pid = %s "
			" ORDER BY(substring(acls.title, '^[0-9]{1,9}')::INT, acls.title) ASC "
			"  , (substring(obj.title, '^[0-9]{1,9}')::INT, obj.title) ASC "
			, oid );

	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table)
	{
		unsigned int rowQty = table->GetRowCount();
		size_t i = 0;

		const ClsCache::fnModify fn_cls = [this, &table, &i]
		(const std::shared_ptr<ClsRec64>& cls)
		{
			table->GetAsString(5, i, cls->mTitle);
			table->GetAsString(6, i, cls->mMeasure);
			cls->ParseFavProp(table->GetAsString(7, i));
			cls->mObjLoaded = true;
		};


		const ObjCache::fnModify fn = [this, &table, &i, fn_cls]
		(const std::shared_ptr<ObjRec64>& obj)
		{
			//obj->mClsId = parent_node->GetId();
			//obj->mCls = parent_node;
			obj->SetId		(	table->GetAsString(0, i));
			obj->mTitle =		table->GetAsString(1, i);
			obj->mQty =			table->GetAsString(2, i);
			obj->SetParentId(	table->GetAsString(3, i));

			obj->mLockUser = table->GetAsString(8, i);
			obj->mLockTime = table->GetAsString(9, i);
			obj->mLockSession = table->GetAsString(10, i);
			
			obj->ParseFavProp(	table->GetAsString(7, i));

			if(!obj->SetClsId(table->GetAsString(4, i)))
				throw;
			auto cls = mCache.mClsTable.GetById(obj->GetClsId(), fn_cls);
			
		};

		std::vector<const IIdent64*> toinsert;
		for (; i < rowQty; i++)
		{
			int64_t id, parentId;
			if (!table->GetAsString(0, i).ToLongLong(&id))
				throw;
			if (!table->GetAsString(3, i).ToLongLong(&parentId))
				throw;
			const std::shared_ptr<ObjRec64>& obj = mCache.mObjTable.GetObjById(id, parentId, fn);
			if (!obj)
				throw;

			//parent_node->AddObj(obj);
			toinsert.emplace_back(obj.get());
		}
		UpdateUntitledActs();
		UpdateUntitledProperties();

		sigAfterRefreshCls(toinsert, mObjPath->GetCurrent().get(), mSearchString, mGroupByType, mMode);

	}//if (table)
	whDataMgr::GetDB().Commit();
}
//-----------------------------------------------------------------------------
void ModelBrowser::Load_ObjDir_ObjList(int64_t cid)
{
	TEST_FUNC_TIME;
	
	const auto oid = mObjPath->GetCurrent()->GetIdAsString();
	wxLongLong cid_ll(cid);

	auto cls = mCache.mClsTable.GetById(cid);
	if (cls)
	{
		std::vector<const IIdent64*> todelete;

		if (cls->GetObjTable())
		{
			auto objTable = *cls->GetObjTable();
			for (const auto& obj : objTable)
				todelete.emplace_back(obj.get());
			sigObjOperation(Operation::BeforeDelete, todelete);
		}

		if (cls->mObjLoaded)
		{
			std::vector<const IIdent64*> toinsert;
			mCache.mObjTable.GetObjByClsId(cls->GetId(), toinsert);
			sigObjOperation(Operation::AfterInsert, toinsert);
			return;
		}
	}

	cls->ClearObjTable();

	wxString query = wxString::Format(
		"SELECT obj.id, obj.title, obj.qty, obj.pid"
		"		, acls.id, acls.title AS ctitle , acls.measure "
		"		, fav_prop_info"
		"		, lock_user, lock_time, lock_session "
		" FROM obj_fav_info obj "
		" LEFT JOIN lock_obj ON lock_obj.oid = obj.id  AND (CURRENT_TIMESTAMP-lock_time)<'10 min' "
		" INNER JOIN acls ON acls.id = obj.cls_id AND acls.kind = obj.cls_kind "
		" WHERE obj.id>0  AND obj.pid = %s AND acls.id = %s"
		" ORDER BY(substring(acls.title, '^[0-9]{1,9}')::INT, acls.title) ASC "
		"  , (substring(obj.title, '^[0-9]{1,9}')::INT, obj.title) ASC "
		, oid
		, cid_ll.ToString()	);

	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table)
	{
		unsigned int rowQty = table->GetRowCount();
		size_t i = 0;

		const ClsCache::fnModify fn_cls = [this, &table, &i]
		(const std::shared_ptr<ClsRec64>& cls)
		{
			table->GetAsString(5, i, cls->mTitle);
			table->GetAsString(6, i, cls->mMeasure);
			cls->ParseFavProp(table->GetAsString(7, i));
			cls->mObjLoaded = true;
		};

		const ObjCache::fnModify fn = [this, &table, &i, fn_cls]
		(const std::shared_ptr<ObjRec64>& obj)
		{
			//obj->SetId(table->GetAsString(0, i));
			obj->mTitle = table->GetAsString(1, i);
			obj->mQty = table->GetAsString(2, i);
			//obj->SetParentId(table->GetAsString(3, i));

			obj->mLockUser = table->GetAsString(8, i);
			obj->mLockTime = table->GetAsString(9, i);
			obj->mLockSession = table->GetAsString(10, i);

			obj->ParseFavProp(table->GetAsString(7, i));

			if (!obj->SetClsId(table->GetAsString(4, i)))
				throw;
			//auto cls = mCache.mClsTable.GetById(obj->GetClsId(), fn_cls);

		};

		std::vector<const IIdent64*> toinsert;
		for (; i < rowQty; i++)
		{
			int64_t id, parentId;
			if (!table->GetAsString(0, i).ToLongLong(&id))
				throw;
			if (!table->GetAsString(3, i).ToLongLong(&parentId))
				throw;
			const std::shared_ptr<ObjRec64>& obj = mCache.mObjTable.GetObjById(id, parentId, fn);
			if (!obj)
				throw;

			//parent_node->AddObj(obj);
			toinsert.emplace_back(obj.get());
		}
		UpdateUntitledActs();
		UpdateUntitledProperties();

		sigObjOperation(Operation::AfterInsert, toinsert);
		

	}//if (table)
	whDataMgr::GetDB().Commit();
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoRefreshObjects(int64_t cid)
{
	if (1 == mMode)
	{
		Load_ObjDir_ObjList(cid);
		return;
	}
	TEST_FUNC_TIME;

	auto cls = mCache.mClsTable.GetById(cid);

	if (!cls)
	{
		auto orig_cls = mClsPath->GetCurrent();
		const ClsCache::fnModify fn = [&orig_cls](const std::shared_ptr<ClsRec64>& cls)
		{
			//cls->SetId(1);
			cls->mTitle = orig_cls->GetTitle();
			cls->mKind = orig_cls->GetKind();
			cls->mMeasure = orig_cls->GetMeasure();
			cls->mObjQty = orig_cls->GetObjectsQty();
			cls->SetParentId(orig_cls->GetParentId());
			cls->mObjLoaded = false;
		};
		cls = mCache.mClsTable.GetById(cid, fn);

	}
		
	if (!cls)
		return;

	std::vector<const IIdent64*> todelete;
	
	if (cls->GetObjTable())
	{
		auto objTable = *cls->GetObjTable();
		for (const auto& obj : objTable)
		{
			todelete.emplace_back(obj.get());
		}
	}
	if (!todelete.empty())
	{
		if (mGroupByType)
			sigObjOperation(Operation::BeforeDelete, todelete);
	}
	std::shared_ptr<ClsRec64> parent_node = std::dynamic_pointer_cast<ClsRec64>(cls);

	if (parent_node->mObjLoaded)
	{
		std::vector<const IIdent64*> toinsert;
		if (mGroupByType)
			sigObjOperation(Operation::BeforeInsert, toinsert); 
		
		mCache.mObjTable.GetObjByClsId(cls->GetId(), toinsert);
		
		if (mGroupByType)
			sigObjOperation(Operation::AfterDelete, toinsert);
		else
			sigAfterRefreshCls(toinsert, cls.get(), mSearchString, mGroupByType, mMode);
		
		return;
	}
		
	
	//auto cls64 = std::dynamic_pointer_cast<ClsRec64>(cls);
	//cls64->RefreshObjects();
	
	cls->ClearObjTable();
	

	wxString query = wxString::Format(
		"SELECT obj.id, obj.title, obj.qty, obj.pid "
		"       ,get_path_objnum(obj.pid,1)  AS path"
		"       ,fav_prop_info"
		"		, lock_user, lock_time, lock_session "
		" FROM obj_fav_info obj "
		" LEFT JOIN lock_obj ON lock_obj.oid = obj.id  AND (CURRENT_TIMESTAMP-lock_time)<'10 min' "
		" WHERE obj.id>0 AND obj.cls_id = %s "
		" ORDER BY (substring(obj.title, '^[0-9]{1,9}')::INT, obj.title ) ASC "
		, cls->GetIdAsString()
	);
	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table)
	{
		unsigned int rowQty = table->GetRowCount();
		
		size_t i = 0;
		const ObjCache::fnModify fn = [/*&parent_node,*/ &table, &i,&cls]
		(const std::shared_ptr<ObjRec64>& obj)
		{
			//obj->mClsId = parent_node->GetId();
			//obj->mCls = parent_node;
			obj->SetId(table->GetAsString(0, i));
			table->GetAsString(1, i, obj->mTitle);
			table->GetAsString(2, i, obj->mQty);

			obj->SetParentId(table->GetAsString(3, i));

			obj->mPath = std::make_shared<ObjPath64>();
			table->GetAsString(4, i, obj->mPath->mStrPath );
			
			obj->SetClsId(cls->GetId());
			obj->ParseFavProp(table->GetAsString(5, i));

			obj->mLockUser = table->GetAsString(6, i);
			obj->mLockTime = table->GetAsString(7, i);
			obj->mLockSession = table->GetAsString(8, i);
		};

		std::vector<const IIdent64*> toinsert;
		for (; i < rowQty; i++)
		{
			int64_t id, parentId;
			if (!table->GetAsString(0, i).ToLongLong(&id))
				throw;
			if (!table->GetAsString(3, i).ToLongLong(&parentId))
				throw;
			const std::shared_ptr<ObjRec64>& obj = mCache.mObjTable.GetObjById(id, parentId, fn);
			if (!obj)
				throw;
			//parent_node->AddObj(obj);
			toinsert.emplace_back(obj.get());
		}
		UpdateUntitledActs();
		UpdateUntitledProperties();

		if (!toinsert.empty())
		{
			if (mGroupByType)
				sigObjOperation(Operation::AfterInsert, toinsert);
			else
				sigAfterRefreshCls(toinsert, cls.get(), mSearchString, mGroupByType, mMode);
		}
			

	}//if (table)
	whDataMgr::GetDB().Commit();
	parent_node->mObjLoaded = true;
}
//-----------------------------------------------------------------------------
void ModelBrowser::ParseSearch(const wxString& ss, std::vector<wxString>& words)
{
	if (!ss.empty())
	{
		size_t start = 0;
		while (wxString::npos != start)
		{
			size_t next = ss.find(" ", start);

			wxString word;

			if (wxString::npos != next)
			{
				word = ss.substr(start, next - start);
				next++;
			}
			else
				word = ss.substr(start, ss.size() - start);

			word.Replace("'", "''");

			words.emplace_back(word);
			start = next;
		}
	}
}
//-----------------------------------------------------------------------------
void ModelBrowser::LoadSearch()
{
	TEST_FUNC_TIME;

	std::vector<wxString>	search_words;
	ParseSearch(mSearchString, search_words);
	wxString search_sql;
	if (1 == search_words.size())
	{
		search_sql += wxString::Format(
			"(obj.title~~*'%%%s%%' OR cls.title~~*'%%%s%%')"
			, search_words[0], search_words[0]);
	}
	else
	{
		search_sql += wxString::Format(
			"   (cls.title~~*'%%%s%%' AND obj.title~~*'%%%s%%') "
			" OR(cls.title~~*'%%%s%%' AND obj.title~~*'%%%s%%') "
			, search_words[0], search_words[1]
			, search_words[1], search_words[0]);
	}

	std::vector<const IIdent64*> toinsert;
	mCache.Clear();

	wxString tree;
	whDataMgr::GetDB().BeginTransaction();
	if (0 == mMode)
	{
		if (mRootId != mClsPath->GetCurrent()->GetId())
			mClsPath->SetId(mRootId);
		const wxString parent_cid = mClsPath->GetCurrent()->GetIdAsString();
		tree = wxString::Format(
			" JOIN LATERAL get_path_cls_info(cls.id, 0)ctree ON ctree.id=%s"
			, parent_cid);
	}
	else if (1 == mMode)
	{
		if (mRootId != mObjPath->GetCurrent()->GetId())
			mObjPath->SetId(mRootId);
		const wxString parent_oid = mObjPath->GetCurrent()->GetIdAsString();
		tree = wxString::Format(
			" JOIN LATERAL get_path_objnum_info(obj.pid, 0)otree ON otree.oid=%s"
			, parent_oid);
		//"--join LATERAL get_path_objnum_info(obj.pid, 0)ipo ON ipo.opid = 0 AND 1::BIGINT = ANY(ipo.arr_id)"
	}

	wxString query = wxString::Format(
		"SELECT  cls.id AS cid, cls.pid AS parent_cid, cls.title AS ctitle, cls.kind, cls.measure"
		"	, sum(qty) OVER(PARTITION BY cls.id)  AS cqty"
		"	, obj.id AS oid, obj.pid AS parent_oid, obj.title AS otitle, qty"
		"	, get_path_objnum(obj.pid, 1)"
		//"--, get_path_cls(cls.id, 1)"
		"	, fav_prop_info"
		"	, lock_user, lock_time, lock_session "
		" FROM obj_fav_info obj"
		" LEFT JOIN lock_obj ON lock_obj.oid = obj.id  AND (CURRENT_TIMESTAMP-lock_time)<'10 min' "
		" INNER join acls cls  ON obj.cls_id = cls.id AND obj.cls_kind = cls.kind"
		" %s"
		" WHERE (%s)"
		" ORDER BY"
		"  (substring(cls.title, '^[0-9]{1,9}')::INT, cls.title) ASC"
		" ,(substring(obj.title, '^[0-9]{1,9}')::INT, obj.title) ASC"
		, tree, search_sql);
	
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table)
	{
		unsigned int rowQty = table->GetRowCount();
		size_t row = 0;
		const ClsCache::fnModify load_cls = [&table, &row](const std::shared_ptr<ClsRec64>& cls)
		{
			//cls->SetId(table->GetAsString(0, row));
			cls->SetParentId(table->GetAsString(1, row));
			table->GetAsString(2, row, cls->mTitle);
			ToClsKind(table->GetAsString(3, row), cls->mKind);
			table->GetAsString(4, row, cls->mMeasure);
			table->GetAsString(5, row, cls->mObjQty);
			cls->mObjLoaded = true;
		};

		const ObjCache::fnModify load_obj = [&table, &row](const std::shared_ptr<ObjRec64>& obj)
		{
			//obj->SetParentId(table->GetAsString(7, row));
			obj->SetClsId(table->GetAsString(0, row));
			
			table->GetAsString(8, row, obj->mTitle);
			table->GetAsString(9, row, obj->mQty);

			obj->mPath = std::make_shared<ObjPath64>();
			table->GetAsString(10, row, obj->mPath->mStrPath);

			obj->ParseFavProp(table->GetAsString(11, row));

			obj->mLockUser = table->GetAsString(12, row);
			obj->mLockTime = table->GetAsString(13, row);
			obj->mLockSession = table->GetAsString(14, row);
		};


		std::set<int64_t> loaded_cid;
		for (; row < rowQty; row++)
		{
			int64_t cid;
			if (!table->GetAsString(0, row).ToLongLong(&cid))
				throw;
			auto ins_it = loaded_cid.emplace(cid);
			if (ins_it.second)
			{
				const std::shared_ptr<ClsRec64>& curr = mCache.mClsTable.GetById(cid, load_cls);
				if (mGroupByType)
					toinsert.emplace_back(curr.get());
			}

			auto parent_oid_str = table->GetAsString(7, row);
			int64_t oid, parent_oid;
			if (   table->GetAsString(6, row).ToLongLong(&oid) 
				&& table->GetAsString(7, row).ToLongLong(&parent_oid))
			{
				const std::shared_ptr<ObjRec64>& obj = mCache.mObjTable.GetObjById(oid, parent_oid, load_obj);
				if (!obj)
					throw;
				if (!mGroupByType)
					toinsert.emplace_back(obj.get());
			}
							
		}
		UpdateUntitledActs();
		UpdateUntitledProperties();

	}//if (table)
	whDataMgr::GetDB().Commit();

	sigAfterRefreshCls(toinsert, nullptr, mSearchString, mGroupByType, mMode);
}
//-----------------------------------------------------------------------------
void ModelBrowser::Load_ClsDir_ClsList()
{
	wxString root_cls_id = mClsPath->GetCurrent()->GetIdAsString();

	std::vector<const IIdent64*> toinsert;
	//parent_node->ClearChilds();
	//insert

	wxString query = wxString::Format(
		"SELECT  id, title, kind, measure"
		", (SELECT COALESCE(SUM(qty), 0)"
		"   FROM obj WHERE obj.cls_id = cls.id GROUP BY cls_id)  AS qty"
		", pid "
		", fav_prop_info "
		" FROM cls_fav_info cls"
		" WHERE pid = %s"
		" ORDER BY (substring(title, '^[0-9]{1,9}')::INT, title ) ASC "
		, root_cls_id);


	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);

	if (table)
	{
		unsigned int rowQty = table->GetRowCount();
		size_t row = 0;
		const ClsCache::fnModify fn = [&table, &row](const std::shared_ptr<ClsRec64>& cls)
		{
			//cls->SetId(table->GetAsString(0, row));
			table->GetAsString(1, row, cls->mTitle);
			ToClsKind(table->GetAsString(2, row), cls->mKind);
			table->GetAsString(3, row, cls->mMeasure);
			table->GetAsString(4, row, cls->mObjQty);
			cls->SetParentId(table->GetAsString(5, row));
			cls->mObjLoaded = false;

			cls->mFavObjProp.clear();
			//cls->mFavActProp.clear();
			//cls->mFavAPropValues.clear();
			//cls->mFavProp.clear();
			cls->ParseFavProp(table->GetAsString(6, row));
		};


		for (; row < rowQty; row++)
		{
			int64_t id;
			if (!table->GetAsString(0, row).ToLongLong(&id))
				throw;
			const std::shared_ptr<ClsRec64>& curr = mCache.mClsTable.GetById(id, fn);

			toinsert.emplace_back(curr.get());
		}

	}//if (table)
	UpdateUntitledActs();
	UpdateUntitledProperties();

	whDataMgr::GetDB().Commit();
	sigAfterRefreshCls(toinsert, mClsPath->GetCurrent().get(), mSearchString, mGroupByType, mMode);
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoRefresh(bool sigBefore)
{
	TEST_FUNC_TIME;
	if(sigBefore)
	{
		std::vector<const IIdent64*> toinsert;
		sigBeforeRefreshCls(toinsert, nullptr, mSearchString, mGroupByType, mMode);
	}

	if (mSearchString.empty())
	{
		if (mMode == 0)
		{
			mClsPath->SetId(mRootId);
			const auto currRoot = mClsPath->GetCurrent();
			if (!mGroupByType && ClsKind::Abstract != currRoot->GetKind())
			{
				const auto cid = currRoot->GetId();
				DoRefreshObjects(cid);
			}
			else
				Load_ClsDir_ClsList();
		}
		else if (mMode == 1)
		{
			mObjPath->SetId(mRootId);
			
			if (mGroupByType)
				Load_ObjDir_ClsList(); 
			else
				Load_ObjDir_ObjList();
		}
	}
	else
		LoadSearch();

	if (mMode == 0)
		sigAfterPathChange(mClsPath->AsString());
	else if (mMode == 1)
		sigAfterPathChange(mObjPath->AsString());
	
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoActivate(int64_t id)
{
	mRootId = id;
	mCache.ClearSelection();
	DoRefresh();
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoUp()
{
	if (mSearchString.empty())
	{
		if(0==mMode)
			mRootId = mClsPath->GetCurrent()->GetParentId();
		else if(1 == mMode)
			mRootId = mObjPath->GetCurrent()->GetParentId();
		SetSearchString(wxEmptyString);
		mCache.ClearSelection();
		DoRefresh();
	}
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoFind(const wxString& str)
{
	if (!mGroupByType && mSearchString!= str)
	{
		const auto cid = mClsPath->GetCurrent()->GetId();
		auto cls = mCache.mClsTable.GetById(cid);
		if (cls)
			cls->mObjLoaded = false;
	}
	SetSearchString(str);
	mCache.ClearSelection();
	DoRefresh();
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoGroupByType(bool enable_group_by_type)
{
	SetGroupedByType(enable_group_by_type);
	mCache.ClearSelection();
	DoRefresh();
}
//-----------------------------------------------------------------------------
void wh::ModelBrowser::DoToggleGroupByType()
{
	mGroupByType = !mGroupByType;
	mCache.ClearSelection();
	DoRefresh();
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoSetMode(int mode)
{
	SetRootId(1);
	SetMode(mode);
	DoRefresh();
}
//-----------------------------------------------------------------------------
void ModelBrowser::Goto(int mode, int64_t id)
{
	std::vector<const IIdent64*> toinsert;
	sigBeforeRefreshCls(toinsert, nullptr, mSearchString, mGroupByType, mMode);
	mCache.Clear();
	SetMode(mode);
	SetRootId(id);
	SetSearchString(wxEmptyString);
	mCache.ClearSelection();
	DoRefresh(false);
}
//-----------------------------------------------------------------------------
void ModelBrowser::OnSmdSelectCls(int64_t cid, bool select)
{
	if (!mCache.mObjSelection.empty())
		return;

	auto& idxId = mCache.mClsSelection;
	auto it = idxId.find(cid);

	if (select)
		auto ins_it = idxId.emplace(cid);
	else
		idxId.erase(cid);
	
	/*
	if (idxId.end() == it)
	{
		auto ins_it = idxId.emplace(cid);
	}
	else
	{
		idxId.erase(it);
	}
	*/
}
//-----------------------------------------------------------------------------
void ModelBrowser::OnCmdSelectObj(int64_t oid, int64_t opid, bool select)
{
	if (!mCache.mClsSelection.empty())
		return;

	auto object = mCache.mObjTable.GetObjById(oid, opid);
	if (!object )//|| !object->GetLockUser().IsEmpty())
		return;

	auto& idxId = mCache.mObjSelection;
	auto obj_key = ObjectKey(oid, opid);

	if (select)
		auto ins_it = idxId.emplace(obj_key);
	else
		idxId.erase(obj_key);

}
//-----------------------------------------------------------------------------
void ModelBrowser::UpdateUntitledProperties()
{
	auto& prop_table = mCache.mPropTable;
	wxString str_id;
	for (const auto& it : prop_table.GetStorage() )
	{
		if (it->GetTitle().empty())// do not load if title already exists
			str_id += wxString::Format(" OR id=%s", it->GetIdAsString());
	}
	if (str_id.empty())
		return;
	str_id.Remove(0, 3);

	wxString query = wxString::Format(
		"SELECT id, title, kind, var, var_strict"
		" FROM prop WHERE %s"
		, str_id);

	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table)
	{
		unsigned int rowQty = table->GetRowCount();
		size_t row = 0;
		const PropCache::fnModify fn = [this, &table, &row]
		(const std::shared_ptr<IProp64>& iact)
		{
			auto prop = std::dynamic_pointer_cast<PropRec64>(iact);
			//act->SetId(table->GetAsString(0, row));
			prop->SetTitle(table->GetAsString(1, row));
			prop->SetKind(table->GetAsString(2, row));
			prop->SetVar(table->GetAsString(3, row));
			prop->SetVarStrict(table->GetAsString(4, row));
		};

		for (; row < rowQty; row++)
		{
			int64_t id;
			if (!table->GetAsString(0, row).ToLongLong(&id))
				throw;
			prop_table.InsertOrUpdate(id, fn);
		}//for
	}//if (table)
}
//-----------------------------------------------------------------------------
void ModelBrowser::ExecuteMoveObjects(const std::set<ObjectKey>& obj)const
{
	if (obj.empty())
		return;

	TEST_FUNC_TIME;
	try
	{
		auto ctrl = whDataMgr::GetInstance()->mContainer;
		auto presenter = ctrl->GetObject<MoveObjPresenter>("MoveObjPresenter");
		if (!presenter)
			return;
		presenter->SetMoveable(obj.begin()->mId, obj.begin()->mParentId);
		presenter->OnViewUpdate();
		presenter->ShowDialog();
	}
	catch (...)
	{
		// Transaction already rollbacked, dialog was destroyed, so nothinh to do
		wxLogError("Объект занят другим пользователем (см.подробности)");
	}
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoMove()
{
	if (!mCache.mClsSelection.empty())
		return;

	if (mCache.mObjSelection.empty())
		sigSelectCurrent(true);

	if (!mCache.mClsSelection.empty())
		return;

	ExecuteMoveObjects(mCache.mObjSelection);
	if (1 == mCache.mObjSelection.size())
		mCache.ClearSelection();
	DoRefresh();
}
//-----------------------------------------------------------------------------
void ModelBrowser::ExecuteActObjects(const std::set<ObjectKey>& obj)const
{
	if (obj.empty())
		return;

	TEST_FUNC_TIME;
	rec::PathItem data;
	data.mObj.mId = obj.begin()->mId;
	data.mObj.mParent.mId = obj.begin()->mParentId;

	using namespace dlg_act;
	namespace view = dlg_act::view;
	try
	{
		auto subj = std::make_shared<model::Obj >();
		subj->SetData(data, true);

		view::Frame dlg;
		dlg.SetModel(subj);
		dlg.ShowModal();
	}
	catch (...)
	{
		// Transaction already rollbacked, dialog was destroyed, so nothinh to do
		wxLogError("Объект занят другим пользователем (см.подробности)");
	}
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoAct()
{
	if (!mCache.mClsSelection.empty())
		return;

	if (mCache.mObjSelection.empty())
		sigSelectCurrent(true);

	if (!mCache.mClsSelection.empty())
		return;

	ExecuteActObjects(mCache.mObjSelection);
	if (1==mCache.mObjSelection.size())
		mCache.ClearSelection();
	DoRefresh();
}
//-----------------------------------------------------------------------------
void ModelBrowser::UpdateUntitledActs()
{
	auto& act_table = mCache.mActTable;

	wxString str_id;
	for (const auto& it : act_table.GetStorage() )
	{
		if(it->GetTitle().empty())// do not load if title already exists
			str_id += wxString::Format(" OR id=%s", it->GetIdAsString());
	}
	if (str_id.empty())
		return;
	str_id.Remove(0, 3);

	wxString query = wxString::Format(
		"SELECT id, title, note, color"
		" FROM act WHERE %s"
		, str_id);

	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table)
	{
		unsigned int rowQty = table->GetRowCount();
		size_t row = 0;
		const ActCache::fnModify fn = [this, &table, &row]
		(const std::shared_ptr<IAct64>& iact)
		{
			auto act = std::dynamic_pointer_cast<ActRec64>(iact);
			//act->SetId(table->GetAsString(0, row));
			act->SetTitle(table->GetAsString(1, row));
			act->SetColour(table->GetAsString(2, row));
		};


		for (; row < rowQty; row++)
		{
			int64_t id;
			if (!table->GetAsString(0, row).ToLongLong(&id))
				throw;

			act_table.InsertOrUpdate(id, fn);
		}//for
	}//if (table)

}
//-----------------------------------------------------------------------------
const wxString ModelBrowser::GetRootTitle()const
{
	switch (mMode)
	{
	case 0: return mClsPath->GetCurrent()->GetTitle();;
	case 1: 
		return wxString::Format("[%s]%s"
			, mObjPath->GetCurrent()->GetCls()->GetTitle()
			, mObjPath->GetCurrent()->GetTitle() );
		
	default:break;
	}
	return wxEmptyString;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ModelPageBrowser::ModelPageBrowser()
{
	connModelBrowser_AfterPathChange = mModelBrowser.sigAfterPathChange
		.connect(std::bind(&ModelPageBrowser::UpdateTitle, this));

}
//-----------------------------------------------------------------------------
ModelPageBrowser::ModelPageBrowser(int mode, int64_t rood_id, bool group, const wxString& ss)
	: ModelPageBrowser()
{
	mModelBrowser.SetMode(mode);
	mModelBrowser.SetRootId(rood_id);
	mModelBrowser.SetSearchString(ss);
	mModelBrowser.SetGroupedByType(group);
}

//-----------------------------------------------------------------------------
void ModelPageBrowser::DoEnableGroupByType(bool group_by_type)
{
	mModelBrowser.DoGroupByType(group_by_type);
}
//---------------------------------------------------------------------------
//virtual 
void ModelPageBrowser::UpdateTitle() //override;
{
	auto mgr = ResMgr::GetInstance();
	const wxIcon* ico = &wxNullIcon;
	if (0 == mModelBrowser.GetMode())
	{
		ico = &mgr->m_ico_folder_type24;
	}
	else if (1 == mModelBrowser.GetMode())
	{
		ico = &mgr->m_ico_folder_obj24;
	}

	wxString title; 
	auto root_id = mModelBrowser.GetRootId();
	if ( 1 >= root_id)
		title = "/";
	else
		title = ".."+mModelBrowser.GetRootTitle();

	auto ss = mModelBrowser.GetSearchString();
	if (!ss.empty())
		title = wxString::Format("поиск:'%s' в %s ", ss, title);
		

	sigUpdateTitle(title, *ico);
}
//---------------------------------------------------------------------------
//virtual 
void ModelPageBrowser::Show()//override;
{
	mModelBrowser.DoRefresh();
	sigShow();

}
//---------------------------------------------------------------------------
//virtual 
void ModelPageBrowser::Load(const boost::property_tree::wptree& page_val)//override;
{
	int mode = page_val.get<int>(L"CtrlPageBrowser.Mode", 0);
	int64_t root_id = page_val.get<int64_t>(L"CtrlPageBrowser.RootId", 1);
	std::wstring search_string = page_val.get<std::wstring>(L"CtrlPageBrowser.SearchString");
	bool group_type = page_val.get<bool>(L"CtrlPageBrowser.GroupByType", true);


	mModelBrowser.SetMode(mode);
	mModelBrowser.SetSearchString(search_string);
	mModelBrowser.SetRootId(root_id);
	mModelBrowser.SetGroupedByType(group_type);
	
	mModelBrowser.DoRefresh();

}
//---------------------------------------------------------------------------
//virtual 
void ModelPageBrowser::Save(boost::property_tree::wptree& page_val)//override;
{
	int mode = mModelBrowser.GetMode();
	int64_t root_id = mModelBrowser.GetRootId();
	wxString search_string = mModelBrowser.GetSearchString();
	bool group_type = mModelBrowser.GetGroupedByType();

	using ptree = boost::property_tree::wptree;
	ptree content;
	content.put(L"Mode", mode);
	content.put(L"RootId", root_id);
	content.put(L"SearchString", std::wstring(search_string.wc_str()) );
	content.put(L"GroupByType", group_type);
	page_val.push_back(std::make_pair(L"CtrlPageBrowser", content));

}
