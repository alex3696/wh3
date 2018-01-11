#include "_pch.h"
#include "ViewBrowser.h"
#include "globaldata.h"
#include "wxDataViewIconMLTextRenderer.h"
#include "wxComboBtn.h"

using namespace wh;

//-----------------------------------------------------------------------------
struct ActInfoColumn
{
	int64_t		mAid;
	//wxString	mActTitle;
	char		mAcol;
	int			mIndex;
	
	ActInfoColumn() 
		:mAid(0), mAcol(0), mIndex(0)
	{}
	ActInfoColumn(const int64_t& aid, const char acol,int idx)
		:mAid(aid), mAcol(acol), mIndex(idx)
	{}
};
//-----------------------------------------------------------------------------
using Columns = boost::multi_index_container
<
	ActInfoColumn,
	indexed_by
	<
		ordered_unique < 
							composite_key
							<
								ActInfoColumn
								, member<ActInfoColumn, int64_t, &ActInfoColumn::mAid>
								, member<ActInfoColumn, char, &ActInfoColumn::mAcol>
							> 
						>
		, ordered_unique<member<ActInfoColumn, int, &ActInfoColumn::mIndex>>
	>
>;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class wxDVTableBrowser
	: public wxDataViewModel
{
	const IIdent64* mCurrentRoot = nullptr;
	const	std::vector<const IIdent64*>* mClsList = nullptr;
	bool	mGroupByType = true;
	//wxRegEx mStartNum = "(^[0-9]+)";
	wxRegEx mStartNum = "^(-?)(0|([1-9][0-9]*))((\\.|\\,)[0-9]+)?$";

	const wxString format_d = wxLocale::GetInfo(wxLOCALE_SHORT_DATE_FMT, wxLOCALE_CAT_DATE);
	const wxString format_t = wxLocale::GetInfo(wxLOCALE_TIME_FMT);
public:
	Columns			mActColumns;

	wxDVTableBrowser(){};
	~wxDVTableBrowser(){};

	// implementation of base class virtuals to define model
	virtual unsigned int	GetColumnCount() const override
	{
		return 3;
	}
	virtual wxString		GetColumnType(unsigned int col) const override 
	{
		switch (col)
		{
		case 0: return "long";
		case 1: return "wxDataViewIconText";
		default: break;
		}
		return "string";
	}

	virtual bool IsContainer(const wxDataViewItem &item)const override
	{
		if (!item.IsOk())
			return true;
		
		const auto node = static_cast<const IIdent64*> (item.GetID());
		const auto cls = dynamic_cast<const ICls64*>(node);
		if (mGroupByType 
			&& cls 
			&& cls != mCurrentRoot
			&& ClsKind::Abstract != cls->GetKind())
			return true;
		
		return false;
	}

	void GetErrorValue(wxVariant &variant, unsigned int col) const
	{
		if(1==col)
			variant << wxDataViewIconText("*ERROR*", wxNullIcon);
		else
			variant = "*ERROR*";
		return;
	}

	void GetClsValue(wxVariant &variant, unsigned int col
		, const ICls64& cls) const
	{
		auto mgr = ResMgr::GetInstance();

		if (mCurrentRoot == &cls)
		{
			if(1==col)
				variant << wxDataViewIconText("..", mgr->m_ico_back24); 
			return;
		}
		
		switch (col)
		{
		case 0:variant = cls.GetIdAsString();	break;
		case 1: {
			const wxIcon*  ico(&wxNullIcon);
			switch (cls.GetKind())
			{
			case ClsKind::Abstract: ico = &mgr->m_ico_type_abstract24; break;
			case ClsKind::Single:	ico = &mgr->m_ico_type_num24; break;
			case ClsKind::QtyByOne:
			case ClsKind::QtyByFloat:
			default: ico = &mgr->m_ico_type_qty24;	break;
			}//switch
			if (ClsKind::Abstract == cls.GetKind() || !mGroupByType)
			{
				variant << wxDataViewIconText(cls.GetTitle(), *ico);
			}
			else
			{
				wxString qty_str = wxString::Format("%s - %s (%s)"
					, cls.GetTitle(), cls.GetObjectsQty(), cls.GetMeasure());
				variant << wxDataViewIconText(qty_str, *ico);
			}
		}break;
			
		case 2: {
			switch (cls.GetKind())
			{
			case ClsKind::Single:
			case ClsKind::QtyByOne:
			case ClsKind::QtyByFloat:
				variant = wxString::Format("%s (%s)"
					, cls.GetObjectsQty(), cls.GetMeasure());
				break;
			case ClsKind::Abstract:
			default: break;
			}
		}break;
			
		default: break;
		}//switch (col)
	}

	void GetObjValue(wxVariant &variant, unsigned int col
		, const IObj64& obj) const
	{
		const wxIcon*  ico(&wxNullIcon);
		auto mgr = ResMgr::GetInstance();

		switch (col)
		{
		case 0: variant = obj.GetIdAsString();	break;
		case 1: variant << wxDataViewIconText(obj.GetTitle(), *ico); break;
		case 2: variant = wxString::Format("%s (%s)"
			, obj.GetQty()
			, obj.GetCls()->GetMeasure());
			break;
		case 3: variant = obj.GetCls()->GetTitle();	break;
		case 4: variant = obj.GetPath()->AsString(); break;

		default:{
			const auto& idxCol = mActColumns.get<1>();
			const auto it = idxCol.find(col);
			if (idxCol.end() != it)
			{
				wxLongLong ll(it->mAid);
				std::wstring ss = ll.ToString() << L"." << (int)it->mAcol;
				std::wstring act_val = obj.GetActInfo().get<std::wstring>(ss, L"");
				if (!act_val.empty())
				{
					if (0 == act_val.compare(L"null"))
						variant = wxString("--");
					else
					{
						switch (it->mAcol)
						{
						case 1: case 4: {
							wxDateTime dt;
							dt.ParseISOCombined(act_val);
							if (dt.IsValid())
							{
								wxString str = wxString::Format("%s %s", dt.Format(format_d), dt.Format(format_t));
								variant = wxString::Format("%s %s", dt.Format(format_d), dt.Format(format_t));
							}
						} break;
						case 2: {
							if (2 == it->mAcol)
							{
								wxString period;
								if (obj.GetActPeriod(it->mAid, period))
								{
									double dperiod;
									if (period.ToCDouble(&dperiod))
									{
										variant = wxString::Format("%g", dperiod / 86400);
										//variant = dperiod / 86400;
									}
								}
							}
							else
								variant = wxString(act_val);
						} break;
						case 8: {
							wxString str(act_val);
							variant = str;
						}break;
						default:
							variant = "*error*";
							break;
						}//switch (it->mAcol)

							
					}
				}//if (!act_val.empty())
			}

		}break;
			
			
		}
	}

	virtual void GetValue(wxVariant &variant,
		const wxDataViewItem &dvitem, unsigned int col) const override
	{
		const auto node = static_cast<const IIdent64*> (dvitem.GetID());
		const auto ident = node;
		if (!ident)
		{
			GetErrorValue(variant, col);
			return;
		}
		
		const auto cls = dynamic_cast<const ICls64*>(ident);
		if (cls)
		{
			GetClsValue(variant, col, *cls);
			return;
		}
		const auto obj = dynamic_cast<const IObj64*>(ident);
		if (obj)
		{
			GetObjValue(variant, col, *obj);
			return;
		}
		
	}
	virtual bool GetAttr(const wxDataViewItem &item, unsigned int col,
		wxDataViewItemAttr &attr) const override
	{
		if (item.IsOk())
		{
			const auto node = static_cast<const IIdent64*> (item.GetID());
			const auto obj = dynamic_cast<const IObj64*>(node);
			if (obj && (1 == col || (2 == col && ClsKind::Single != obj->GetCls()->GetKind())))
			{
				attr.SetBold(true);
				return true;
			}
			const auto& idxCol = mActColumns.get<1>();
			const auto it = idxCol.find(col);
			if (idxCol.end() != it)
			{
				if (it->mAcol & 8)
				{
					wxVariant val;
					GetValue(val,item, col);
					if (val.IsType("string"))
					{
						auto left_str = val.GetString();
						double left;
						if (!left_str.empty())
						{
							if (left_str.ToCDouble(&left))
							{
								if (left< 10)
								{
									attr.SetBold(true);
									if (left<0)
									{
										//attr.SetBackgroundColour(wxColour(255, 200, 200));
										attr.SetColour(*wxRED);
									}
									else
									{
										//attr.SetBackgroundColour(*wxYELLOW);
										attr.SetColour(wxColour(230, 130, 30));
									}//else if (left<0)
									return true;
								}//if (left< 10)
							}
							else
								if (left_str == "--")
								{
									attr.SetBold(true); 
									attr.SetBackgroundColour(wxColour(255, 200, 200));
									attr.SetColour(*wxRED);
									return true;
								}
						}//if (!left_str.empty())
					}//if (val.IsType("string"))
					else
					{
						//wxVariant val;
						//GetValue(val, item, col);
						attr.SetBackgroundColour(*wxLIGHT_GREY);
						return true;
					}
					/*
					wxDateTime previos_dt;
					if (obj->GetActPrevios(it->mAid, previos_dt))
					{
						wxString period;
						obj->GetActPeriod(it->mAid, period);
						double dperiod;
						if (period.ToDouble(&dperiod))
						{
							wxTimeSpan ts_period(0, 0, floor(dperiod), (dperiod-floor(dperiod))*1000 );
							
							wxTimeSpan left = previos_dt + ts_period - wxDateTime::Now();
							if (left.GetDays() < 10)
							{
								attr.SetBold(true);
								if (left.IsNegative())
									//attr.SetBackgroundColour(wxColour(255, 150, 150));
									attr.SetColour(*wxRED);
								else
									//attr.SetBackgroundColour(*wxYELLOW);
									attr.SetColour(wxColour(255, 128, 0));
							}
						}//if (period.ToDouble(&dperiod))
					}//if (obj->GetActPrevios(it->mAid, dt))
					*/
				}//if (it->mAcol & 8)

			}//if (idxCol.end() != it)

		}
		return false;
	}
	virtual bool SetValue(const wxVariant &variant, const wxDataViewItem &item,
		unsigned int col)override
	{
		return false;
	}
	
	virtual int Compare(const wxDataViewItem &item1, const wxDataViewItem &item2
		, unsigned int column, bool ascending) const override
	{
		wxVariant value1, value2;
		GetValue(value1, item1, column);
		GetValue(value2, item2, column);

		if (mCurrentRoot)
		{
			if (static_cast<const IIdent64*> (item1.GetID()) == mCurrentRoot)
				return -1;
			else if (static_cast<const IIdent64*> (item2.GetID()) == mCurrentRoot)
				return 1;
		}

		if (!ascending)
			std::swap(value1, value2);

		if (value1.IsNull())
			if (!value2.IsNull())
				return -1;
		else
			if (!value2.IsNull())
				return 1;

		// all columns sorted by TRY FIRST numberic value
		if (value1.GetType() == "string" || value1.GetType() == "wxDataViewIconText")
		{
			wxString str1;
			wxString str2;

			if (value1.GetType() == "wxDataViewIconText")
			{
				wxDataViewIconText iconText1, iconText2;

				iconText1 << value1;
				iconText2 << value2;
				str1 = iconText1.GetText();
				str2 = iconText2.GetText();
			}
			else
			{
				str1 = value1.GetString();
				str2 = value2.GetString();
			}
			//wxRegEx mStartNum = "(^[0-9]+)";
			if (mStartNum.Matches(str1))
			{
				size_t start1;
				size_t len1;
				double num1;
				bool match = mStartNum.GetMatch(&start1, &len1);
				if (match && str1.substr(start1, len1).ToCDouble(&num1))
				{
					if (mStartNum.Matches(str2))
					{
						size_t start2;
						size_t len2;
						double num2;
						match = mStartNum.GetMatch(&start2, &len2);
						if (match && str2.substr(start2, len2).ToCDouble(&num2))
						{
							
							if (num1 < num2)
								return -1;
							else if (num1 > num2)
								return 1;
							str1.erase(start1, start1 + len1);
							str2.erase(start2, start2 + len2);
						}
					}//if (mStartNum.Matches(str1))
				}//if (match && str1.substr(start, len).ToLong(&num1))
			}//if (mStartNum.Matches(str1))

			int res = str1.CmpNoCase(str2);
			if (res)
				return res;

			// items must be different
			wxUIntPtr id1 = wxPtrToUInt(item1.GetID()),
				id2 = wxPtrToUInt(item2.GetID());
			return ascending ? id1 - id2 : id2 - id1;
		}
		
		return wxDataViewModel::Compare(item1, item2, column, ascending);
	}

	virtual wxDataViewItem GetParent(const wxDataViewItem &item) const override
	{
		if (!item.IsOk())
			return wxDataViewItem(nullptr);

		const auto ident = static_cast<const IIdent64*> (item.GetID());
		const auto cls = dynamic_cast<const ICls64*>(ident);
		if (cls)
			return wxDataViewItem(nullptr);

		const auto& obj = dynamic_cast<const IObj64*>(ident);
		if (obj)
			return wxDataViewItem((void*)obj->GetCls().get());

		return wxDataViewItem(nullptr);
	}

	virtual unsigned int GetChildren(const wxDataViewItem &parent
		, wxDataViewItemArray &arr) const override
	{
		if (!parent.IsOk() && mClsList)
		{
			if (mCurrentRoot && 1 < mCurrentRoot->GetId())
			{
				wxDataViewItem dvitem((void*)mCurrentRoot);
				arr.push_back(dvitem);
			}
			for (const auto& child_cls : *mClsList)
			{
				wxDataViewItem dvitem((void*)child_cls);
				arr.push_back(dvitem);
			}
			return arr.size();
		}
		const auto ident = static_cast<const IIdent64*> (parent.GetID());
		const auto cls = dynamic_cast<const ICls64*>(ident);
		if (mGroupByType && cls)
		{
			const auto obj_list = cls->GetObjTable();
			if (obj_list && obj_list->size())
			{
				for (const auto& sp_obj : *obj_list)
				{
					wxDataViewItem dvitem((void*)sp_obj.get());
					arr.push_back(dvitem);
				}
				return arr.size();
			}
		}


		return 0;
	}

	void SetGroupByType(bool group)
	{
		mGroupByType = group;
		Cleared();
	}

	virtual bool  IsListModel() const override
	{
		return !mGroupByType;
	}

	void SetClsList(const std::vector<const IIdent64*>* current, const IIdent64* curr = nullptr)
	{
		mCurrentRoot = curr;
		mClsList = current;
		Cleared();
	}

	const IIdent64* GetCurrentRoot()const
	{
		return mCurrentRoot;
	}


};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class wxAuiPanel : public wxPanel
{
public:
	wxAuiPanel(wxWindow* wnd)
		:wxPanel(wnd)
	{
		mAuiMgr.SetManagedWindow(this);
	}
	~wxAuiPanel()
	{
		mAuiMgr.UnInit();
	}
	wxAuiManager	mAuiMgr;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewTableBrowser::ViewTableBrowser(const std::shared_ptr<IViewWindow>& parent)
	:ViewTableBrowser(parent->GetWnd())
{}
//-----------------------------------------------------------------------------
ViewTableBrowser::ViewTableBrowser(wxWindow* parent)
{
	auto table = new wxDataViewCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize
		, wxDV_ROW_LINES 
		 | wxDV_VERT_RULES 
		 //| wxDV_HORIZ_RULES
		 | wxDV_MULTIPLE
		);
	mTable = table;

	auto dv_model = new wxDVTableBrowser();
	table->AssociateModel(dv_model);
	dv_model->DecRef();

	int ch = table->GetCharHeight();
	table->SetRowHeight(ch * 1.6 );


	//int ch = table->GetCharHeight();
	//table->SetRowHeight(ch * 4 + 2);

	ResetColumns();


	//table->SetExpanderColumn(col1);

	table->GetTargetWindow()->SetToolTip("ToolTip");
	table->GetTargetWindow()->Bind(wxEVT_MOTION, &ViewTableBrowser::OnCmd_MouseMove, this);

	table->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED
		, [this](wxDataViewEvent& evt) 
		{ 
			if (evt.GetItem().IsOk())
				StoreSelect();
			else
				RestoreSelect();
		});
	table->Bind(wxEVT_DATAVIEW_COLUMN_SORTED
		, [this](wxDataViewEvent& evt) { RestoreSelect(); });


	table->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED
		, &ViewTableBrowser::OnCmd_Activate, this);

	table->Bind(wxEVT_DATAVIEW_ITEM_EXPANDING
		, &ViewTableBrowser::OnCmd_Expanding, this);
	table->Bind(wxEVT_DATAVIEW_ITEM_EXPANDED
		, &ViewTableBrowser::OnCmd_Expanded, this);

	table->Bind(wxEVT_DATAVIEW_ITEM_COLLAPSED
		, &ViewTableBrowser::OnCmd_Collapseded, this);

	table->Bind(wxEVT_COMMAND_MENU_SELECTED
		,&ViewTableBrowser::OnCmd_Refresh, this, wxID_REFRESH);

	table->Bind(wxEVT_COMMAND_MENU_SELECTED
		, &ViewTableBrowser::OnCmd_Up, this, wxID_UP);






	wxAcceleratorEntry entries[3];
	entries[0].Set(wxACCEL_CTRL, (int) 'R', wxID_REFRESH);
	entries[1].Set(wxACCEL_NORMAL, WXK_F5,   wxID_REFRESH);
	entries[2].Set(wxACCEL_NORMAL, WXK_BACK, wxID_UP);
	wxAcceleratorTable accel(3, entries);
	table->SetAcceleratorTable(accel);

}
//-----------------------------------------------------------------------------
void ViewTableBrowser::OnCmd_Refresh(wxCommandEvent& evt)
{
	TEST_FUNC_TIME;
	sigRefresh();
}
//-----------------------------------------------------------------------------
void ViewTableBrowser::OnCmd_Up(wxCommandEvent& evt)
{
	TEST_FUNC_TIME;
	sigUp();
}
//-----------------------------------------------------------------------------
void ViewTableBrowser::OnCmd_MouseMove(wxMouseEvent& evt)
{
	wxDataViewColumn* col = nullptr;
	wxDataViewItem item(nullptr);
	auto pos = evt.GetPosition();
	mTable->HitTest(pos, item, col);

	wxString str;

	if (col && item.IsOk())
	{
		wxVariant var;
		//mTable->GetModel()->GetValue(var, item, col->GetModelColumn());
		//wxDataViewIconText2 ico_txt;
		//ico_txt << var;
		//str = ico_txt.GetText();

		mTable->GetModel()->GetValue(var, item, 0);
		str = var.GetString();
		mTable->GetTargetWindow()->GetToolTip()->SetTip(str);
	}
	
}
//-----------------------------------------------------------------------------
void ViewTableBrowser::OnCmd_Activate(wxDataViewEvent& evt)
{
	TEST_FUNC_TIME;

	auto item = evt.GetItem();

	if (mTable->GetModel()->IsContainer(item))
	{
		if (mTable->IsExpanded(item))
			mTable->Collapse(item);
		else
			mTable->Expand(item);
	}
	else
	{
		const IIdent64* node = static_cast<const IIdent64*> (item.GetID());
		const ICls64* cls = dynamic_cast<const ICls64*> (node);
		if (cls)
		{
			wxBusyCursor busyCursor;
			
			if(cls->GetId() == mParentCid)
				sigUp();
			else
			{
				// если убрать этот локер, выделится первый элемент,
				// даже в обратной сортировке
				wxWindowUpdateLocker lock(mTable);
				sigActivate(cls->GetId());
			}
				
		}
			
	}

}
//-----------------------------------------------------------------------------
void ViewTableBrowser::OnCmd_Expanding(wxDataViewEvent& evt)
{
	TEST_FUNC_TIME;

	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel || mTable->GetModel()->IsListModel())
		return;

	auto item = evt.GetItem();

	const IIdent64* node = static_cast<const IIdent64*> (item.GetID());
	const ICls64* cls = dynamic_cast<const ICls64*> (node);
	if (cls)
	{
		mSortCol = mTable->GetSortingColumn();
		if (mSortCol)
		{
			mSortAsc = mSortCol->IsSortOrderAscending();
			mSortCol->UnsetAsSortKey();
		}

		wxBusyCursor busyCursor;
		wxWindowUpdateLocker lock(mTable);

		//sigActivate(cls->GetId());
		sigRefreshClsObjects(cls->GetId());
		const std::shared_ptr<const ICls64::ObjTable> ot = cls->GetObjTable();
		if (ot && ot->size())
		{
			std::vector<const IIdent64*> vec;
			for (const auto& obj : *ot)
				vec.emplace_back(obj.get());
			RebuildClsColumns(vec);
		}

	}

}
//-----------------------------------------------------------------------------
void ViewTableBrowser::OnCmd_Expanded(wxDataViewEvent& evt)
{
	TEST_FUNC_TIME;
	if (mSortCol)
	{
	
		mSortCol->SetSortable(true);
		mSortCol->SetSortOrder(mSortAsc);

		wxBusyCursor busyCursor;
		wxWindowUpdateLocker lock(mTable);

		mTable->GetModel()->Resort();
	}

	AutosizeColumns();

	auto item = evt.GetItem();
	const IIdent64* node = static_cast<const IIdent64*> (item.GetID());
	const ICls64* cls = dynamic_cast<const ICls64*> (node);
	if (cls)
		mExpandedCls.insert(cls->GetId());
	
}
//-----------------------------------------------------------------------------
void ViewTableBrowser::OnCmd_Collapseded(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	if (item.IsOk())
	{
		auto model = mTable->GetModel();
		auto dvparent = evt.GetItem();
		wxDataViewItemArray arr;
		model->GetChildren(dvparent, arr);
		model->ItemsDeleted(dvparent, arr);

		const IIdent64* node = static_cast<const IIdent64*> (item.GetID());
		const ICls64* cls = dynamic_cast<const ICls64*> (node);
		if (cls)
			mExpandedCls.erase(cls->GetId());
	}

}

//-----------------------------------------------------------------------------
const IIdent64* ViewTableBrowser::FindChildCls(const int64_t& id)const
{
	//wxDataViewItem dvparent(nullptr);
	//auto model = mTable->GetModel();
	//wxDataViewItemArray arr;
	//model->GetChildren(dvparent, arr);

	//for (size_t i = 0; i < arr.size() ; i++)
	//{
	//	const auto ident = static_cast<const IIdent64*> (arr[i].GetID());
	//	const auto cls = dynamic_cast<const ICls64*>(ident);
	//	if (cls && cls->GetId() == id)
	//		return cls;
	//}

	if (mClsList.empty())
		return nullptr;
	auto it = std::find_if(mClsList.cbegin(), mClsList.cend()
		, [&id](const IIdent64* it)
		{ 
			return it->GetId() == id; 
		});

	if (mClsList.cend() != it)
		return (*it);
	
	return nullptr;
}
//-----------------------------------------------------------------------------
const IIdent64* ViewTableBrowser::GetTopChildCls()const
{
	//wxDataViewItem dvparent(nullptr);
	//auto model = mTable->GetModel();
	//wxDataViewItemArray arr;
	//model->GetChildren(dvparent, arr);

	//const auto ident = static_cast<const IIdent64*> (arr[0].GetID());
	//const auto cls = dynamic_cast<const ICls64*>(ident);
	//if (cls)
	//	return cls;
	//return nullptr;

	if (1 < mParentCid)	//(mParent && 1 != mParent->GetId())
	{
		auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
		if (dvmodel)
			return dvmodel->GetCurrentRoot();
	}
		
	if (!mClsList.empty())
		return mClsList.front();
	return nullptr;
}
//-----------------------------------------------------------------------------
void ViewTableBrowser::StoreSelect()
{
	mClsSelected = 0;
	mObjSelected = 0;

	const IIdent64* ident = static_cast<const IIdent64*> (mTable->GetCurrentItem().GetID());
	if (ident)
	{
		const auto& obj = dynamic_cast<const IObj64*>(ident);
		if (obj)
		{
			mObjSelected = obj->GetId();
			mClsSelected = obj->GetCls()->GetId();
		}
		else
		{
			const auto& cls = dynamic_cast<const ICls64*>(ident);
			if (cls)
				mClsSelected = cls->GetId();
		}
	}//if (ident)

}

//-----------------------------------------------------------------------------
void ViewTableBrowser::RestoreSelect()
{
	wxDataViewItem dvitem;
	if(mClsSelected)
	{
		const auto finded = FindChildCls(mClsSelected);
		if (finded)
		{
			//auto id = finded->GetIdAsString();
			//auto title = finded->GetTitle();
			dvitem = wxDataViewItem((void*)finded);

			if (!mTable->GetModel()->IsListModel() && mObjSelected)
			{
				//sigActivate(finded);
				mTable->Expand(dvitem);// finded cls
				const auto& cls = dynamic_cast<const ICls64*>(finded);
				const auto objTable = cls->GetObjTable();
				auto it = std::find_if(objTable->cbegin(), objTable->cend()
					, [this](const std::shared_ptr<const IObj64>& it)
				{
					return it->GetId() == mObjSelected;
				});

				if (objTable->cend() != it)
					dvitem = wxDataViewItem((void*)(it->get()));
			}
		}//if (finded)


	}

	if (!dvitem.IsOk())
	{
		const auto finded = GetTopChildCls();
		if (finded)
			dvitem = wxDataViewItem((void*)finded);
	}

	auto model = mTable->GetModel();
	
	mTable->UnselectAll();

	if (dvitem.IsOk())
	{
		mTable->EnsureVisible(dvitem);
		mTable->SetCurrentItem(dvitem);
		mTable->Select(dvitem);
	}

	//auto new_sel = mTable->GetCurrentItem();
	//const auto ident = static_cast<const IIdent64*> (new_sel.GetID());
	//const auto cls = dynamic_cast<const ICls64*>(ident);
	//if (cls)
	//{
	//	auto title = cls->GetTitle();
	//	auto id = cls->GetIdAsString();
	//}
	
}
//-----------------------------------------------------------------------------
void ViewTableBrowser::AutosizeColumns()
{
	TEST_FUNC_TIME;
	wxBusyCursor busyCursor;

	if (mColAutosize)
	{
		for (size_t i = 0; i < mTable->GetColumnCount(); i++)
		{
			auto col = mTable->GetColumn(i);
			if(col && "datetime"!=col->GetRenderer()->GetVariantType())
				col->SetWidth(mTable->GetBestColumnWidth(i));
		}
			
	}
}
//-----------------------------------------------------------------------------
void ViewTableBrowser::ResetColumns()
{
	wxWindowUpdateLocker lock(mTable);

	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return;

	auto table = mTable;

	if (dvmodel->mActColumns.empty() && 4 == table->GetColumnCount() )
		return;

	table->ClearColumns();
	dvmodel->mActColumns.clear();

	auto renderer1 = new wxDataViewIconTextRenderer();
	auto attr = renderer1->GetAttr();
	attr.SetColour(*wxBLACK);
	renderer1->SetAttr(attr);

	auto renderer2 = new wxDataViewTextRenderer();

	//table->AppendTextColumn("#", 0,wxDATAVIEW_CELL_INERT,-1, wxALIGN_LEFT);

	auto col1 = new wxDataViewColumn("Имя"
		, renderer1, 1, 150, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	table->AppendColumn(col1);
	//col1->SetSortOrder(true);

	auto col3 = table->AppendTextColumn("Тип", 3, wxDATAVIEW_CELL_INERT, -1
		, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	col3->GetRenderer()->EnableEllipsize(wxELLIPSIZE_START);

	auto col2 = new wxDataViewColumn("Количество"
		, renderer2, 2, 150, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	table->AppendColumn(col2);

	auto col4 = table->AppendTextColumn("Местоположение", 4, wxDATAVIEW_CELL_INERT, -1
		, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	col4->GetRenderer()->EnableEllipsize(wxELLIPSIZE_START);
	

}
//-----------------------------------------------------------------------------
void ViewTableBrowser::RebuildClsColumns(const std::vector<const IIdent64*>& vec)
{
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return;

	for (const auto& ident : vec)
	{
		const auto obj = dynamic_cast<const IObj64*>(ident);
		if (!obj)
			continue;

		std::vector<const IAct64*> fav_act;
		obj->GetCls()->GetFavActs(fav_act);
		for (const auto& fa_it : fav_act)
		{
			auto aid = fa_it->GetId();
			char visible;
			if (obj->GetCls()->GetActVisible(aid, visible))
			{
				for (char v = 1; v <= 8; v<<=1)
				{
					if (visible & v)
					{
						auto& idx0 = dvmodel->mActColumns.get<0>();
						auto it = idx0.find(boost::make_tuple(aid, (char)(visible & v)));
						if (idx0.end() == it)
						{
							ActInfoColumn acol(aid
								, (char)(visible & v)
								, mTable->GetColumnCount() + 1);

							dvmodel->mActColumns.emplace(acol);

							wxString str;
							switch (acol.mAcol)
							{
							case 2: str="период(сут.)"; break;
							case 4: str = "след."; break;
							case 8: str = "осталось(сут.)"; break;
							default:str = "пред."; break;
							}

							wxString title;
							title << fa_it->GetTitle() << ":" << str;
							this->mTable->AppendTextColumn(title, acol.mIndex
								, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT
								, /*wxCOL_REORDERABLE | */ wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
						}
					}
				}
				

			}
		}


		/*
		auto begin = obj->GetActInfo().begin();
		auto end = obj->GetActInfo().end();
		while (begin != end)
		{
			auto act_col_begin = begin->second.begin();
			auto act_col_end = begin->second.end();
			wxString str_aid = begin->first.data();
			int64_t aid;
			if (str_aid.ToLongLong(&aid))
			{
				while (act_col_begin != act_col_end)
				{
					wxString str_ainfo = act_col_begin->first.data();
					long ainfo;
					if (str_ainfo.ToLong(&ainfo))
					{
						auto& idx0 = dvmodel->mActColumns.get<0>();
						auto it = idx0.find(boost::make_tuple(aid, (char)ainfo));
						if (idx0.end() == it)
						{
							ActInfoColumn acol(aid
								, (char)ainfo
								, mTable->GetColumnCount() + 1);

							dvmodel->mActColumns.emplace(acol);

							wxString str;
							str << acol.mAid << ":" << (int)acol.mAcol;
							this->mTable->AppendTextColumn(str, acol.mIndex
								, wxDATAVIEW_CELL_INERT, -1
								, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
						}

					}//if (str_ainfo.ToLong(&ainfo))
					++act_col_begin;
				}
			}//if (str_aid.ToLongLong(&aid))
			++begin;
		}//while (begin != end)
		*/
	}//for (const auto& obj : *ot)
	
}

//-----------------------------------------------------------------------------
//virtual 
void ViewTableBrowser::SetBeforeRefreshCls(const std::vector<const IIdent64*>& 
											, const IIdent64* parent
											, const wxString&
											) //override;
{
	TEST_FUNC_TIME;

	mClsSelected = 0;
	mObjSelected = 0;
	if (parent && mParentCid)
	{
		auto curr_id = mParentCid;
		auto  new_id = parent->GetId();
		if(curr_id== new_id)
			StoreSelect();
		else
			mClsSelected = curr_id;
	}
	else if(parent)
	{
		mClsSelected = parent->GetId();
	}
		
	ResetColumns();
	mClsList.clear();
	mParentCid = 0;
	//mParent = nullptr;
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (dvmodel)
	{
		dvmodel->SetClsList(nullptr, nullptr);
	}
	
	
	//wxBusyCursor busyCursor;
	//wxWindowUpdateLocker lock(mTable);

	//OnCmd_Select(wxDataViewEvent());
}
//-----------------------------------------------------------------------------
//virtual 
void ViewTableBrowser::SetAfterRefreshCls(const std::vector<const IIdent64*>& vec
											, const IIdent64* root
											, const wxString& ss) //override;
{
	TEST_FUNC_TIME;
	wxBusyCursor busyCursor;
	{
		wxWindowUpdateLocker lock(mTable);

		auto col3 = mTable->GetColumn(1);
		if (root)
		{
			// and CLASS catalog 
			col3->SetHidden(true);
		}
		else
		{
			col3->SetHidden(!mTable->GetModel()->IsListModel());
		}
			
		
		mParentCid = root ? root->GetId() : 0;
		//mParent = root;
		mClsList = vec;
		auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
		if (dvmodel)
		{
			dvmodel->SetClsList(&mClsList, (root && 1 != root->GetId()) ? root : nullptr);

		}

		if (!dvmodel->IsListModel())
		{
			for (const auto& cid : mExpandedCls)
			{
				auto ident = FindChildCls(cid);
				if (ident)
				{
					wxDataViewItem item((void*)ident);
					mTable->Expand(item);
				}
			}//for (const auto& cid : mExpandedCls)
		}
		else
		{
			RebuildClsColumns(vec);
		}



	}
	
	RestoreSelect();
	AutosizeColumns();

}

//-----------------------------------------------------------------------------
//virtual
void ViewTableBrowser::SetGroupByType(bool enable)// override;
{
	TEST_FUNC_TIME;
	wxBusyCursor busyCursor;

	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return;

	{
		wxWindowUpdateLocker lock(mTable);
		dvmodel->SetGroupByType(enable);
	}
		
	AutosizeColumns();
	RestoreSelect();

}
//-----------------------------------------------------------------------------
//virtual
void ViewTableBrowser::SetObjOperation(Operation op, const std::vector<const IIdent64*>& obj_list)// override;
{
	TEST_FUNC_TIME;


	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel || mTable->GetModel()->IsListModel())
		return;

	wxBusyCursor busyCursor;
	wxWindowUpdateLocker lock(mTable);

	switch (op)
	{
	case Operation::BeforeInsert:	break;
	case Operation::AfterInsert:	
		// тормозит при добавлении большого количества элементов
		//for (const auto& item : obj_list)
		//{
		//	wxDataViewItem cls((void*)item->GetCls().get());
		//	wxDataViewItem obj((void*)item);
		//	dvmodel->ItemAdded(cls, obj);
		//}
		RebuildClsColumns(obj_list);
		break;
	case Operation::BeforeUpdate:	break;
	case Operation::AfterUpdate:	
		for (const auto& item : obj_list)
		{
			//wxDataViewItem cls((void*)item->GetCls().get());
			wxDataViewItem obj((void*)item);
			dvmodel->ItemChanged(obj);
		}
		break;
	case Operation::BeforeDelete:	
		for (const auto& item : obj_list)
		{
			const auto obj = dynamic_cast<const IObj64*>(item);
			wxDataViewItem item_cls((void*)obj->GetCls().get());
			wxDataViewItem item_obj((void*)item);
			dvmodel->ItemDeleted(item_cls, item_obj);




			

		}
		break;
	case Operation::AfterDelete:	break;
	default:	break;
	}

	
	AutosizeColumns();

	
}












//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewToolbarBrowser::ViewToolbarBrowser(const std::shared_ptr<IViewWindow>& parent)
	:ViewToolbarBrowser(parent->GetWnd())
{}
//-----------------------------------------------------------------------------
ViewToolbarBrowser::ViewToolbarBrowser(wxWindow* parent)
{
	auto mgr = ResMgr::GetInstance();

	long style = wxAUI_TB_DEFAULT_STYLE 
		| wxAUI_TB_PLAIN_BACKGROUND
		| wxAUI_TB_TEXT
		//| wxAUI_TB_HORZ_TEXT
		//| wxAUI_TB_OVERFLOW
		;
	auto tool_bar = new wxAuiToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);

	tool_bar->AddTool(wxID_REFRESH, "Обновить", mgr->m_ico_refresh24, "Обновить(CTRL+R)");
	//tool_bar->AddTool(wxID_UP,"Вверх", mgr->m_ico_back24, "Вверх(BACKSPACE)");

	tool_bar->AddTool(wxID_EXECUTE, "Выполнить", mgr->m_ico_act24, "Выполнить(F9)");
	tool_bar->AddTool(wxID_REPLACE, "Переместить", mgr->m_ico_move24, "Переместить(F6)");

	tool_bar->AddTool(wxID_NEW_TYPE, "Создать тип", mgr->m_ico_add_type24, "Создать тип(CTRL+T)");
	tool_bar->AddTool(wxID_NEW_OBJECT, "Создать объект", mgr->m_ico_add_obj_tab24, "Создать объект(CTRL+O)");
	tool_bar->AddTool(wxID_DELETE, "Удалить", mgr->m_ico_delete24, "Удалить(DELETE)");
	tool_bar->AddTool(wxID_EDIT, "Редактировать", mgr->m_ico_edit24, "Редактировать(CTRL+E)");

	tool_bar->AddTool(wxID_VIEW_LIST , "Группировать по типу", wxArtProvider::GetBitmap(wxART_LIST_VIEW, wxART_TOOLBAR), "Группировать по типу(CTRL+G)");
	tool_bar->FindTool(wxID_VIEW_LIST)->SetState(wxAUI_BUTTON_STATE_CHECKED);

	tool_bar->AddSeparator();


	//auto mFindCtrl = new wxComboBtn(tool_bar, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	//wxBitmap bmp(wxArtProvider::GetBitmap(wxART_FIND, wxART_MENU));
	//mFindCtrl->SetButtonBitmaps(bmp, true);

	////mFindCtrl->Bind(wxEVT_COMMAND_TEXT_ENTER, fn);
	//tool_bar->AddControl(mFindCtrl, "Поиск");


	tool_bar->Realize();

	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarBrowser::OnCmd_Refresh, this, wxID_REFRESH);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarBrowser::OnCmd_Up, this, wxID_UP);

	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarBrowser::OnCmd_Act, this, wxID_EXECUTE);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarBrowser::OnCmd_Move, this, wxID_REPLACE);
	
	
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarBrowser::OnCmd_AddType, this, wxID_NEW_TYPE);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarBrowser::OnCmd_AddObject, this, wxID_NEW_OBJECT);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarBrowser::OnCmd_DeleteSelected, this, wxID_DELETE);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarBrowser::OnCmd_UpdateSelected, this, wxID_EDIT);
	
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarBrowser::OnCmd_GroupByType, this, wxID_VIEW_LIST);

	mToolbar = tool_bar;

}
//-----------------------------------------------------------------------------
//virtual 
void ViewToolbarBrowser::SetVisibleFilters(bool enable)// override;
{

}
//-----------------------------------------------------------------------------
//virtual 
void ViewToolbarBrowser::SetGroupByType(bool enable)// override;
{
	int show;
	show = enable ? wxAUI_BUTTON_STATE_CHECKED : wxAUI_BUTTON_STATE_NORMAL;
	mToolbar->FindTool(wxID_VIEW_LIST)->SetState(show);

}
//-----------------------------------------------------------------------------
//virtual 
void ViewToolbarBrowser::OnCmd_Refresh(wxCommandEvent& evt)
{
	sigRefresh();
}
//-----------------------------------------------------------------------------
//virtual 
void ViewToolbarBrowser::OnCmd_Up(wxCommandEvent& evt)
{
	sigUp();
}
//-----------------------------------------------------------------------------
//virtual 
void ViewToolbarBrowser::OnCmd_Act(wxCommandEvent& evt)
{}
//-----------------------------------------------------------------------------
//virtual 
void ViewToolbarBrowser::OnCmd_Move(wxCommandEvent& evt)
{}
//-----------------------------------------------------------------------------
//virtual 
void ViewToolbarBrowser::OnCmd_AddType(wxCommandEvent& evt )
{}
//-----------------------------------------------------------------------------
//virtual 
void ViewToolbarBrowser::OnCmd_AddObject(wxCommandEvent& evt )
{}
//-----------------------------------------------------------------------------
//virtual 
void ViewToolbarBrowser::OnCmd_DeleteSelected(wxCommandEvent& evt )
{}
//-----------------------------------------------------------------------------
//virtual 
void ViewToolbarBrowser::OnCmd_UpdateSelected(wxCommandEvent& evt )
{}
//-----------------------------------------------------------------------------
//virtual 
void ViewToolbarBrowser::OnCmd_GroupByType(wxCommandEvent& evt)
{
	int state = mToolbar->FindTool(wxID_VIEW_LIST)->GetState();
	int enable = state & wxAUI_BUTTON_STATE_CHECKED;
	sigGroupByType(enable ? false : true);
}
//-----------------------------------------------------------------------------
//virtual 
void ViewToolbarBrowser::OnCmd_CollapseGroupByType(wxCommandEvent& evt )
{}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewPathBrowser::ViewPathBrowser(const std::shared_ptr<IViewWindow>& parent)
	:ViewPathBrowser(parent->GetWnd())
{
}
//-----------------------------------------------------------------------------
ViewPathBrowser::ViewPathBrowser(wxWindow* parent)
{
	mPathCtrl = new wxTextCtrl(parent, wxID_ANY, wxEmptyString
		, wxDefaultPosition, wxDefaultSize, 0 | wxTE_READONLY /*| wxNO_BORDER*/);

	//auto window_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
	//mPathCtrl->SetBackgroundColour(window_colour);
}
//-----------------------------------------------------------------------------
//virtual 
void ViewPathBrowser::SetPathMode(const int mode)// override;
{
}
//-----------------------------------------------------------------------------
//virtual 
void ViewPathBrowser::SetPathString(const ICls64& node)// override;
{
	wxString ret = "/";
	const auto* curr = &node;
	while (curr != nullptr && curr->GetId()!=1)
	{
		const auto& title = curr->GetTitle();

		if (wxNOT_FOUND == title.Find('/'))
			ret = wxString::Format("/%s%s", title, ret);
		else
			ret = wxString::Format("/[%s]%s", title, ret);

		curr = curr->GetParent().get();
	}

	mPathCtrl->SetValue(ret);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewBrowserPage::ViewBrowserPage(const std::shared_ptr<IViewWindow>& parent)
	:ViewBrowserPage(parent->GetWnd())
{
}
//-----------------------------------------------------------------------------
ViewBrowserPage::ViewBrowserPage(wxWindow* parent)
{
	auto mgr = ResMgr::GetInstance();

	auto panel = new wxAuiPanel(parent);
	mAuiMgr = &panel->mAuiMgr;
	mPanel = panel;

	auto window_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
	auto face_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
	panel->mAuiMgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, face_colour);
	panel->mAuiMgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_SASH_SIZE,5);
	panel->mAuiMgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);

	

	mViewToolbarBrowser = std::make_shared<ViewToolbarBrowser>(panel);
	panel->mAuiMgr.AddPane(mViewToolbarBrowser->GetWnd(), wxAuiPaneInfo().Name(wxT("BrowserToolBar"))
		.CaptionVisible(false)
		.Top().Dock().Fixed().Floatable(false).Row(0).Layer(0).ToolbarPane()
		.Fixed()
		);
	
	long style = wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_PLAIN_BACKGROUND;
	//auto navigateToolBar = new wxAuiToolBar(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	//navigateToolBar->AddTool(wxID_REFRESH, "Обновить", mgr->m_ico_refresh16, "Обновить(CTRL+R)");
	//navigateToolBar->AddTool(wxID_UP,"Вверх", mgr->m_ico_sort_asc16, "Вверх(BACKSPACE)");
	//navigateToolBar->Realize();
	//panel->mAuiMgr.AddPane(navigateToolBar, wxAuiPaneInfo().Name(wxT("navigateToolBar"))
	//	.Top().CaptionVisible(false).Dock().Fixed().DockFixed(true)
	//	.Row(1).Layer(0)
	//);

	mViewPathBrowser = std::make_shared<ViewPathBrowser>(panel);
	panel->mAuiMgr.AddPane(mViewPathBrowser->GetWnd(), wxAuiPaneInfo().Name(wxT("PathBrowser"))
		.Top().CaptionVisible(false).Dock().Resizable()
		.Row(1).Layer(0)
	);

	mCtrlFind = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition,
									wxDefaultSize, wxTE_PROCESS_ENTER);
	panel->mAuiMgr.AddPane(mCtrlFind, wxAuiPaneInfo().Name("mCtrlFind")
		.Top().CaptionVisible(false).Dock().Fixed()
		.Row(1).Layer(0)
	);

	auto findToolBar = new wxAuiToolBar(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	//auto mCtrlFind = new wxTextCtrl(findToolBar, wxID_ANY);
	//findToolBar->AddControl(mCtrlFind, "Поиск");
	findToolBar->AddTool(wxID_FIND, "Поиск", wxArtProvider::GetBitmap(wxART_FIND, wxART_MENU), "Поиск(CTRL+F)");
	findToolBar->Realize();
	panel->mAuiMgr.AddPane(findToolBar, wxAuiPaneInfo().Name(wxT("findToolBar"))
		.Top().CaptionVisible(false).Dock().Fixed().DockFixed(true)
		.Row(1).Layer(0)
	);


	mViewTableBrowser = std::make_shared<ViewTableBrowser>(panel);
	panel->mAuiMgr.AddPane(mViewTableBrowser->GetWnd(), wxAuiPaneInfo().Name(wxT("TableBrowser"))
		.CaptionVisible(false)
		.CenterPane()
		);



	mViewTableBrowser->GetWnd()->SetFocus();
	panel->mAuiMgr.Update();


	mCtrlFind->Bind(wxEVT_COMMAND_TEXT_ENTER, &ViewBrowserPage::OnCmd_Find, this);
	panel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewBrowserPage::OnCmd_Find, this, wxID_FIND);
}
//-----------------------------------------------------------------------------
void ViewBrowserPage::OnCmd_Find(wxCommandEvent& evt)
{
	sigFind(mCtrlFind->GetValue());
}
//-----------------------------------------------------------------------------
//virtual 
std::shared_ptr<IViewToolbarBrowser>	
ViewBrowserPage::GetViewToolbarBrowser()const// override;
{
	return mViewToolbarBrowser;
}
//-----------------------------------------------------------------------------
//virtual 
std::shared_ptr<IViewPathBrowser>
ViewBrowserPage::GetViewPathBrowser()const// override;
{
	return mViewPathBrowser;
}
//-----------------------------------------------------------------------------
//virtual 
std::shared_ptr<IViewTableBrowser>		
ViewBrowserPage::GetViewTableBrowser()const //override;
{
	return mViewTableBrowser;
}
//-----------------------------------------------------------------------------
//virtual 
void ViewBrowserPage::SetPathMode(const int mode)// override;
{

}
//-----------------------------------------------------------------------------
//virtual 
void ViewBrowserPage::SetPathString(const ICls64& path_string) //override;
{

}
//-----------------------------------------------------------------------------
//virtual 
void ViewBrowserPage::SetAfterRefreshCls(const std::vector<const IIdent64*>& vec
	, const IIdent64* root
	, const wxString& ss) //override;
{
	mCtrlFind->SetValue(ss);
	sigUpdateTitle();
}
