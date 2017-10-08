#include "_pch.h"
#include "ModelBrowser.h"

using namespace wh;
//-----------------------------------------------------------------------------
void ClsRec64::RefreshObjects()
{
	auto p0 = GetTickCount();

	ClearObjTable();


	wxString query = wxString::Format(
		"SELECT o.id, o.title, o.qty, o.pid "
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
		unsigned int rowQty = table->GetRowCount();

		//NotyfyTable toinsert;
		for (size_t i = 0; i < rowQty; i++)
		{
			std::shared_ptr<ObjRec64>& obj = std::make_shared<ObjRec64>();
			obj->mCls = this->shared_from_this();
			obj->SetId(table->GetAsString(0, i));
			table->GetAsString(1, i, obj->mTitle);
			table->GetAsString(2, i, obj->mQty);

			std::shared_ptr<ObjRec64>& parent_obj = std::make_shared<ObjRec64>();
			parent_obj->SetId(table->GetAsString(3, i));
			obj->SetParent(parent_obj);


			AddObj(obj);
			//toinsert.emplace_back(obj.get());
		}
		//if (!toinsert.empty())
		//	sigAfterInsert(*cls, toinsert);


	}//if (table)
	whDataMgr::GetDB().Commit();
	wxLogMessage(wxString::Format("%d\t %s", GetTickCount() - p0, __FUNCTION__));
}
//-----------------------------------------------------------------------------
void ClsRec64::RefreshChilds()
{
	auto p0 = GetTickCount();
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
	wxLogMessage(wxString::Format("%d\t %s", GetTickCount() - p0, __FUNCTION__));

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ClsTree::ClsTree()
{
	//std::shared_ptr<ClsRec64> value = std::make_shared<ClsRec64>();
	std::shared_ptr<ClsRec64> value = ClsRec64::MakeShared();
	value->SetId(1);
	value->mTitle = "ClsRoot";
	value->mKind = ClsKind::Abstract;

	auto root = value;
	
	mRoot = root;
	mCurrent = root;
}
//-----------------------------------------------------------------------------
void ClsTree::Up()
{
	if (mCurrent != mRoot)
	{
		auto parent = mCurrent->GetParent();

		//std::shared_ptr<ClsRec64> value = std::make_shared<ClsRec64>();
		std::shared_ptr<ClsRec64> value = ClsRec64::MakeShared();
		value->SetId(mCurrent->GetParent()->GetId());

		value->SetParent(parent);

		mCurrent = value;
	}
		
	Refresh();
}
//-----------------------------------------------------------------------------
void ClsTree::Refresh()
{
	auto p0 = GetTickCount();

	sigBeforePathChange(*mCurrent);

	auto id = mCurrent->GetIdAsString();
	mCurrent->ClearChilds();
		
	mCurrent = mRoot;
	mRoot->ClearChilds();
	//if (1 != GetId())
	{
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

	}//if ("1" != id)
	whDataMgr::GetDB().Commit();

	sigAfterPathChange(*mCurrent);


	wxLogMessage(wxString::Format("%d\t %s ", GetTickCount() - p0, __FUNCTION__));
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
	//auto value = std::make_shared<ClsRec64>();
	auto value = ClsRec64::MakeShared();
	value->SetId(val);

	//std::shared_ptr<const ClsNode> croot = mRoot;
	//auto curr = std::make_shared<ClsNode>(croot);
	mCurrent = value;
	Refresh();
}
//-----------------------------------------------------------------------------
int64_t  ClsTree::GetId()const
{
	return mCurrent->GetId();
}
//-----------------------------------------------------------------------------
wxString ClsTree::GetIdAsString()const
{
	wxLongLong tmp(GetId());
	return tmp.ToString();
}
//-----------------------------------------------------------------------------
wxString ClsTree::AsString()const //override
{
	wxString ret="/";
	std::shared_ptr<const ICls64> curr = mCurrent;
	while (curr!=mRoot)
	{
		const auto& title = curr->GetTitle();

		if (wxNOT_FOUND == title.Find('/'))
			ret = wxString::Format("/%s%s", title, ret);
		else
			ret = wxString::Format("/[%s]%s", title, ret);

		curr = curr->GetParent();
	}
	return ret;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ModelBrowser::ModelBrowser()
{
	//sigClear();
	
	mClsPath.sigBeforePathChange.connect(sigBeforePathChange);
	
	//mClsPath.sigAfterPathChange.connect(sigAfterPathChange);
	mClsPath.sigAfterPathChange.connect(std::bind(&ModelBrowser::DoRefresh, this));
	
}
//-----------------------------------------------------------------------------
ModelBrowser::~ModelBrowser()
{
	
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoRefreshObjects(const std::shared_ptr<ICls64>& cls)
{
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
	
	auto cls64 = std::dynamic_pointer_cast<ClsRec64>(cls);
	cls64->RefreshObjects();
	/*
	cls->ClearObjTable();

	wxString query = wxString::Format(
		"SELECT o.id, o.title, o.qty, o.pid "
		" FROM obj o "
		" WHERE o.id>0 AND o.cls_id = %s "
		" ORDER BY "
		"   (substring(o.title, '^[0-9]+')::INT, o.title ) ASC "
		, cls->GetIdAsString()
	);
	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table)
	{
		unsigned int rowQty = table->GetRowCount();
		
		//NotyfyTable toinsert;
		for (size_t i = 0; i < rowQty; i++)
		{
			std::shared_ptr<ObjRec64>& obj = std::make_shared<ObjRec64>();
			obj->mCls = cls;
			obj->SetId(table->GetAsString(0, i));
			table->GetAsString(1, i, obj->mTitle);
			table->GetAsString(2, i, obj->mQty);
			
			std::shared_ptr<ObjRec64>& parent_obj = std::make_shared<ObjRec64>();
			parent_obj->SetId(table->GetAsString(3, i));
			obj->SetParent(parent_obj);
			

			parent_node->AddObj(obj);
			//toinsert.emplace_back(obj.get());
		}
		//if (!toinsert.empty())
		//	sigAfterInsert(*cls, toinsert);


	}//if (table)
	whDataMgr::GetDB().Commit();
	wxLogMessage(wxString::Format("%d\t %s", GetTickCount() - p0, __FUNCTION__));
	*/
}

//-----------------------------------------------------------------------------
void ModelBrowser::DoRefresh()
{
	std::shared_ptr<ICls64>& cls = mClsPath.GetCurrent();
	auto& cls64 = std::dynamic_pointer_cast<ClsRec64>(cls);
	cls64->RefreshChilds();

	sigAfterPathChange(*cls);

	/*
	auto p0 = GetTickCount();
	std::shared_ptr<ICls64>& parent_node = mClsPath.GetCurrent();
	auto id = mClsPath.GetIdAsString();
	
	//sigClear();
	parent_node->ClearChilds();
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

			
			curr->SetParent(parent_node);
			
			parent_node->AddChild(curr);
			//toinsert.emplace_back(new_node.get());
		}
		sigAfterPathChange(*parent_node);

		//if (!toinsert.empty())
		//	sigAfterInsert(*parent_node, toinsert);

	}//if (table)
	whDataMgr::GetDB().Commit();
	wxLogMessage(wxString::Format("%d\t %s", GetTickCount() - p0, __FUNCTION__));

	*/
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
			size_t i = 0;
			for (; i < childs->size(); ++i)
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
