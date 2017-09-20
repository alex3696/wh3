#include "_pch.h"
#include "ModelBrowser.h"

using namespace wh;

//-----------------------------------------------------------------------------

ClsNode::ClsNode()
{

}
//-----------------------------------------------------------------------------
ClsNode::ClsNode(const std::shared_ptr<const ClsNode>& parent)
	:mParent(parent)
{
}
//-----------------------------------------------------------------------------
ClsNode::ClsNode(const std::shared_ptr<const ClsNode>& parent
	, const std::shared_ptr<ICls64>& value)
	: mParent(parent), mValue(value)
{

}


//-----------------------------------------------------------------------------
void ClsNode::SetValue(const std::shared_ptr<ICls64>& new_value)
{
	sigBeforeValChange(*this, mValue, new_value);
		
	auto tmp = mValue;
	mValue = new_value;

	sigBeforeValChange(*this, mValue, new_value);
}
//-----------------------------------------------------------------------------
std::shared_ptr<const ICls64> ClsNode::GetValue()const
{
	return mValue;
}
//-----------------------------------------------------------------------------
void ClsNode::ClearChilds()
{
	mChild.reset();
}
//-----------------------------------------------------------------------------
void ClsNode::AddChild(const std::shared_ptr<ClsNode>& new_node)
{
	if (!mChild)
		mChild = std::make_shared<ChildsTable>();

	mChild->emplace_back(new_node);

}


//-----------------------------------------------------------------------------
std::shared_ptr<const ClsNode> ClsNode::GetParent()const
{ 
	return mParent.lock(); 
}
//-----------------------------------------------------------------------------
void ClsNode::SetParent(const std::shared_ptr<const ClsNode>& parent)
{
	mParent = parent;
}


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

				mRoot->AddChild(curr);
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
std::shared_ptr<const ClsNode> ClsTree::AddValToCurrent(const std::shared_ptr<ICls64>& val)
{
	//auto p0 = GetTickCount();
	
	auto new_node = std::make_shared<ClsNode>(mCurrent, val);
	mCurrent->AddChild(new_node);

	//wxLogMessage(wxString::Format("%d\t %s ", GetTickCount() - p0, __FUNCTION__));
	return new_node;
}
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
//-----------------------------------------------------------------------------
ClsPath::ClsPath()
{	
	Home();
}
//-----------------------------------------------------------------------------
void ClsPath::Home()
{
	SetId(1);
}
//-----------------------------------------------------------------------------
void ClsPath::Refresh()
{
	if (!mParents.size())
		Home();

	auto id = GetIdAsString();
	mParents.clear();
	//if (1 != GetId())
	{
		wxString query = wxString::Format(
			"SELECT id, title FROM public.get_path_cls_info(%s, 0)"
			, id);
		
		whDataMgr::GetDB().BeginTransaction();
		auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
		if (table)
		{
			unsigned int rowQty = table->GetRowCount();
			for (size_t i = 0; i < rowQty; i++)
			{
				auto r = std::make_shared<ClsRec64>();
				r->SetId(table->GetAsString(0, i));
				table->GetAsString(1, i, r->mTitle);
				mParents.emplace_back(r);
			}
		}//if (table)
	}//if ("1" != id)
	sigPathChanged(AsString());
}
//-----------------------------------------------------------------------------
void ClsPath::Up()
{
	if (0 == GetId() || 1 == GetId())
	{
		Home();
		return;
	}	
	
	auto id = GetIdAsString();
	mParents.clear();
	wxString query = wxString::Format(
		"SELECT pid FROM acls WHERE id=%s"
		, id);

	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	wxString up_id = table->GetAsString(0, 0);
	whDataMgr::GetDB().Commit();

	SetId(up_id);

		//Refresh();
}
//-----------------------------------------------------------------------------
void ClsPath::SetId(const wxString& str)
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
	//Refresh();
};
//-----------------------------------------------------------------------------
void ClsPath::SetId(const int64_t& val) 
{ 
	mParents.clear();
	auto r = std::make_shared<ClsRec64>();
	r->mId = val;
	mParents.emplace_back(r);
	Refresh();
}
//-----------------------------------------------------------------------------
int64_t  ClsPath::GetId()const
{
	return mParents.empty() ? 1 : mParents.front()->GetId();
}
//-----------------------------------------------------------------------------
wxString ClsPath::GetIdAsString()const
{
	wxLongLong tmp(GetId());
	return tmp.ToString();
}
//-----------------------------------------------------------------------------
//virtual 
wxString ClsPath::AsString()const //override
{
	wxString ret;
	if (!mParents.empty())
	{
		size_t i = mParents.size();
		do{
			i--;

			wxString val;
			if (1 == mParents.at(i)->GetId())
				val = "���:";
			else
				val = mParents.at(i)->GetTitle();
			if (wxNOT_FOUND == val.Find('/'))
				ret += wxString::Format("%s/", val);
			else
				ret += wxString::Format("[%s]/", val);
		
		} while (i);
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
void ModelBrowser::DoRefresh()
{
	sigAfterPathChange(*mClsPath.GetCurrent());

	auto p0 = GetTickCount();

	//mClsPath.Refresh();



	const auto& parent = mClsPath.GetCurrent()->GetValue();

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
		wxLogMessage(wxString::Format("%d\t %s LOAD BEGIN", GetTickCount() - p0, __FUNCTION__));

		unsigned int rowQty = table->GetRowCount();

		//expand cache
		for (size_t i = mClsNodeCache.size(); i < rowQty; i++)
		{
			std::shared_ptr<ClsRec64> val = std::make_shared<ClsRec64>();
			auto parent = mClsPath.GetCurrent();
			auto new_node = std::make_shared<ClsNode>(parent, val);
			mClsNodeCache.emplace_back(new_node);
			mClsValCache.emplace_back(val);
		}
			
		
		NotyfyTable toinsert;
		for (size_t i = 0; i < rowQty; i++)
		{
			std::shared_ptr<ClsNode>& new_node = mClsNodeCache[i];
			{
				const std::shared_ptr<ClsRec64>& curr = mClsValCache[i];
				curr->SetId(table->GetAsString(0, i));
				table->GetAsString(1, i, curr->mTitle);
				ToClsKind(table->GetAsString(2, i), curr->mKind);
				table->GetAsString(3, i, curr->mMeasure);
			
			}

			mClsPath.GetCurrent()->AddChild(new_node);
			toinsert.emplace_back(new_node.get());
		}
		wxLogMessage(wxString::Format("%d\t %s LOAD END", GetTickCount() - p0, __FUNCTION__));
		if (!toinsert.empty())
			sigAfterInsert(*mClsPath.GetCurrent(), toinsert);

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
void ModelBrowser::DoActivate(const ClsNode& item)
{
	mClsPath.SetId(item.GetValue()->GetId());
	//DoRefresh();
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















