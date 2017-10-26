#include "_pch.h"
#include "ModelBrowser.h"

using namespace wh;

std::shared_ptr<ClsRec64>  ClsCache::mNullObj = std::shared_ptr<ClsRec64>(nullptr);
std::shared_ptr<ObjRec64>  ObjCache::mNullObj = std::shared_ptr<ObjRec64>(nullptr);

/*
ObjCache gObjCache;

//-----------------------------------------------------------------------------
void ClsRec64::RefreshObjects()
{
	TEST_FUNC_TIME;

	ClearObjTable();

	wxString query = wxString::Format(
		"SELECT o.id, o.title, o.qty, o.pid "
		//"       ,get_path_objnum(o.pid,1)  AS path"
		" FROM obj o "
		" WHERE o.id>0 AND o.cls_id = %s "
		" ORDER BY "
		"   (substring(o.title, '^[0-9]+')::INT, o.title ) ASC "
		, GetIdAsString()
	);
	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);

	if (table)
	{
		const unsigned int rowQty = table->GetRowCount();

		size_t i = 0;
		const ObjCache::fnModify fn = [this, &table, &i](const std::shared_ptr<ObjRec64>& obj)
		{
			obj->mCls = this->shared_from_this();
			obj->SetId(table->GetAsString(0, i));
			table->GetAsString(1, i, obj->mTitle);
			table->GetAsString(2, i, obj->mQty);
		};


		//NotyfyTable toinsert;
		for (; i < rowQty; i++)
		{
			int64_t id;
			if ( !table->GetAsString(0, i).ToLongLong(&id) )
				throw;
			const std::shared_ptr<ObjRec64>& obj = gObjCache.GetObjById(id, fn);

			//std::shared_ptr<ObjRec64>& obj = std::make_shared<ObjRec64>();
			//obj->mCls = this->shared_from_this();
			//obj->SetId(table->GetAsString(0, i));
			//table->GetAsString(1, i, obj->mTitle);
			//table->GetAsString(2, i, obj->mQty);

			// удаляется привыходе из области видимости
			//std::shared_ptr<ObjRec64>& parent_obj = std::make_shared<ObjRec64>();
			//parent_obj->SetId(table->GetAsString(3, i));
			//obj->SetParent(parent_obj);


			AddObj(obj);
			//toinsert.emplace_back(obj.get());
		}
		//if (!toinsert.empty())
		//	sigAfterInsert(*cls, toinsert);


	}//if (table)
	whDataMgr::GetDB().Commit();
	
}
*/


//-----------------------------------------------------------------------------
void ClsRec64::RefreshChilds()
{
	TEST_FUNC_TIME;

	auto id = GetIdAsString();

	//sigClear();
	ClearChilds();
	//sigAfterPathChange(*parent_node); // при смене корня сбрасывается весь кэш

	wxString query = wxString::Format(
		"SELECT  id, title, kind, measure"
		", (SELECT COALESCE(SUM(qty), 0)"
		"		FROM obj WHERE obj.cls_id = acls.id GROUP BY cls_id)  AS qty"
		" FROM acls"
		" WHERE acls.id > 99 AND pid = %s"
		" ORDER BY acls.title ASC"
		, id
	);
	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);

	if (table)
	{
		unsigned int rowQty = table->GetRowCount();
		//insert
		//NotyfyTable toinsert;
		for (size_t i = 0; i < rowQty; i++)
		{

			//auto curr = std::make_shared<ClsRec64>();
			auto curr = ClsRec64::MakeShared();
			curr->SetId(table->GetAsString(0, i));
			table->GetAsString(1, i, curr->mTitle);
			ToClsKind(table->GetAsString(2, i), curr->mKind);
			table->GetAsString(3, i, curr->mMeasure);
			table->GetAsString(4, i, curr->mObjQty);


			curr->SetParent(this->shared_from_this());

			AddChild(curr);
			//toinsert.emplace_back(new_node.get());
		}

		//if (!toinsert.empty())
		//	sigAfterInsert(*parent_node, toinsert);

	}//if (table)
	whDataMgr::GetDB().Commit();

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ClsTree::ClsTree()
{
	//std::shared_ptr<ClsRec64> value = std::make_shared<ClsRec64>();
	std::shared_ptr<ClsRec64> root = ClsRec64::MakeShared();
	root->SetId(1);
	root->mTitle = "ClsRoot";
	root->mKind = ClsKind::Abstract;
	
	mRoot = root;
	mCurrent = root;
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
	mCurrent->ClearChilds();
		
	mCurrent = mRoot;
	mRoot->ClearChilds();

	wxString query = wxString::Format(
		"SELECT id, title,kind FROM public.get_path_cls_info(%s, 1)"
		, id);

	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table)
	{
		unsigned int rowQty = table->GetRowCount();
		for (size_t i = rowQty; i > 0 ; --i)
		{
			size_t pos = i - 1;
			//auto value = std::make_shared<ClsRec64>();
			auto value = ClsRec64::MakeShared();
			value->SetId(table->GetAsString(0, pos));
			table->GetAsString(1, pos, value->mTitle);
			ToClsKind(table->GetAsString(2, pos), value->mKind);

			value->SetParent(mCurrent);
			mCurrent->AddChild(value);
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
	auto new_curr = ClsRec64::MakeShared();
	new_curr->SetId(val);

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
	//sigClear();
	
	mClsPath.sigBeforePathChange.connect(sigBeforePathChange);

	
	//mClsPath.sigBeforePathChange.connect([this](const ICls64& parent)
	//	{

	//	});
		
	
	mClsPath.sigAfterPathChange.connect(sigAfterPathChange);
	mClsPath.sigAfterPathChange.connect([this](const ICls64&)
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

	std::vector<const IObj64*> todelete;
	
	if (cls->GetObjTable())
	{
		for (const auto& obj : *cls->GetObjTable())
		{
			todelete.emplace_back(obj.get());
		}
	}
	if (!todelete.empty())
		sigObjOperation(Operation::BeforeDelete, todelete);
	
	//auto cls64 = std::dynamic_pointer_cast<ClsRec64>(cls);
	//cls64->RefreshObjects();
	
	cls->ClearObjTable();

	wxString query = wxString::Format(
		"SELECT o.id, o.title, o.qty, o.pid "
		"       ,get_path_objnum(o.pid,1)  AS path"
		"       ,prop"
		" FROM obj o "
		" WHERE o.id>0 AND o.cls_id = %s "
		//" ORDER BY (substring(o.title, '^[0-9]+')::INT, o.title ) ASC "
		, cls->GetIdAsString()
	);
	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table)
	{
		unsigned int rowQty = table->GetRowCount();
		std::shared_ptr<ClsRec64> parent_node = std::dynamic_pointer_cast<ClsRec64>(cls);

		size_t i = 0;
		const ObjCache::fnModify fn = [&parent_node, &table, &i](const std::shared_ptr<ObjRec64>& obj)
		{
			obj->mCls = parent_node;
			obj->mId = 0;
			obj->SetId(table->GetAsString(0, i));
			table->GetAsString(1, i, obj->mTitle);
			table->GetAsString(2, i, obj->mQty);

			obj->mParentId = 0;
			obj->SetParentId(table->GetAsString(3, i));

			obj->mPath = std::make_shared<ObjPath64>();
			table->GetAsString(4, i, obj->mPath->mStrPath );
			
		};

		
		std::vector<const IObj64*> toinsert;
		for (; i < rowQty; i++)
		{
			int64_t id, parentId;
			if (!table->GetAsString(0, i).ToLongLong(&id))
				throw;
			if (!table->GetAsString(3, i).ToLongLong(&parentId))
				throw;
			const std::shared_ptr<ObjRec64>& obj = mObjCache.GetObjById(id, parentId, fn);
			if (!obj)
				throw;
			parent_node->AddObj(obj);
			toinsert.emplace_back(obj.get());
		}
		if (!toinsert.empty())
			sigObjOperation(Operation::AfterInsert, toinsert);

	}//if (table)
	whDataMgr::GetDB().Commit();
}

//-----------------------------------------------------------------------------
void ModelBrowser::DoRefresh()
{
	TEST_FUNC_TIME;

	//std::shared_ptr<ICls64>& cls = mClsPath.GetCurrent();
	//auto& cls64 = std::dynamic_pointer_cast<ClsRec64>(cls);
	//cls64->RefreshChilds();
	//sigAfterPathChange(*cls);

	std::shared_ptr<ICls64>& parent_node = mClsPath.GetCurrent();
	auto id = parent_node->GetIdAsString();
	
	std::vector<const ICls64*> toinsert;
	sigBeforeRefreshCls(toinsert, parent_node.get());
	
	
	parent_node->ClearChilds();
	//insert
	


	wxString query = wxString::Format(
		"SELECT  id, title, kind, measure"
		", (SELECT COALESCE(SUM(qty), 0)"
		"		FROM obj WHERE obj.cls_id = acls.id GROUP BY cls_id)  AS qty"
		" FROM acls"
		" WHERE acls.id > 99 AND pid = %s"
		//" ORDER BY acls.title ASC"
		, id
		);
	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);

	if (table)
	{
		unsigned int rowQty = table->GetRowCount();
		size_t i = 0;
		const ClsCache::fnModify fn = [&parent_node, &table, &i](const std::shared_ptr<ClsRec64>& cls)
		{
			cls->SetId(table->GetAsString(0, i));
			table->GetAsString(1, i, cls->mTitle);
			ToClsKind(table->GetAsString(2, i), cls->mKind);
			table->GetAsString(3, i, cls->mMeasure);
			table->GetAsString(4, i, cls->mObjQty);
			cls->SetParent(parent_node);
		};


		for (; i < rowQty; i++)
		{
			int64_t id;
			if (!table->GetAsString(0, i).ToLongLong(&id))
				throw;
			const std::shared_ptr<ClsRec64>& curr = mClsCache.GetById(id, fn);
			
			parent_node->AddChild(curr);
			
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

	auto id = cls->GetId();


	if (ClsKind::Abstract == cls->GetKind())
		mClsPath.SetId(id);
	else
	{
		std::shared_ptr<ICls64>& node = mClsPath.GetCurrent();
		const auto childs = node->GetChilds();
		if (childs && !childs->empty())
		{
			for (size_t i = 0; i < childs->size(); ++i)
			{
				if (childs->at(i)->GetId() == id)
				{
					DoRefreshObjects(childs->at(i));
					break;
				}
			}
		}

	}
		
		

}
//-----------------------------------------------------------------------------
void ModelBrowser::DoUp()
{
	mClsPath.Up();
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
void ModelBrowser::DoFind(const wxString&)
{
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
