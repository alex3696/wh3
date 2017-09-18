#include "_pch.h"
#include "ModelBrowser.h"

using namespace wh;

class ClsRec64 : public ICls64
{
public:
	ClsRec64(){}
	ClsRec64(std::shared_ptr<const ICls64>& parent)
		:mParent(parent)
	{}

	int64_t		mId;
	wxString	mTitle;
	ClsKind		mKind;
	wxString	mMeasure;
	std::weak_ptr<const ICls64>			mParent;
	std::shared_ptr<SpClsConstTable>	mChilds;

	bool SetId(const wxString& str){ return str.ToLongLong(&mId); }
	void SetId(const int64_t& val) { mId = val; }

	virtual const int64_t&  GetId()const override		{ return mId; }
	virtual const wxString& GetTitle()const override	{ return mTitle; };
	virtual       ClsKind	GetKind()const override		{ return mKind; };
	virtual const wxString& GetMeasure()const override	{ return mMeasure; };

	virtual SpClsConst GetParent()const { return mParent.lock(); }
	virtual SpClsConstTable GetChilds()const { return *mChilds; }

	virtual const SpObjConstTable&		GetObjects()const override { throw; };
	virtual const SpPropValConstTable&	GetProperties()const override { throw; };



};

class ClsNode 
{

	struct extr_void_ptr_ClsNode
	{
		typedef const void* result_type;
		inline result_type operator()(const std::shared_ptr<const ClsNode>& r)const
		{
			return r.get();
		}
	};

	using ChildsTable = boost::multi_index_container
	<
		std::shared_ptr<ClsNode>,
		indexed_by
		<
			random_access<> //SQL order
			, ordered_unique< extr_void_ptr_ClsNode >
		>
	>;

	std::shared_ptr<const ICls64>	mValue;
	std::weak_ptr<ClsNode>			mParent;
	std::shared_ptr<ChildsTable>	mChild;

	using sigUpdate = sig::signal < void(const ClsNode&
		, std::shared_ptr<const ICls64>
		, std::shared_ptr<const ICls64>)>;
	
	sigUpdate sigBeforeValChange;
	sigUpdate sigAfterValChange;

public:
	ClsNode()
	{

	}
	ClsNode(std::shared_ptr<ClsNode>& parent)
		:mParent(parent)
	{
	}

	void SetValue(const std::shared_ptr<const ICls64>& new_value)
	{
		sigBeforeValChange(*this, mValue, new_value);
		
		auto tmp = mValue;
		mValue = new_value;

		sigBeforeValChange(*this, mValue, new_value);
	}
	std::shared_ptr<const ICls64> GetValue()const 
	{
		return mValue;
	}

	void ClearChilds()
	{
		mChild.reset();
	}

	void AddChild(const std::shared_ptr<ClsNode>& child)
	{
		if (!mChild)
			mChild = std::make_shared<ChildsTable>();

		mChild->emplace_back(child);

	}

	std::shared_ptr<const ClsNode> GetParent()const 
	{ 
		return mParent.lock(); 
	}

	void RefreshChilds(bool recursive=false)
	{
		
	}

};

class ClsTree
{
	std::shared_ptr<ClsNode> mRoot;
	std::shared_ptr<const ClsNode> mCurrent;
public:
	ClsTree()
	{
		auto value = std::make_shared<ClsRec64>();
		value->SetId(1);
		value->mTitle = "ClsRoot";
		value->mKind = ClsKind::Abstract;

		auto root = std::make_shared<ClsNode>();
		root->SetValue(value);

		mRoot = root;
	}

	void Up()
	{
		if (mCurrent->GetParent() != mRoot)
			mCurrent = mCurrent->GetParent();
		Refresh();
	}
	void Home()
	{
		mCurrent = mRoot;
		Refresh();
	}

	void Refresh()
	{
		auto id = mCurrent->GetValue()->GetIdAsString();
		
		mCurrent = mRoot;
		mRoot->ClearChilds();
		//if (1 != GetId())
		{
			wxString query = wxString::Format(
				"SELECT id, title,kind FROM public.get_path_cls_info(%s, 0)"
				, id);

			whDataMgr::GetDB().BeginTransaction();
			auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
			if (table)
			{
				

				unsigned int rowQty = table->GetRowCount();
				for (size_t i = (rowQty-1); i >= 0; i++)
				{
					auto value = std::make_shared<ClsRec64>();
					value->SetId(table->GetAsString(0, i));
					table->GetAsString(1, i, value->mTitle);
					ToClsKind(table->GetAsString(2, i),value->mKind);
					
					auto curr = std::make_shared<ClsNode>(mRoot);
					curr->SetValue(value);

					mRoot->AddChild(curr);
					mCurrent = curr;

					

				}
			}//if (table)

		}//if ("1" != id)
		sigCurrChanged(*mCurrent);
	}

	sig::signal<void(const ClsNode&)> sigCurrChanged;
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
	//sigClear();
	mClsPath.sigPathChanged.connect(std::bind(&ModelBrowser::DoRefresh, this));
	
}
//-----------------------------------------------------------------------------
ModelBrowser::~ModelBrowser()
{
	
}
//-----------------------------------------------------------------------------
void ModelBrowser::DoRefresh()
{
	const auto& parent = mClsPath.GetParents().front();

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
	


	sigPathChanged(mClsPath.AsString());
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


	



}
//-----------------------------------------------------------------------------
void ModelBrowser::DoActivate(const IIdent64* item)
{
	mClsSelected.clear();
	if (item)
	{
		if (item->GetId() == mClsAll[0]->GetId())
			DoUp();
		else
			mClsPath.SetId(item->GetId());
	}
		
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















