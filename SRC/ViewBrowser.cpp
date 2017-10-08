#include "_pch.h"
#include "ViewBrowser.h"
#include "globaldata.h"
#include "wxDataViewIconMLTextRenderer.h"
#include "wxComboBtn.h"

using namespace wh;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class wxDVTableBrowser
	: public wxDataViewModel
{
	const ICls64*	mCurrent =nullptr;
	bool mGroupByType = true;
public:
	wxDVTableBrowser(){};
	~wxDVTableBrowser(){};

	// implementation of base class virtuals to define model
	virtual unsigned int	GetColumnCount() const override
	{
		return 1;
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
		if (mGroupByType && cls && ClsKind::Abstract != cls->GetKind())
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
		const wxIcon*  ico(&wxNullIcon);
		auto mgr = ResMgr::GetInstance();

		switch (col)
		{
		case 0:variant = cls.GetIdAsString();	break;
		case 1:
			switch (cls.GetKind())
			{
			case ClsKind::Abstract: ico = &mgr->m_ico_type_abstract24; break;
			case ClsKind::Single:	ico = &mgr->m_ico_type_num24; break;
			case ClsKind::QtyByOne:
			case ClsKind::QtyByFloat:
			default: ico = &mgr->m_ico_type_qty24;	break;
			}
			variant << wxDataViewIconText(
				ClsKind::Abstract == cls.GetKind() ? cls.GetTitle() 
					: wxString::Format("%s - %s (%s)"
						, cls.GetTitle()
						, cls.GetObjectsQty()
						, cls.GetMeasure() 
						)
				, *ico);
			break;
		default: break;
		}
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
							, obj.GetCls()->GetMeasure() ); 
			break;
		default: break;
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
		
		const auto& cls = dynamic_cast<const ICls64*>(ident);
		if (cls)
		{
			GetClsValue(variant, col, *cls);
			return;
		}
		const auto& obj = dynamic_cast<const IObj64*>(ident);
		if (obj)
		{
			GetObjValue(variant, col, *obj);
			return;
		}
		
	}
	//virtual bool GetAttr (const wxDataViewItem &item, unsigned int col, 
	//	wxDataViewItemAttr &attr) const ; 
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

		if (!ascending)
			std::swap(value1, value2);

		if (value1.GetType() == wxT("string"))
		{
			wxString str1 = value1.GetString();
			wxString str2 = value2.GetString();
			int res = str1.CmpNoCase(str2);
			if (res)
				return res;
		}
		else if (value1.GetType() == wxT("wxDataViewIconText"))
		{
			wxDataViewIconText iconText1, iconText2;

			iconText1 << value1;
			iconText2 << value2;

			int res = iconText1.GetText().CmpNoCase(iconText2.GetText());
			if (res != 0)
				return res;
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
			return wxDataViewItem( (void*)obj->GetCls().get() );

		return wxDataViewItem(nullptr);
	}

	virtual unsigned int GetChildren(const wxDataViewItem &parent
		, wxDataViewItemArray &array) const override
	{
		if (!mCurrent)
			return 0;

		if (!parent.IsOk() && mCurrent->GetChilds())
		{
			for (const auto& child_cls : *mCurrent->GetChilds())
			{
				wxDataViewItem dvitem((void*)child_cls.get());
				array.push_back(dvitem);
			}
			return array.size();
		}


		const auto ident = static_cast<const IIdent64*> (parent.GetID());
		const auto cls = dynamic_cast<const ICls64*>(ident);
		if (cls && ClsKind::Abstract != cls->GetKind() && cls->GetObjTable())
		{
			for (const auto& obj : *cls->GetObjTable())
			{
				wxDataViewItem dvitem((void*)obj.get());
				array.push_back(dvitem);
			}
			return array.size();
		}

		return 0;
	}

	void Clear()
	{
		mCurrent = nullptr;
		Cleared();
	}

	void AddItems(const ICls64& parent, const NotyfyTable& list)
	{
		wxDataViewItem dvparent((void*)&parent);
		
		wxDataViewItemArray arr;
		for (const auto& item : list)
		{
			arr.Add(wxDataViewItem((void*)item));
		}
		ItemsAdded(dvparent, arr);
	}

	void DelItems(const ICls64& parent, const NotyfyTable& list)
	{
		wxDataViewItem dvparent((void*)&parent);

		wxDataViewItemArray sel_arr;
		for (const auto& item : list)
		{
			sel_arr.Add(wxDataViewItem((void*)item));
		}
		
		ItemsDeleted(dvparent, sel_arr);
	}

	void UpdateItems(const NotyfyTable& list)
	{
		wxDataViewItemArray arr;
		for (const auto& item : list)
			arr.Add(wxDataViewItem((void*)item));
		ItemsChanged(arr);
	}

	const ICls64* FindNode(const int64_t& id)const
	{
		wxDataViewItemArray arr;
		GetChildren(wxDataViewItem(nullptr), arr);
		for (size_t i = 0; i < arr.size(); i++)
		{
			const auto node = static_cast<const IIdent64*> (arr[i].GetID() );
			const auto cls = dynamic_cast<const ICls64*>(node);
			
			if (cls && cls->GetId() == id)
				return cls;
		}
		return nullptr;
		
		
		
		
		
		/*
		if (!mCurrent || !mCurrent->GetChilds())
			return nullptr;
		
		auto it = std::find_if(mCurrent->GetChilds()->cbegin()
								,mCurrent->GetChilds()->cend()
		, [&id](const std::shared_ptr<ClsNode>& it){ return it->GetValue()->GetId() == id; });
		
		if (mCurrent->GetChilds()->cend() != it)
		{
			return (*it).get();
		}
		return nullptr;
		*/
	}

	const ICls64* GetTopItem()const
	{
		if (mCurrent && mCurrent->GetChilds() && !mCurrent->GetChilds()->empty())
			return mCurrent->GetChilds()->front().get();

		return nullptr;
	}
	
	void SetCurrent(const ICls64*	current)
	{
		if (mCurrent)
			Clear();	
		mCurrent = current;
		Cleared();
	}

	const ICls64*	GetCurrent()
	{
		return mCurrent;
	}

	void SetGroupByType(bool enable)
	{
		mGroupByType = enable;
	}

	bool GetGroupByType()
	{
		return mGroupByType;
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
ViewTableBrowser::ViewTableBrowser(std::shared_ptr<IViewWindow> parent)
	:ViewTableBrowser(parent->GetWnd())
{}
//-----------------------------------------------------------------------------
ViewTableBrowser::ViewTableBrowser(wxWindow* parent)
{
	auto table = new wxDataViewCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize
		, wxDV_ROW_LINES 
		 | wxDV_VERT_RULES 
		 //| wxDV_HORIZ_RULES
		 | wxDV_MULTIPLE);
	auto dv_model = new wxDVTableBrowser();
	table->AssociateModel(dv_model);
	dv_model->DecRef();

	int ch = table->GetCharHeight();
	table->SetRowHeight(ch * 1.6 );


	//int ch = table->GetCharHeight();
	//table->SetRowHeight(ch * 4 + 2);

	auto renderer1 = new wxDataViewIconTextRenderer();
	auto attr = renderer1->GetAttr();
	attr.SetColour(*wxBLACK);
	renderer1->SetAttr(attr);

	auto renderer2 = new wxDataViewTextRenderer();

	//table->AppendTextColumn("#", 0,wxDATAVIEW_CELL_INERT,-1, wxALIGN_LEFT);

	auto col1 = new wxDataViewColumn("Имя"
		, renderer1, 1, 150, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE );
	table->AppendColumn(col1);
	auto col2 = new wxDataViewColumn("Количество"
		, renderer2, 2, 150, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	table->AppendColumn(col2);
	
	auto col3 = table->AppendTextColumn("Тип/Местоположение", 3);


	//table->SetExpanderColumn(col1);

	table->GetTargetWindow()->SetToolTip("ToolTip");


	table->Bind(wxEVT_MOTION, &ViewTableBrowser::OnCmd_MouseMove, this);

	table->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED
		, &ViewTableBrowser::OnCmd_Select, this);

	table->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED
		, &ViewTableBrowser::OnCmd_Activate, this);

	table->Bind(wxEVT_DATAVIEW_ITEM_EXPANDING
		, &ViewTableBrowser::OnCmd_Expanding, this);



	table->Bind(wxEVT_COMMAND_MENU_SELECTED
		,&ViewTableBrowser::OnCmd_Refresh, this, wxID_REFRESH);

	table->Bind(wxEVT_COMMAND_MENU_SELECTED
		, &ViewTableBrowser::OnCmd_Up, this, wxID_UP);





	mTable = table;

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
	auto p0 = GetTickCount();
	wxBusyCursor busyCursor;
	wxWindowUpdateLocker lock(mTable);
	sigRefresh();
	wxLogMessage(wxString::Format("%d\t %s ", GetTickCount() - p0, __FUNCTION__));
}
//-----------------------------------------------------------------------------
void ViewTableBrowser::OnCmd_Up(wxCommandEvent& evt)
{
	auto p0 = GetTickCount();
	wxBusyCursor busyCursor;
	wxWindowUpdateLocker lock(mTable);
	sigUp();
	wxLogMessage(wxString::Format("%d\t %s ", GetTickCount() - p0, __FUNCTION__));
}
//-----------------------------------------------------------------------------
void ViewTableBrowser::OnCmd_Select(wxDataViewEvent& evt)
{
	mClsSelected=0;
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
			if(cls)
				mClsSelected = cls->GetId();
		}
	}//if (node)


}
//-----------------------------------------------------------------------------
void ViewTableBrowser::RestoreSelect()
{
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return;

	wxDataViewItem dvitem;
	if(mClsSelected)
	{
		const auto finded = dvmodel->FindNode(mClsSelected);
		if (finded)
			dvitem = wxDataViewItem((void*)finded);
	}

	if (!dvitem.IsOk())
	{
		const auto finded = dvmodel->GetTopItem();
		if (finded)
			dvitem = wxDataViewItem((void*)finded);
	}
	
	
	mTable->SetCurrentItem(dvitem);
	mTable->Select(dvitem);
	mTable->EnsureVisible(dvitem);
}
//-----------------------------------------------------------------------------
void ViewTableBrowser::AutosizeColumns()
{
	if (mColAutosize)
	{
		for (size_t i = 0; i < mTable->GetColumnCount(); i++)
			mTable->GetColumn(i)->SetWidth(mTable->GetBestColumnWidth(i));
	}
}

//-----------------------------------------------------------------------------
void ViewTableBrowser::OnCmd_MouseMove(wxMouseEvent& evt)
{
	/*
	wxDataViewColumn* col = nullptr;
	wxDataViewItem item(nullptr);
	auto pos = evt.GetPosition();
	mTable->HitTest(pos, item, col);

	wxString str;
	if (col && item.IsOk())
	{
		wxVariant var;
		mTable->GetModel()->GetValue(var, item, col->GetModelColumn());
		wxDataViewIconText2 ico_txt;
		ico_txt << var;
		str = ico_txt.GetText();
	}
	mTable->GetTargetWindow()->GetToolTip()->SetTip(str);
	*/
}
//-----------------------------------------------------------------------------
void ViewTableBrowser::OnCmd_Activate(wxDataViewEvent& evt)
{
	auto p0 = GetTickCount();
	wxBusyCursor busyCursor;
	wxWindowUpdateLocker lock(mTable);
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
		if (node)
			sigActivate(node->GetId());
	}

	wxLogMessage(wxString::Format("%d\t %s ", GetTickCount() - p0, __FUNCTION__));
}
//-----------------------------------------------------------------------------
void ViewTableBrowser::OnCmd_Expanding(wxDataViewEvent& evt)
{
	auto p0 = GetTickCount();
	wxBusyCursor busyCursor;
	wxWindowUpdateLocker lock(mTable);
	auto item = evt.GetItem();

	const IIdent64* node = static_cast<const IIdent64*> (item.GetID());
	sigActivate(node->GetId());
	wxLogMessage(wxString::Format("%d\t %s ", GetTickCount() - p0, __FUNCTION__));
}
//-----------------------------------------------------------------------------
//virtual
void ViewTableBrowser::SetBeforePathChange(const ICls64& node)// override;
{
	auto p0 = GetTickCount();
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return;

	if (dvmodel->GetCurrent())
		mClsSelected=dvmodel->GetCurrent()->GetId();
	
	dvmodel->Clear();
	wxLogMessage(wxString::Format("%d\t %s ", GetTickCount() - p0, __FUNCTION__));
}
//-----------------------------------------------------------------------------
//virtual
void ViewTableBrowser::SetAfterPathChange(const ICls64& node)// override;
{
	auto p0 = GetTickCount();
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return;

	dvmodel->SetCurrent(&node);
	AutosizeColumns();
	RestoreSelect();

	wxLogMessage(wxString::Format("%d\t %s ", GetTickCount() - p0, __FUNCTION__));
}

//-----------------------------------------------------------------------------
//virtual
void ViewTableBrowser::SetGroupByType(bool enable)// override;
{
	auto p0 = GetTickCount();
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return;

	dvmodel->SetGroupByType(enable);

	if (enable)
	{
		auto col1 = mTable->GetColumnAt(0);
		mTable->SetExpanderColumn(col1);
	}
	else
	{
		mTable->SetExpanderColumn(nullptr);
	}
		
	
	
	
	OnCmd_Refresh(wxCommandEvent());

	wxLogMessage(wxString::Format("%d\t %s ", GetTickCount() - p0, __FUNCTION__));
}
//-----------------------------------------------------------------------------
//virtual
void ViewTableBrowser::SetCollapsedGroupByType(bool enable)// override;
{

}
//-----------------------------------------------------------------------------
//virtual
void ViewTableBrowser::SetClear()// override;
{
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return;
	dvmodel->Clear();
	RestoreSelect();
}
//-----------------------------------------------------------------------------
//virtual
void ViewTableBrowser::SetAfterInsert(const ICls64& parent, const NotyfyTable& obj_list)// override;
{
	auto p0 = GetTickCount();
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return;

	dvmodel->AddItems(parent, obj_list);
	
	

	AutosizeColumns();
	RestoreSelect();

	wxLogMessage(wxString::Format("%d\t %s ", GetTickCount() - p0, __FUNCTION__));
}
//-----------------------------------------------------------------------------
//virtual
void ViewTableBrowser::SetAfterUpdate(const ICls64& parent, const NotyfyTable& list)// override;
{
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return;

	dvmodel->UpdateItems(list);

	RestoreSelect();
}
//-----------------------------------------------------------------------------
//virtual
void ViewTableBrowser::SetBeforeDelete(const ICls64& parent, const NotyfyTable& list)// override;
{
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return;

	dvmodel->DelItems(parent, list);

	RestoreSelect();

}
//-----------------------------------------------------------------------------
//virtual 
void ViewTableBrowser::SetPathMode(const int mode)// override;
{

}













//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewToolbarBrowser::ViewToolbarBrowser(std::shared_ptr<IViewWindow> parent)
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

	//tool_bar->AddTool(wxID_REFRESH, "Обновить", mgr->m_ico_refresh24, "Обновить(CTRL+R)");
	//tool_bar->AddTool(wxID_UP,"Вверх", mgr->m_ico_back24, "Вверх(BACKSPACE)");

	tool_bar->AddTool(wxID_EXECUTE, "Выполнить", mgr->m_ico_act24, "Выполнить(F9)");
	tool_bar->AddTool(wxID_REPLACE, "Переместить", mgr->m_ico_move24, "Переместить(F6)");

	tool_bar->AddTool(wxID_NEW_TYPE, "Создать тип", mgr->m_ico_add_type24, "Создать тип(CTRL+T)");
	tool_bar->AddTool(wxID_NEW_OBJECT, "Создать объект", mgr->m_ico_add_obj_tab24, "Создать объект(CTRL+O)");
	tool_bar->AddTool(wxID_DELETE, "Удалить", mgr->m_ico_delete24, "Удалить(DELETE)");
	tool_bar->AddTool(wxID_EDIT, "Редактировать", mgr->m_ico_edit24, "Редактировать(CTRL+E)");

	tool_bar->AddTool(wxID_VIEW_LIST , "Группировать по типу", wxArtProvider::GetBitmap(wxART_LIST_VIEW, wxART_TOOLBAR), "Группировать по типу(CTRL+G)");
	tool_bar->AddTool(wxID_VIEW_LARGEICONS, "Развернуть группы", wxArtProvider::GetBitmap(wxART_FULL_SCREEN, wxART_TOOLBAR), "Развернуть группы(CTRL+H)");

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
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarBrowser::OnCmd_Find, this, wxID_FIND);
	
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarBrowser::OnCmd_AddType, this, wxID_NEW_TYPE);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarBrowser::OnCmd_AddObject, this, wxID_NEW_OBJECT);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarBrowser::OnCmd_DeleteSelected, this, wxID_DELETE);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarBrowser::OnCmd_UpdateSelected, this, wxID_EDIT);
	
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarBrowser::OnCmd_GroupByType, this, wxID_VIEW_LIST);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarBrowser::OnCmd_CollapseGroupByType, this, wxID_VIEW_LARGEICONS);

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
void ViewToolbarBrowser::SetCollapsedGroupByType(bool enable)// override;
{}
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
void ViewToolbarBrowser::OnCmd_Find(wxCommandEvent& evt)
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
ViewPathBrowser::ViewPathBrowser(std::shared_ptr<IViewWindow> parent)
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
ViewBrowserPage::ViewBrowserPage(std::shared_ptr<IViewWindow> parent)
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
	auto navigateToolBar = new wxAuiToolBar(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	navigateToolBar->AddTool(wxID_REFRESH, "Обновить", mgr->m_ico_refresh16, "Обновить(CTRL+R)");
	navigateToolBar->AddTool(wxID_UP,"Вверх", mgr->m_ico_sort_asc16, "Вверх(BACKSPACE)");
	navigateToolBar->Realize();
	panel->mAuiMgr.AddPane(navigateToolBar, wxAuiPaneInfo().Name(wxT("navigateToolBar"))
		.Top().CaptionVisible(false).Dock().Fixed().DockFixed(true)
		.Row(1).Layer(0)
	);

	mViewPathBrowser = std::make_shared<ViewPathBrowser>(panel);
	panel->mAuiMgr.AddPane(mViewPathBrowser->GetWnd(), wxAuiPaneInfo().Name(wxT("PathBrowser"))
		.Top().CaptionVisible(false).Dock().Resizable()
		.Row(1).Layer(0)
	);

	auto findToolBar = new wxAuiToolBar(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	auto mCtrlFind = new wxTextCtrl(findToolBar, wxID_ANY);
	findToolBar->AddControl(mCtrlFind, "Поиск");
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
