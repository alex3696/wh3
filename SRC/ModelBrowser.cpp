#include "_pch.h"
#include "ModelBrowser.h"

using namespace wh;

class ClsRec64 : public ICls64
{
public:
	int64_t		mId;
	wxString	mTitle;
	wxString	mKind;
	wxString	mMeasure;

	bool SetId(const wxString& str){ return str.ToLongLong(&mId); }
	void SetId(const int64_t& val) { mId = val; }

	virtual const int64_t&  GetId()const override		{ return mId; }
	virtual const wxString& GetTitle()const override	{ return mTitle; };
	virtual const wxString& GetKind()const override		{ return mKind; };
	virtual const wxString& GetMeasure()const override	{ return mMeasure; };

	virtual const SpObjConstTable&		GetObjects()const override { throw; };
	virtual const SpPropValConstTable&	GetProperties()const override { throw; };
	virtual const IClsPath64&			GetPath()const override { throw; };

};


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
				val = "Òèï:";
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

	mClsPath.sigPathChanged.connect(std::bind(&ModelBrowser::DoRefresh, this));
	sigClear();
}
//-----------------------------------------------------------------------------
ModelBrowser::~ModelBrowser()
{
	
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoRefresh()
{
	sigPathChanged(mClsPath.AsString());
	auto id = mClsPath.GetIdAsString();

	wxString query = wxString::Format(
		"SELECT id, title FROM acls WHERE pid = %s AND id>100"
		, id
		);
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);

	//sigClear();


	if (table)
	{
		size_t m = 0;

		std::function<void(std::shared_ptr<ICls64>& e)> itemChanger64
			= [&table, &m](std::shared_ptr<ICls64>& e)
		{
			auto curr = std::dynamic_pointer_cast<ClsRec64>(e);
			curr->SetId(table->GetAsString(0, m));
			table->GetAsString(1, m, curr->mTitle);
		};

		unsigned int arrQty = mClsAll.size();
		unsigned int rowQty = table->GetRowCount();
		if (rowQty < arrQty)
		{
			NotyfyTable todel;
			for (auto& it = mClsAll.begin() + rowQty; it != mClsAll.end(); ++it)
				todel.emplace_back((*it).get());
			sigBeforeDelete(todel);
			
			mClsAll.erase(mClsAll.begin() + rowQty, mClsAll.end());
			arrQty = rowQty;
		}
	
		// update
		NotyfyTable toupdate;
		for (m = 0; m < arrQty; m++)
		{
			auto it64 = mClsAll.begin() + m;
			mClsAll.modify(it64, itemChanger64);

			toupdate.emplace_back((*it64).get());
		}
		sigAfterUpdate(toupdate);

		// insert
		NotyfyTable toinsert;
		for (m = arrQty; m < rowQty; m++)
		{
			std::shared_ptr<ICls64> curr64 = std::make_shared<ClsRec64>();
			itemChanger64(curr64);
			mClsAll.emplace_back(curr64);

			toinsert.emplace_back(curr64.get());
		}
		sigAfterInsert(toinsert);


	}//if (table)
	whDataMgr::GetDB().Commit();


	//RestoreSelect();
	auto copy = mClsSelected;
	DoSelect(copy);

}
//-----------------------------------------------------------------------------
void ModelBrowser::DoActivate(const IIdent64* item)
{
	mClsSelected.clear();
	if (item)
		mClsPath.SetId(item->GetId());
	else
		mClsPath.Home();
	//DoRefresh();
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoUp()
{
	auto id = mClsPath.GetId();
	mClsSelected.clear();
	mClsSelected.emplace_back(id);

	mClsPath.Up();
	//DoRefresh();

}
//-----------------------------------------------------------------------------
void ModelBrowser::DoSelect(const NotyfyTable& sel_vec)
{
	mClsSelected.clear();
	NotyfyTable table;
	for (const auto& void_ptr : sel_vec)
	{
		const auto& ptrIdx = mClsAll.get<2>();

		const auto& it = ptrIdx.find(void_ptr);
		const auto& ss = *it;
		if (ptrIdx.cend() != it)
		{
			const std::shared_ptr<const ICls64>& sel = *it;
			table.emplace_back(sel.get());
			mClsSelected.emplace_back(sel->GetId());
		}
	}
	
	if (mClsSelected.empty() && !mClsAll.empty())
	{
		std::shared_ptr<const ICls64> sel = mClsAll.front();
		table.emplace_back(sel.get());
	}
	sigSelected(table);

}
//-----------------------------------------------------------------------------
void ModelBrowser::DoSelect(const std::vector<int64_t>& sel_vec)
{
	mClsSelected.clear();
	NotyfyTable table;

	for (const auto& ll_id : sel_vec)
	{
		const auto& idIdx = mClsAll.get<1>();
		
		const auto& it = idIdx.find(ll_id);
		const auto& ss = *it;
		if (idIdx.cend() != it)
		{
			auto sel = *it;
			table.emplace_back(sel.get());
			mClsSelected.emplace_back(ll_id);
		}
	}

	if (mClsSelected.empty() && !mClsAll.empty())
	{
		auto sel = mClsAll.front();
		table.emplace_back(sel.get());
	}

	sigSelected(table);
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















