#include "_pch.h"
#include "ModelBrowser.h"

using namespace wh;
//-----------------------------------------------------------------------------
void ActCache::LoadDetailById(const std::set<int64_t>& aid_vector)
{
	if (aid_vector.empty())
		return;
	//Clear();

	wxString str_aid;
	for (const int64_t& aid : aid_vector)
	{
		wxLongLong ll(aid);
		str_aid += wxString::Format(" OR id=%s", ll.ToString() );
	}
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

			UpdateOrInsert(id, fn);
		}//for
	}//if (table)

}
//-----------------------------------------------------------------------------
void ActCache::LoadDetailById()
{
	if (this->mData.empty())
		return;
	//Clear();

	wxString str_aid;
	for (const auto& it : mData)
	{
		str_aid += wxString::Format(" OR id=%s", it->GetIdAsString());
	}
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

			UpdateOrInsert(id, fn);
		}//for
	}//if (table)

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void ViewCidAidPeriod::LoadByParentCid(const wxString& parent_id)
{
	Clear();

	wxString query = wxString::Format(
		"WITH "
		"curr AS(SELECT %s::BIGINT AS pid) "
		", parent_tree(id) AS( "
		"	SELECT id FROM get_path_cls_info((SELECT pid FROM curr), 1) "
		") "
		", parent_fav(aid, visible) AS( "
		"	SELECT aid, visible FROM parent_tree cls "
		"	INNER JOIN fav_act ON fav_act.cid = cls.id AND fav_act.usr = CURRENT_USER "
		") "
		", current_list(cid) AS( "
		"	SELECT id FROM acls WHERE acls.pid = (SELECT pid FROM curr) "
		") "
		", all_fav(cid, aid, visible) AS( "
		"	SELECT  current_list.cid, parent_fav.aid, parent_fav.visible "
		"	FROM current_list "
		"	RIGHT JOIN parent_fav ON TRUE "
		"	UNION ALL "
		"	SELECT  current_list.cid, fav_act.aid, fav_act.visible "
		"	FROM current_list "
		"	INNER JOIN  fav_act ON fav_act.cid = current_list.cid AND fav_act.usr = CURRENT_USER "
		") "
		", all_fav_bitor(cid, aid, visible) AS( "
		"	SELECT cid, aid, bit_or(visible) "
		"	FROM all_fav "
		"	GROUP BY cid, aid "
		") "
		"SELECT * FROM all_fav_bitor "
		"LEFT JOIN  LATERAL(SELECT ref_cls_act.period "
		"	FROM(SELECT * FROM parent_tree UNION SELECT all_fav_bitor.cid) ct "
		"	INNER JOIN ref_cls_act ON ref_cls_act.period IS NOT NULL "
		"	AND ref_cls_act.cls_id = ct.id "
		"	AND ref_cls_act.act_id = all_fav_bitor.aid "
		")ref_ca ON TRUE "
		,parent_id);
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table)
	{
		unsigned int rowQty = table->GetRowCount();
		size_t n = 0;
		for (; n < rowQty; n++)
		{
			RowType row;
			table->GetAsString(3, n, row.mPeriod);
			if (!table->GetAsString(2, n).ToLong(&row.mVisible))
				row.mVisible = 0;
			if (!table->GetAsString(1, n).ToLongLong(&row.mAid))
				row.mAid = 0;
			if (!table->GetAsString(0, n).ToLongLong(&row.mCid))
				throw;
			mData.emplace(row);
		}//for
	}//if (table)

}








const std::shared_ptr<ClsRec64>  ClsCache::NullValue = std::shared_ptr<ClsRec64>(nullptr);
const std::shared_ptr<ObjRec64>  ObjCache::mNullObj = std::shared_ptr<ObjRec64>(nullptr);

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
bool ClsRec64::GetFavActs(std::vector<const IAct64*>& acts)const //override;
{
	for (const auto& act : mFavActLog)
		acts.emplace_back(act.mAct.get());
	return true;
}
//-----------------------------------------------------------------------------
//virtual 
bool ClsRec64::GetActVisible(int64_t aid, char& visible)const //override;
{
	const auto& idxAid = mFavActLog.get<0>();
	const auto it = idxAid.find(aid);
	if (it != idxAid.cend())
	{
		visible = it->mVisible;
		return true;
	}
	return false;
}
//-----------------------------------------------------------------------------
//virtual 
bool ClsRec64::GetActPeriod(int64_t aid, wxString& period)const //override;
{
	const auto& idxAid = mFavActLog.get<0>();
	const auto it = idxAid.find(aid);
	if (it != idxAid.cend())
	{
		period = it->mPeriod;
		return true;
	}
	return false;
}












//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void ObjRec64::ParseActInfo(const wxString& act_info_json)
{
	if (act_info_json.empty())
		return;

	std::wstringstream ss(act_info_json.ToStdWstring());
	mActInfo.clear();
	boost::property_tree::read_json(ss, mActInfo);

	auto cache = this->mTable->GetCache();


	auto begin = GetActInfo().begin();
	auto end = GetActInfo().end();
	while (begin != end)
	{
		auto act_col_begin = begin->second.begin();
		auto act_col_end = begin->second.end();
		int visible = 0;
		wxString period;
		std::wstring str_aid = begin->first.data();
		int64_t aid = std::stoll(str_aid);

		while (act_col_begin != act_col_end)
		{
			std::wstring str_ainfo = act_col_begin->first.data();
			int cur_visible = stoi(str_ainfo);
			visible |= cur_visible;
			if (cur_visible == 2)
			{
				period = act_col_begin->second.get_value<std::wstring>();

			}//if (str_aid.ToLongLong(&aid))
			++act_col_begin;
		}
		const ClsCache::fnModify upd_cls = [&visible, &period, &cache, &aid](const std::shared_ptr<ClsRec64>& cls)
		{
			const ActCache::fnModify fn_act_upsert = [](const std::shared_ptr<IAct64>& act)
			{};

			ClsRec64::FavAct fa;
			fa.mAct = cache->mActTable.UpdateOrInsert(aid, fn_act_upsert);
			fa.mPeriod = period;
			fa.mVisible = visible;
			cls->mFavActLog.emplace(fa);
		};
		cache->mClsTable.GetById(GetCls()->GetId(), upd_cls);

		++begin;
	}//while (begin != end)
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
bool ObjRec64::GetActPrevios(int64_t aid, wxDateTime& dt)const //override
{
	wxLongLong ll(aid);
	std::wstring ss = ll.ToString() << L"." << (int)1;
	std::wstring act_val = GetActInfo().get<std::wstring>(ss, L"");

	if (dt.ParseISOCombined(act_val) && dt.IsValid())
		return true;

	return false;
}
//-----------------------------------------------------------------------------
//virtual 
bool ObjRec64::GetActNext(int64_t aid, wxDateTime& next)const //override
{
	wxLongLong ll(aid);
	std::wstring ss = ll.ToString() << L"." << (int)4;
	std::wstring act_val = GetActInfo().get<std::wstring>(ss, L"");

	if (next.ParseISOCombined(act_val) && next.IsValid())
		return true;

	return false;
}
//-----------------------------------------------------------------------------
//virtual 
bool ObjRec64::GetActLeft(int64_t aid, double& left)const //override
{
	wxLongLong ll(aid);
	std::wstring ss = ll.ToString() << L"." << (int)8;
	std::wstring act_val = GetActInfo().get<std::wstring>(ss, L"");

	if(wxString(act_val).ToCDouble(&left))
		return true;

	return false;
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
		"SELECT o.id, o.title, o.qty, o.pid "
		"       ,get_path_objnum(o.pid,1)  AS path"
		"       ,prop"
		"		,(WITH  cls_tree(id) AS( "
		"			SELECT id FROM get_path_cls_info(o.cls_id, 1) "
		"			) "
		"			SELECT jsonb_object_agg(all_fav_bitor.aid  "
		"				, CASE WHEN((visible & 1)>0) THEN jsonb_build_object('1', previos) ELSE '{}'::jsonb END "
		"				|| CASE WHEN(period IS NOT NULL) THEN "
		"					CASE WHEN((visible & 2)>0) THEN jsonb_build_object('2', EXTRACT(EPOCH FROM period)) ELSE '{}' END "
		"					|| CASE WHEN((visible & 4)>0) THEN jsonb_build_object('4', previos + period) ELSE '{}' END "
		"					|| CASE WHEN((visible & 8)>0) THEN jsonb_build_object('8', ROUND((EXTRACT(EPOCH FROM(previos + period - now())) / 86400)::NUMERIC, 2))  ELSE '{}' END "
		"				ELSE '{}' END "
		"				) as last_act "
		"		FROM(SELECT aid, bit_or(visible) AS visible FROM cls_tree cls "
		"			INNER JOIN fav_act ON fav_act.cid = cls.id AND fav_act.usr = CURRENT_USER "
		"			GROUP BY aid)all_fav_bitor "
		"		LEFT JOIN  LATERAL(SELECT ref_cls_act.period "
		"			FROM cls_tree ct "
		"			INNER JOIN ref_cls_act ON ref_cls_act.period IS NOT NULL "
		"			AND ref_cls_act.cls_id = ct.id "
		"			AND ref_cls_act.act_id = all_fav_bitor.aid "
		"		)ref_ca ON TRUE "
		"		LEFT JOIN(SELECT MAX(timemark) AS previos, act_id AS aid, obj_id AS oid "
		"			FROM log_main GROUP BY obj_id, act_id) "
		"		last_log             ON  last_log.oid = o.id "
		"		AND last_log.aid = all_fav_bitor.aid "
		"		AND last_log.aid<>0 "
		"	) AS ainfo "
		" FROM obj o "
		" WHERE o.id>0 AND o.cls_id = %s "
		" ORDER BY (substring(o.title, '^[0-9]+')::INT, o.title ) ASC "
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

			wxString str;
			table->GetAsString(5, i, str);
			if (!str.empty())
			{
				std::wstringstream ss(str.ToStdWstring());
				obj->mProp.clear();
				boost::property_tree::read_json(ss, obj->mProp);
			}
			str.clear();
			table->GetAsString(6, i, str);

			obj->ParseActInfo(str);


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
			"(obj.title~~*'%%%s%%' OR cls._title~~*'%%%s%%')"
			, search_words[0], search_words[0]);
	}
	else
	{
		search += wxString::Format(
		"(cls._title~~*'%%%s%%' AND obj.title~~*'%%%s%%') "
		" OR(cls._title~~*'%%%s%%' AND obj.title~~*'%%%s%%') "
			, search_words[0], search_words[1]
			, search_words[1], search_words[0] );
	}

	wxString query = wxString::Format(
		"SELECT cls._id, cls._pid, cls._title, cls._kind, cls._measure" //, cls._note, cls._dobj "
		"      ,sum(qty) OVER(PARTITION BY cls._id ORDER BY cls._id DESC)  AS allqty "
		"      ,obj.id, obj.pid, obj.title, obj.qty "
		"      ,get_path_objnum(obj.pid,1)  AS path"
		"      ,obj.prop "
		"		,(WITH  cls_tree(id) AS( "
		"			SELECT id FROM get_path_cls_info(obj.cls_id, 1) "
		"			) "
		"			SELECT jsonb_object_agg(all_fav_bitor.aid  "
		"				, CASE WHEN((visible & 1)>0) THEN jsonb_build_object('1', previos) ELSE '{}'::jsonb END "
		"				|| CASE WHEN(period IS NOT NULL) THEN "
		"					CASE WHEN((visible & 2)>0) THEN jsonb_build_object('2', EXTRACT(EPOCH FROM period)) ELSE '{}' END "
		"					|| CASE WHEN((visible & 4)>0) THEN jsonb_build_object('4', previos + period) ELSE '{}' END "
		"					|| CASE WHEN((visible & 8)>0) THEN jsonb_build_object('8', ROUND((EXTRACT(EPOCH FROM(previos + period - now())) / 86400)::NUMERIC, 2))  ELSE '{}' END "
		"				ELSE '{}' END "
		"				) as last_act "
		"		FROM(SELECT aid, bit_or(visible) AS visible FROM cls_tree cls "
		"			INNER JOIN fav_act ON fav_act.cid = cls.id AND fav_act.usr = CURRENT_USER "
		"			GROUP BY aid)all_fav_bitor "
		"		LEFT JOIN  LATERAL(SELECT ref_cls_act.period "
		"			FROM cls_tree ct "
		"			INNER JOIN ref_cls_act ON ref_cls_act.period IS NOT NULL "
		"			AND ref_cls_act.cls_id = ct.id "
		"			AND ref_cls_act.act_id = all_fav_bitor.aid "
		"		)ref_ca ON TRUE "
		"		LEFT JOIN(SELECT MAX(timemark) AS previos, act_id AS aid, obj_id AS oid "
		"			FROM log_main GROUP BY obj_id, act_id) "
		"		last_log             ON  last_log.oid = obj.id "
		"		AND last_log.aid = all_fav_bitor.aid "
		"		AND last_log.aid<>0 "
		"	) AS ainfo "
		" FROM get_childs_cls(%s) cls "
		" INNER JOIN obj obj ON obj.cls_id = cls._id "
		" WHERE %s"
		" ORDER BY " //cls._title ASC "
		" (substring(cls._title, '^[0-9]+')::INT, cls._title ) ASC "
		" ,(substring(obj.title, '^[0-9]+')::INT, obj.title ) ASC "
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

			wxString str;
			table->GetAsString(11, row, str);
			if (!str.empty())
			{
				std::wstringstream ss(str.ToStdWstring());
				obj->mProp.clear();
				boost::property_tree::read_json(ss, obj->mProp);
			}
			str.clear();
			table->GetAsString(12, row, str);
			obj->ParseActInfo(str);
			
		};


		std::set<int64_t> loaded_id;
		for (; row < rowQty; row++)
		{
			int64_t cid;
			if (!table->GetAsString(0, row).ToLongLong(&cid))
				throw;
			auto ins_it = loaded_id.emplace(cid);
			if (ins_it.second)
			{
				const std::shared_ptr<ClsRec64>& curr = mCache.mClsTable.GetById(cid, load_cls);
				if (mGroupByType)
					toinsert.emplace_back(curr.get());
			}

			int64_t oid, parent_oid;
			if (!table->GetAsString(6, row).ToLongLong(&oid))
				throw;
			if (!table->GetAsString(7, row).ToLongLong(&parent_oid))
				throw;
			const std::shared_ptr<ObjRec64>& obj = mCache.mObjTable.GetObjById(oid, parent_oid, load_obj);
			if (!obj)
				throw;

			if (!mGroupByType)
				toinsert.emplace_back(obj.get());
							
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
		"   FROM obj WHERE obj.cls_id = acls.id GROUP BY cls_id)  AS qty"
		", pid "
		" FROM acls"
		" WHERE acls.id > 99 "
		" AND pid = %s"
		" ORDER BY acls.title ASC"
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
void ModelBrowser::DoAct()
{
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoMove()
{
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoFind(const wxString& str)
{
	SetSearchString(str);
	DoRefresh();
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoAddType()
{
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoAddObject()
{
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoDeleteSelected()
{
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoUpdateSelected()
{
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoGroupByType(bool enable_group_by_type)
{
	SetGroupedByType(enable_group_by_type);
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
		

	sigUpdateTitle(title, mIco);
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
