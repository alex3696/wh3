#include "_pch.h"
#include "ModelBrowser.h"

using namespace wh;

const std::shared_ptr<ClsRec64>  ClsCache::mNullObj = std::shared_ptr<ClsRec64>(nullptr);
const std::shared_ptr<ObjRec64>  ObjCache::mNullObj = std::shared_ptr<ObjRec64>(nullptr);

//virtual 
std::shared_ptr<const ICls64> ClsRec64::GetParent()const //override
{
	return mTable->GetCache()->mClsTable.GetById(mParentId);
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
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ClsTree::ClsTree(ClsCache* clsTable)
	:mClsCache(clsTable)
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
		auto id = mCurrent->GetParent()->GetId();
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
ModelBrowser::ModelBrowser()
{
	mClsPath = std::make_unique<ClsTree>(&mCache.mClsTable);

	//sigClear();
	
	mClsPath->sigBeforePathChange.connect(sigBeforePathChange);

	
	//mClsPath.sigBeforePathChange.connect([this](const ICls64& parent)
	//	{

	//	});
		
	
	mClsPath->sigAfterPathChange.connect(sigAfterPathChange);
	mClsPath->sigAfterPathChange.connect([this](const ICls64&)
		{
			DoRefresh();
			//sigAfterRefreshCls(toinsert, parent_node.get()); // internal in DoRefresh
		});
	
}
//-----------------------------------------------------------------------------
ModelBrowser::~ModelBrowser()
{
	
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoRefreshObjects(const std::shared_ptr<ICls64>& cls)
{
	TEST_FUNC_TIME;

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
			sigBeforeRefreshCls(todelete, cls.get());
	}
	std::shared_ptr<ClsRec64> parent_node = std::dynamic_pointer_cast<ClsRec64>(cls);

	if (parent_node->mObjLoaded)
	{
		std::vector<const IIdent64*> toinsert;
		sigBeforeRefreshCls(toinsert, cls.get());
		mCache.mObjTable.GetObjByClsId(cls->GetId(), toinsert);
		sigAfterRefreshCls(toinsert, cls.get());
		return;
	}
		
	
	//auto cls64 = std::dynamic_pointer_cast<ClsRec64>(cls);
	//cls64->RefreshObjects();
	
	cls->ClearObjTable();
	

	wxString query = wxString::Format(
		"SELECT o.id, o.title, o.qty, o.pid "
		"       ,get_path_objnum(o.pid,1)  AS path"
		"       ,prop"
		"       ,cls_id"
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
		

		size_t i = 0;
		const ObjCache::fnModify fn = [&parent_node, &table, &i](const std::shared_ptr<ObjRec64>& obj)
		{
			//obj->mClsId = parent_node->GetId();
			//obj->mCls = parent_node;
			obj->SetId(table->GetAsString(0, i));
			table->GetAsString(1, i, obj->mTitle);
			table->GetAsString(2, i, obj->mQty);

			obj->SetParentId(table->GetAsString(3, i));

			obj->mPath = std::make_shared<ObjPath64>();
			table->GetAsString(4, i, obj->mPath->mStrPath );
			
			obj->SetClsId(table->GetAsString(6, i));
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
		if (!toinsert.empty())
		{
			if(mGroupByType)
				sigObjOperation(Operation::AfterInsert, toinsert);
			else
				sigAfterRefreshCls(toinsert, cls.get());
		}
			

	}//if (table)
	whDataMgr::GetDB().Commit();
	parent_node->mObjLoaded = true;
}

//-----------------------------------------------------------------------------
void ModelBrowser::DoRefresh()
{
	TEST_FUNC_TIME;

	std::shared_ptr<ICls64>& parent_node = mClsPath->GetCurrent();
	auto id = parent_node->GetIdAsString();

	if (ClsKind::Abstract != parent_node->GetKind())
	{
		DoRefreshObjects(parent_node);
		return;
	}

	
	std::vector<const IIdent64*> toinsert;
	sigBeforeRefreshCls(toinsert, parent_node.get());
	
	
	//parent_node->ClearChilds();
	//insert

	wxString search;
	if (!mSearchWords.empty())
	{
		for (const auto& word: mSearchWords)
		{
			search += wxString::Format(" acls.title ~~*'%%%s%%' OR", word);
		}
		search.RemoveLast(2);
		search = wxString::Format("AND (%s)", search);
		//search = " AND (" + search + ")";
	}
	else
		search = wxString::Format("AND pid = %s", id);
	


	wxString query = wxString::Format(
		"SELECT  id, title, kind, measure"
		", (SELECT COALESCE(SUM(qty), 0)"
		"   FROM obj WHERE obj.cls_id = acls.id GROUP BY cls_id)  AS qty"
		", pid "
		" FROM acls"
		" WHERE acls.id > 99 "
		" %s " //" AND pid = %s"
		" ORDER BY acls.title ASC"
		, search
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
		sigAfterRefreshCls(toinsert, parent_node.get());
}
//-----------------------------------------------------------------------------
//void ModelBrowser::DoActivate(int64_t id)
void ModelBrowser::DoActivate(const ICls64* cls)
{
	if (!cls)
		return;
	mSearchWords.clear();

	auto id = cls->GetId();

	if (ClsKind::Abstract == cls->GetKind())
		mClsPath->SetId(id);
	else
	{
		if (!mGroupByType)
			mClsPath->SetId(id);

		const std::shared_ptr<ClsRec64>& node = mCache.mClsTable.GetById(id);
		DoRefreshObjects(node);
	}
		
		

}
//-----------------------------------------------------------------------------
void ModelBrowser::DoUp()
{
	mClsPath->Up();
	//DoRefresh();
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
	mSearchWords.clear();

	if (!str.empty())
	{
		size_t start = 0;
		while (wxString::npos != start)
		{
			size_t next = str.find(" ", start);

			wxString word;

			if (wxString::npos != next)
			{
				word = str.substr(start, next - start);
				next++;
			}
			else
				word = str.substr(start, str.size() - start);

			mSearchWords.emplace_back(word);
			start = next;
		}
	}
	
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
	mGroupByType = enable_group_by_type;
	sigGroupByType(mGroupByType);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ModelPageBrowser::ModelPageBrowser()
	:mFiltersVisible(true), mGroupedByType(true), mCollapsedGroup(true)
{

}
//-----------------------------------------------------------------------------
void ModelPageBrowser::DoShowFilters(bool show_filters)
{

}
//-----------------------------------------------------------------------------
void ModelPageBrowser::DoEnableGroupByType(bool enable_group_by_type)
{

}
//-----------------------------------------------------------------------------
void ModelPageBrowser::DoCollapsedGroupByType(bool enable_collapse_by_type)
{

}
//---------------------------------------------------------------------------
//virtual 
void ModelPageBrowser::UpdateTitle() //override;
{
	sigUpdateTitle(mTitle, mIco);
	mModelBrowser.DoRefresh();
}
//---------------------------------------------------------------------------
//virtual 
void ModelPageBrowser::Show()//override;
{
	//Update();
	sigShow();
}
//---------------------------------------------------------------------------
//virtual 
void ModelPageBrowser::Load(const boost::property_tree::ptree& page_val)//override;
{
	int mode = page_val.get<size_t>("CtrlPageHistory.Mode", 0);
	
	/*
	rec::PageHistory cfg;
	cfg.mRowsOffset = page_val.get<size_t>("CtrlPageHistory.Offset", 0);
	cfg.mRowsLimit = page_val.get<size_t>("CtrlPageHistory.Limit", 20);
	cfg.mStringPerRow = page_val.get<size_t>("CtrlPageHistory.StringPerRow", 4);
	cfg.mShowFilterList = page_val.get<bool>("CtrlPageHistory.ShowFilterList", false);
	cfg.mShowPropertyList = page_val.get<bool>("CtrlPageHistory.ShowPropertyList", false);
	cfg.mColAutosize = page_val.get<bool>("CtrlPageHistory.ColAutosize", false);
	cfg.mPathInProperties = page_val.get<bool>("CtrlPageHistory.PathInProperties", false);
	cfg.mVisibleColumnClsObj = page_val.get<bool>("CtrlPageHistory.VisibleColumnClsObj", true);
	*/
}
//---------------------------------------------------------------------------
//virtual 
void ModelPageBrowser::Save(boost::property_tree::ptree& page_val)//override;
{
	using ptree = boost::property_tree::ptree;
	ptree content;
	//content.put("Limit", mGuiModel.mRowsLimit);
	page_val.push_back(std::make_pair("CtrlPageBrowser", content));

}
