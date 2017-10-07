#include "_pch.h"
#include "ModelBrowser.h"

using namespace wh;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ClsTree::ClsTree()
{
	std::shared_ptr<ClsRec64> value = std::make_shared<ClsRec64>();
	value->SetId(1);
	value->mTitle = "ClsRoot";
	value->mKind = ClsKind::Abstract;

	auto root = std::make_shared<ClsNode>(nullptr, value);
	//root->SetValue(value);

	mRoot = root;
	mCurrent = root;
}
//-----------------------------------------------------------------------------
void ClsTree::Up()
{
	if (mCurrent != mRoot)
	{
		auto parent = mCurrent->GetParent();

		std::shared_ptr<ClsRec64> value = std::make_shared<ClsRec64>();
		value->SetId(mCurrent->GetParent()->GetValue()->GetId());

		mCurrent = std::make_shared<ClsNode>(parent, value);
	}
		
	Refresh();
}
//-----------------------------------------------------------------------------
void ClsTree::Home()
{
	mCurrent = mRoot;
	Refresh();
}
//-----------------------------------------------------------------------------
void ClsTree::Refresh()
{
	auto p0 = GetTickCount();

	sigBeforePathChange(*mCurrent);

	auto id = mCurrent->GetValue()->GetIdAsString();
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
				auto value = std::make_shared<ClsRec64>();
				value->SetId(table->GetAsString(0, pos));
				table->GetAsString(1, pos, value->mTitle);
				ToClsKind(table->GetAsString(2, pos), value->mKind);
					
				auto curr = std::make_shared<ClsNode>(mCurrent);
				curr->SetValue(value);

				mCurrent->AddChild(curr);
				mCurrent = curr;

					

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
	Home();
}
//-----------------------------------------------------------------------------
void ClsTree::SetId(const int64_t& val)
{
	auto value = std::make_shared<ClsRec64>();
	value->SetId(val);

	//std::shared_ptr<const ClsNode> croot = mRoot;
	//auto curr = std::make_shared<ClsNode>(croot);
	auto curr = std::make_shared<ClsNode>(mRoot);
	curr->SetValue(value);
	mCurrent = curr;
	Refresh();
}
//-----------------------------------------------------------------------------
int64_t  ClsTree::GetId()const
{
	return mCurrent->GetValue()->GetId();
}
//-----------------------------------------------------------------------------
wxString ClsTree::GetIdAsString()const
{
	wxLongLong tmp(GetId());
	return tmp.ToString();
}
//-----------------------------------------------------------------------------
/*
std::shared_ptr<const ClsNode> ClsTree::AddValToCurrent(const std::shared_ptr<ICls64>& val)
{
	//auto p0 = GetTickCount();
	
	auto new_node = std::make_shared<ClsNode>(mCurrent, val);
	mCurrent->AddChild(new_node);

	//wxLogMessage(wxString::Format("%d\t %s ", GetTickCount() - p0, __FUNCTION__));
	return new_node;
}
*/
//-----------------------------------------------------------------------------
wxString ClsTree::AsString()const //override
{
	wxString ret="/";
	std::shared_ptr<const ClsNode> curr = mCurrent;
	while (curr!=mRoot)
	{
		const auto& title = curr->GetValue()->GetTitle();

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
void ModelBrowser::DoRefreshNodeChilds(const std::shared_ptr<ClsNode>& parent_node)
{
	auto p0 = GetTickCount();
	auto cls = std::dynamic_pointer_cast<const ClsRec64>(parent_node->GetValue());
	if (!cls)
		return;

	NotyfyTable todelete;
	const auto& curr_childs = parent_node->GetChilds();
	if (curr_childs && !curr_childs->empty())
	{
		for (const auto& child : *curr_childs)
			todelete.emplace_back(child.get());
		sigBeforeDelete(*parent_node, todelete);
		parent_node->ClearChilds();
	}
	


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

		NotyfyTable toinsert;
		for (size_t i = 0; i < rowQty; i++)
		{
			std::shared_ptr<ClsNode>& new_node = std::make_shared<ClsNode>();
			{
				std::shared_ptr<ObjRec64>& obj = std::make_shared<ObjRec64>();
				obj->mCls = cls;
				obj->SetId(table->GetAsString(0, i));
				table->GetAsString(1, i, obj->mTitle);
				table->GetAsString(2, i, obj->mQty);
				obj->SetParentOid(table->GetAsString(3, i));

				new_node->SetValue(obj);
			}
			
			new_node->SetParent(parent_node);

			parent_node->AddChild(new_node);
			toinsert.emplace_back(new_node.get());
		}
		//if (!toinsert.empty())
		//	sigAfterInsert(*parent_node, toinsert);


	}//if (table)
	whDataMgr::GetDB().Commit();
	wxLogMessage(wxString::Format("%d\t %s END", GetTickCount() - p0, __FUNCTION__));

}

//-----------------------------------------------------------------------------
void ModelBrowser::DoRefresh()
{
	auto p0 = GetTickCount();
	std::shared_ptr<ClsNode>& parent_node = mClsPath.GetCurrent();
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
		/*
		//expand cache
		for (size_t i = mClsNodeCache.size(); i < rowQty; i++)
		{
			std::shared_ptr<ClsRec64> val = std::make_shared<ClsRec64>();
			auto new_node = std::make_shared<ClsNode>(parent_node, val);
			mClsNodeCache.emplace_back(new_node);
			mClsValCache.emplace_back(val);
		}
		*/
		
		//insert
		NotyfyTable toinsert;
		for (size_t i = 0; i < rowQty; i++)
		{
			
			//const std::shared_ptr<ClsRec64>&	curr = mClsValCache[i];
			auto curr = std::make_shared<ClsRec64>();
			curr->SetId(table->GetAsString(0, i));
			table->GetAsString(1, i, curr->mTitle);
			ToClsKind(table->GetAsString(2, i), curr->mKind);
			table->GetAsString(3, i, curr->mMeasure);
			table->GetAsString(4, i, curr->mObjQty);

			//const std::shared_ptr<ClsNode>&		new_node = mClsNodeCache[i];
			//new_node->SetValue(curr);
			//new_node->SetParent(parent_node);
			auto new_node = std::make_shared<ClsNode>(parent_node, curr);
			
			
			
			parent_node->AddChild(new_node);
			toinsert.emplace_back(new_node.get());
		}
		sigAfterPathChange(*parent_node);

		//if (!toinsert.empty())
		//	sigAfterInsert(*parent_node, toinsert);

	}//if (table)
	whDataMgr::GetDB().Commit();
	wxLogMessage(wxString::Format("%d\t %s END", GetTickCount() - p0, __FUNCTION__));
	
	/*
	//mClsPath.AddValToCurrent(std::make_shared<ClsNode>())

	size_t offset=0;
	if (parent->GetId() != 1)
	{
		offset = 1;
		if (mClsAll.empty())
		{
			std::shared_ptr<ClsRec64> to_up = std::make_shared<ClsRec64>();
			to_up->mId = parent->GetId();
			to_up->mTitle = "..";

			NotyfyTable toinsert;
			toinsert.emplace_back(to_up.get());
			mClsAll.emplace_back(to_up);
			sigAfterInsert(toinsert);
		}
		else
		{
			NotyfyTable toupdate;
			auto to_up = std::dynamic_pointer_cast<ClsRec64>(mClsAll[0]);
			to_up->mId = parent->GetId();
			to_up->mTitle = "..";
			toupdate.emplace_back(to_up.get());
			sigAfterUpdate(toupdate);
		}
	}
	
	auto id = mClsPath.GetIdAsString();

	wxString query = wxString::Format(
		"SELECT  id, title, kind, measure"
		", (SELECT COALESCE(SUM(qty), 0)"
		"		FROM obj WHERE obj.cls_id = acls.id GROUP BY cls_id)  AS qty"
		" FROM acls"
		" WHERE acls.id > 100 AND pid = %s"
		" ORDER BY acls.title ASC"
		, id
		);
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);

	//sigClear();


	if (table)
	{
		size_t m = 0;


		std::function<void(std::shared_ptr<ICls64>& e)> itemChanger64
			= [&table, &m, &parent](std::shared_ptr<ICls64>& e)
		{
			auto curr = std::dynamic_pointer_cast<ClsRec64>(e);
			curr->SetId(table->GetAsString(0, m));
			table->GetAsString(1, m, curr->mTitle);
			ToClsKind(table->GetAsString(2, m), curr->mKind);
			table->GetAsString(3, m, curr->mMeasure);
			
			curr->mParent = parent;

		};

		unsigned int arrQty = mClsAll.size();
		unsigned int rowQty = table->GetRowCount();
		if ((rowQty + offset) < arrQty)
		{
			// delete
			NotyfyTable todel;
			for (auto& it = mClsAll.begin() + (rowQty + offset); it != mClsAll.end(); ++it)
				todel.emplace_back((*it).get());
			if (!todel.empty())
				sigBeforeDelete(todel);
			
			mClsAll.erase(mClsAll.begin() + (rowQty + offset), mClsAll.end());
			arrQty = rowQty + offset;
		}
	
		// update
		NotyfyTable toupdate;
		for (size_t i = offset; i < arrQty; i++)
		{
			auto it64 = mClsAll.begin() + i;
			mClsAll.modify(it64, itemChanger64);

			toupdate.emplace_back((*it64).get());
			m++;
		}
		if (!toupdate.empty())
			sigAfterUpdate(toupdate);

		// insert
		NotyfyTable toinsert;
		for (size_t i = arrQty; i < (rowQty + offset); i++)
		{
			std::shared_ptr<ICls64> curr64 = std::make_shared<ClsRec64>();
			itemChanger64(curr64);
			mClsAll.emplace_back(curr64);

			toinsert.emplace_back(curr64.get());
			m++;
		}
		if (!toinsert.empty())
			sigAfterInsert(toinsert);


	}//if (table)
	whDataMgr::GetDB().Commit();

	//if (1 != mClsPath.GetId())

		



	//RestoreSelect();
	auto sel_vec = mClsSelected;
	DoSelect(sel_vec);
	*/

	
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoActivate(int64_t id)
{
	auto p0 = GetTickCount();

	const auto& curr_childs = mClsPath.GetCurrent()->GetChilds();
	if (!curr_childs || curr_childs->empty() )
		return;

	auto it = std::find_if(curr_childs->cbegin(), curr_childs->cend()
		, [&id](const std::shared_ptr<ClsNode>& it)
		{
			return it->GetValue()->GetId() == id;
		});

	if (curr_childs->cend() == it)
		return;

	const std::shared_ptr<ClsNode>& parent_node = *it;



	auto cls = std::dynamic_pointer_cast<const ClsRec64>(parent_node->GetValue());

	if (cls)
	{
		if(ClsKind::Abstract == cls->GetKind())
			mClsPath.SetId(id);
		else
			DoRefreshNodeChilds(parent_node);
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
void ModelBrowser::DoCollapseGroupByType(bool enable_collapse_by_type)
{
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















