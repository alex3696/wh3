#include "_pch.h"
#include "ModelBrowser.h"

using namespace wh;

//-----------------------------------------------------------------------------
void ActCache::LoadDetailById()
{
	if (this->mData.empty())
		return;
	//Clear();

	wxString str_aid;
	for (const auto& it : mData)
	{
		if(it->GetTitle().empty())// do not load if title already exists
			str_aid += wxString::Format(" OR id=%s", it->GetIdAsString());
	}
	if (str_aid.empty())
		return;

	str_aid.Remove(0, 3);

	wxString query = wxString::Format(
		"SELECT id, title, note, color"
		" FROM act WHERE %s"
		, str_aid);

	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table)
	{
		unsigned int rowQty = table->GetRowCount();
		size_t row = 0;
		const fnModify fn = [this, &table, &row](const std::shared_ptr<RowType>& iact)
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

			InsertOrUpdate(id, fn);
		}//for
	}//if (table)

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


const std::shared_ptr<ClsRec64>  ClsCache::NullValue = std::shared_ptr<ClsRec64>(nullptr);
const std::shared_ptr<ObjRec64>  ObjCache::mNullObj = std::shared_ptr<ObjRec64>(nullptr);



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
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
	return mTable->GetById(mParentId);
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
				cache->mClsTable.GetById(GetCls()->GetId(), upd_cls);

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
	

}
//-----------------------------------------------------------------------------
//virtual 
SpClsConst ObjRec64::GetCls()const //override 
{ 
	return mTable->GetCache()->mClsTable.GetById(mClsId);
}
//-----------------------------------------------------------------------------
//virtual 
std::shared_ptr<const IObj64> ObjRec64::GetParent()const //override
{
	return mTable->GetObjById(mId,mParentId);
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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ClsTree::ClsTree()
	:mClsCache(std::make_unique<ClsCache>(nullptr))
{
	
	const ClsCache::fnModify fn = [](const std::shared_ptr<ClsRec64>& cls)
	{
		//cls->SetId(1);
		cls->mTitle = "ClsRoot";
		cls->mKind = ClsKind::Abstract;
		cls->mMeasure.Clear();
		cls->mObjQty.Clear();
		cls->SetParentId(0);
	};
	mRoot = mClsCache->GetById(1, fn);
	mCurrent = mRoot;
}

//-----------------------------------------------------------------------------
void ClsTree::Up()
{
	if (mCurrent != mRoot)
	{
		auto id = mCurrent->GetParentId();
		SetId(id);
	}
}
//-----------------------------------------------------------------------------
void ClsTree::Refresh()
{
	TEST_FUNC_TIME;

	auto id = mCurrent->GetIdAsString();
	//mCurrent->ClearChilds();
		
	mCurrent = mRoot;
	//mRoot->ClearChilds();

	wxString query = wxString::Format(
		"SELECT id, title,kind,pid, measure"
		" FROM public.get_path_cls_info(%s, 1)"
		, id);

	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table)
	{
		size_t row = 0;
		const ClsCache::fnModify fn = [this, &table, &row](const std::shared_ptr<ClsRec64>& cls)
		{
			//cls->SetId(table->GetAsString(0, row));
			table->GetAsString(1, row, cls->mTitle);
			ToClsKind(table->GetAsString(2, row), cls->mKind);
			cls->SetParentId(table->GetAsString(3, row));
			table->GetAsString(4, row, cls->mMeasure);
			cls->mObjQty.Clear();
		};

		unsigned int rowQty = table->GetRowCount();
		for (size_t i = rowQty; i > 0 ; --i)
		{
			row = i - 1;
			int64_t id;
			if (!table->GetAsString(0, row).ToLongLong(&id))
				throw;
			const std::shared_ptr<ClsRec64>& value = mClsCache->GetById(id, fn);
			mCurrent = value;


		}
	}//if (table)

	whDataMgr::GetDB().Commit();

	

}

//-----------------------------------------------------------------------------

void ClsTree::SetId(const wxString& str)
{
	if (!str.IsEmpty())
	{
		int64_t tmp;
		if (str.ToLongLong(&tmp))
		{
			SetId(tmp);
			return;
		}
	}
	SetId(1);
}
//-----------------------------------------------------------------------------
void ClsTree::SetId(const int64_t& val)
{
	const ClsCache::fnModify fn = [&val](const std::shared_ptr<ClsRec64>& cls)
	{
		//cls->SetId(val);
	};
	const std::shared_ptr<ClsRec64>& new_curr = mClsCache->GetById(val, fn);
	
	//auto new_curr = ClsRec64::MakeShared();
	//new_curr->SetId(val);

	sigBeforePathChange(*mCurrent);
	mCurrent = new_curr;
	Refresh();
	sigAfterPathChange(*mCurrent);
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
	return mClsPath->GetCurrent()->GetId();
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
	:mClsPath (std::make_unique<ClsTree>())
{

	//sigClear();
	
	mClsPath->sigBeforePathChange.connect(sigBeforePathChange);
	mClsPath->sigAfterPathChange.connect(sigAfterPathChange);
	
	//mClsPath->sigBeforePathChange.connect([this](const ICls64& parent)
		//{
			//std::vector<const IIdent64*> toinsert;
			//sigBeforeRefreshCls(toinsert, &parent); // internal in DoRefresh
		//});
	
	/*
	mClsPath->sigAfterPathChange.connect([this](const ICls64&)
		{
			DoRefresh();
			//sigAfterRefreshCls(toinsert, parent_node.get()); // internal in DoRefresh
		});
	*/
}
//-----------------------------------------------------------------------------
ModelBrowser::~ModelBrowser()
{
	
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoRefreshObjects(int64_t cid)
{
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
		else
			sigBeforeRefreshCls(todelete, cls.get(), mSearchString, mGroupByType);
	}
	std::shared_ptr<ClsRec64> parent_node = std::dynamic_pointer_cast<ClsRec64>(cls);

	if (parent_node->mObjLoaded)
	{
		std::vector<const IIdent64*> toinsert;
		if (mGroupByType)
			sigObjOperation(Operation::BeforeInsert, toinsert); 
		else
			sigBeforeRefreshCls(toinsert, cls.get(), mSearchString, mGroupByType);
		
		mCache.mObjTable.GetObjByClsId(cls->GetId(), toinsert);
		
		if (mGroupByType)
			sigObjOperation(Operation::AfterDelete, toinsert);
		else
			sigAfterRefreshCls(toinsert, cls.get(), mSearchString, mGroupByType);
		
		return;
	}
		
	
	//auto cls64 = std::dynamic_pointer_cast<ClsRec64>(cls);
	//cls64->RefreshObjects();
	
	cls->ClearObjTable();
	

	wxString query = wxString::Format(
		"SELECT obj.id, obj.title, obj.qty, obj.pid "
		"       ,get_path_objnum(obj.pid,1)  AS path"
		"       ,fav_prop_info"
		" FROM obj_current_info obj "
		" WHERE obj.id>0 AND obj.cls_id = %s "
		" ORDER BY (substring(obj.title, '^[0-9]{1,9}')::INT, obj.title ) ASC "
		, cls->GetIdAsString()
	);
	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table)
	{
		unsigned int rowQty = table->GetRowCount();
		
		auto& cache = this->mCache;

		size_t i = 0;
		const ObjCache::fnModify fn = [&parent_node, &table, &i,&cls,&cache]
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
		cache.mActTable.LoadDetailById();

		if (!toinsert.empty())
		{
			if (mGroupByType)
				sigObjOperation(Operation::AfterInsert, toinsert);
			else
				sigAfterRefreshCls(toinsert, cls.get(), mSearchString, mGroupByType);
		}
			

	}//if (table)
	whDataMgr::GetDB().Commit();
	parent_node->mObjLoaded = true;
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoRefreshFindInClsTree()
{
	TEST_FUNC_TIME;

	std::vector<wxString>	search_words;

	if (!mSearchString.empty())
	{
		size_t start = 0;
		while (wxString::npos != start)
		{
			size_t next = mSearchString.find(" ", start);

			wxString word;

			if (wxString::npos != next)
			{
				word = mSearchString.substr(start, next - start);
				next++;
			}
			else
				word = mSearchString.substr(start, mSearchString.size() - start);

			word.Replace("'", "''");

			search_words.emplace_back(word);
			start = next;
		}
	}
	
	std::shared_ptr<const ICls64> parent_node = mClsPath->GetCurrent();
	std::vector<const IIdent64*> toinsert;
	sigBeforeRefreshCls(toinsert, parent_node.get(), mSearchString, mGroupByType);
	mCache.Clear();

	whDataMgr::GetDB().BeginTransaction();
	if( mRootId!= mClsPath->GetCurrent()->GetId() )
		mClsPath->SetId(mRootId);

	parent_node = mClsPath->GetCurrent();
	auto parent_id = parent_node->GetIdAsString();

	wxString search;
	if (1 == search_words.size())
	{
		search += wxString::Format(
			"(obj_name.title~~*'%%%s%%' OR cls._title~~*'%%%s%%')"
			, search_words[0], search_words[0]);
	}
	else
	{
		search += wxString::Format(
		"   (cls._title~~*'%%%s%%' AND obj_name.title~~*'%%%s%%') "
		" OR(cls._title~~*'%%%s%%' AND obj_name.title~~*'%%%s%%') "
			, search_words[0], search_words[1]
			, search_words[1], search_words[0] );
	}
	wxString query = wxString::Format ( 
		"SELECT cid, parent_cid, ctitle, kind, measure "
		" , sum(qty) OVER(PARTITION BY cid ORDER BY cid DESC)  AS allqty "
		" , oid, parent_oid, otitle, qty "
		" , get_path_objnum(parent_oid, 1) AS path "
		" , (SELECT fav_prop_info FROM obj_current_info "
		"	  WHERE obj_current_info.cls_id = co.cid "
		"       AND obj_current_info.id = co.oid "
		" 		LIMIT 1 " //--obj_current_info.pid = co.parent_oid
	    "    ) AS fp "
		" FROM "
		" ( "
		" SELECT cls._id AS cid, cls._title AS ctitle, cls._pid AS parent_cid, cls._kind AS kind, cls._measure AS measure "
		"     , obj_name.id  AS oid, obj_name.title  AS otitle "
		"     , (CASE obj_name.cls_kind WHEN 1 THEN obj_num.pid WHEN 2 THEN obj_qtyi.pid ELSE obj_qtyf.pid END) AS parent_oid "
		"     , (CASE obj_name.cls_kind WHEN 1 THEN 1 WHEN 2 THEN obj_qtyi.qty ELSE obj_qtyf.qty END) AS qty "
		"   FROM obj_name "
		"   LEFT JOIN obj_num  USING(id) "
		"   LEFT JOIN obj_qtyi USING(id) "
		"   LEFT JOIN obj_qtyf USING(id) "
		"   RIGHT JOIN get_childs_cls(%s)cls ON cls._id = obj_name.cls_id "
		"   WHERE (%s) "
		" )co "
		" ORDER BY "
		"  (substring(co.ctitle, '^[0-9]{1,9}')::INT, co.ctitle) ASC "
		" ,(substring(co.otitle, '^[0-9]{1,9}')::INT, co.otitle) ASC "

		, parent_id, search);

	
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

		auto& cache = this->mCache;
		const ObjCache::fnModify load_obj = [&table, &row, &cache](const std::shared_ptr<ObjRec64>& obj)
		{
			//obj->SetParentId(table->GetAsString(7, row));
			obj->SetClsId(table->GetAsString(0, row));
			
			table->GetAsString(8, row, obj->mTitle);
			table->GetAsString(9, row, obj->mQty);

			obj->mPath = std::make_shared<ObjPath64>();
			table->GetAsString(10, row, obj->mPath->mStrPath);

			obj->ParseFavProp(table->GetAsString(11, row));
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
		cache.mActTable.LoadDetailById();

	}//if (table)
	whDataMgr::GetDB().Commit();

	if (!toinsert.empty())
		sigAfterRefreshCls(toinsert, nullptr, mSearchString, mGroupByType);
		//sigAfterRefreshCls(toinsert, parent_node.get(), mSearchString);


}
//-----------------------------------------------------------------------------
void ModelBrowser::DoRefresh()
{
	TEST_FUNC_TIME;

	if (!mSearchString.empty())
	{
		DoRefreshFindInClsTree();
		return;
	}

	
	mClsPath->SetId(mRootId);
	if (ClsKind::Abstract != mClsPath->GetCurrent()->GetKind())
	{
		if (mGroupByType)
		{
			auto parent = mClsPath->GetCurrent()->GetParent();
			SetRootId(parent->GetId());
			mClsPath->SetId(mRootId);
			//DoRefresh();
			//DoRefreshObjects(mClsPath->GetCurrent());
			//return;
		}
		else
		{
			std::shared_ptr<const ICls64> parent_node = mClsPath->GetCurrent();
			DoRefreshObjects(parent_node->GetId());
			return;
		}
	}

	
	std::shared_ptr<const ICls64> parent_node = mClsPath->GetCurrent();
	wxString parent_id = parent_node->GetIdAsString();



	
	std::vector<const IIdent64*> toinsert;
	sigBeforeRefreshCls(toinsert, parent_node.get(), mSearchString, mGroupByType);
	
	
	//parent_node->ClearChilds();
	//insert

	wxString query = wxString::Format(
		"SELECT  id, title, kind, measure"
		", (SELECT COALESCE(SUM(qty), 0)"
		"   FROM obj WHERE obj.cls_id = cls.id GROUP BY cls_id)  AS qty"
		", pid "
		", fav_prop_info "
		" FROM cls"
		" WHERE pid = %s"
		" ORDER BY (substring(title, '^[0-9]{1,9}')::INT, title ) ASC "
		, parent_id
		);
	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);

	if (table)
	{
		unsigned int rowQty = table->GetRowCount();
		size_t row = 0;
		const ClsCache::fnModify fn = [&parent_node, &table, &row](const std::shared_ptr<ClsRec64>& cls)
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


	whDataMgr::GetDB().Commit();
	
	if (!toinsert.empty())
		sigAfterRefreshCls(toinsert, parent_node.get(), mSearchString, mGroupByType);
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoActivate(int64_t id)
{
	SetRootId(id);
	DoRefresh();
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoUp()
{
	if (mSearchString.empty())
	{
		auto current = mClsPath->GetCurrent();
		if (mGroupByType)
		{
			int64_t parent_id = mClsPath->GetCurrent()->GetParentId();
			SetRootId(parent_id);
		}
		else
		{
			auto parent = current->GetParent();
			if (parent)
			{
				SetRootId(parent->GetId());
			}
			else
			{
				SetRootId(current->GetId());
			}

		}
		DoRefresh();

	}
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoFind(const wxString& str)
{
	SetSearchString(str);
	DoRefresh();
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoGroupByType(bool enable_group_by_type)
{
	SetGroupedByType(enable_group_by_type);
	DoRefresh();
}
//-----------------------------------------------------------------------------
void wh::ModelBrowser::DoToggleGroupByType()
{
	mGroupByType = !mGroupByType;
	DoRefresh();
}
//-----------------------------------------------------------------------------
const ICls64& ModelBrowser::GetRootCls() const
{
	return *mClsPath->GetCurrent();
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
void ModelPageBrowser::DoEnableGroupByType(bool group_by_type)
{
	mModelBrowser.DoGroupByType(group_by_type);
}
//---------------------------------------------------------------------------
//virtual 
void ModelPageBrowser::UpdateTitle() //override;
{
	const wxIcon& ico = ResMgr::GetInstance()->m_ico_folder_type24;

	const ICls64& node = mModelBrowser.GetRootCls();
	wxString title;
	if (node.GetParentId() > 0)
		title = "..";
	if (node.GetId() == 1)
		title = "/";
	else
		title += node.GetTitle();

	auto ss = mModelBrowser.GetSearchString();
	if (!ss.empty())
		title = wxString::Format("поиск:'%s' в %s ", ss, title);
		

	sigUpdateTitle(title, ico);
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
