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
	const ClsNode*	mCurrent=nullptr;
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
		return "wxDataViewIconText";//return "string";
	}

	virtual bool IsContainer(const wxDataViewItem &item)const override
	{
		if (!item.IsOk())
			return true;

		const auto node = static_cast<const ClsNode*> (item.GetID());
		const auto cls = std::dynamic_pointer_cast<const ICls64>(node->GetValue());
		if (cls && ClsKind::Abstract != cls->GetKind())
			return true;

		return false;
	}


	virtual void GetValue(wxVariant &variant,
		const wxDataViewItem &dvitem, unsigned int col) const override
	{

		const auto node = static_cast<const ClsNode*> (dvitem.GetID());
		const auto cls = node->GetValue();
		if (!cls)
		{
			if(1==col)
				variant << wxDataViewIconText2("*ERROR*", wxNullIcon);
			return;
		}


		const wxIcon*  ico(&wxNullIcon);
		auto mgr = ResMgr::GetInstance();

		switch (col)
		{
		case 0:variant = cls->GetIdAsString();	break;
		case 1:		
			if (".." == cls->GetTitle())
				ico = &mgr->m_ico_back24;
			else
				switch (cls->GetKind())
				{
				case ClsKind::Abstract: ico = &mgr->m_ico_type_abstract24; break;
				case ClsKind::Single:	ico = &mgr->m_ico_type_num24; break;
				case ClsKind::QtyByOne:	
				case ClsKind::QtyByFloat:
				default: ico = &mgr->m_ico_type_qty24;	break;
				}

			variant << wxDataViewIconText(cls->GetTitle(), *ico);
			break;
		case 2:		break;
		case 3:		variant = cls->GetMeasure();	break;
		default: break;
		}
		
	}
	//virtual bool GetAttr (const wxDataViewItem &item, unsigned int col, 
	//	wxDataViewItemAttr &attr) const ; 
	virtual bool SetValue(const wxVariant &variant, const wxDataViewItem &item,
		unsigned int col)override
	{
		return false;
	}

	virtual wxDataViewItem GetParent(const wxDataViewItem &item) const override
	{
		if (!item.IsOk())
			return wxDataViewItem(nullptr);

		const auto node = static_cast<const ClsNode*> (item.GetID());
		if (node && !(node->GetParent().get() == mCurrent))
		{
			return wxDataViewItem((void*)node->GetParent().get());
		}
			

		return wxDataViewItem(nullptr);
	}
	virtual unsigned int GetChildren(const wxDataViewItem &parent
		, wxDataViewItemArray &array) const override
	{
		if (!mCurrent)
			return 0;

		std::shared_ptr<ClsNode::ChildsTable> node_parent;
		if (!parent.IsOk() )
		{
			node_parent = mCurrent->GetChilds();
		}
		else
		{
			node_parent = (static_cast<const ClsNode*> (parent.GetID()))->GetChilds();
		}
			
		if (node_parent && !node_parent->empty())
		{
			for (const auto& child : *node_parent)
			{
				wxDataViewItem dvitem((void*)child.get());
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

	void AddItems(const ClsNode& parent, const NotyfyTable& list)
	{
		const ClsNode* pnode = (&parent == mCurrent) ? nullptr : &parent;
		wxDataViewItem dvparent((void*)pnode);
		
		wxDataViewItemArray arr;
		for (const auto& item : list)
		{
			arr.Add(wxDataViewItem((void*)item));
			//mRoot.emplace_back(item);
		}
		ItemsAdded(dvparent, arr);
	}

	void DelItems(const ClsNode& parent, const NotyfyTable& list)
	{
		wxDataViewItem dvroot(nullptr);

		wxDataViewItemArray sel_arr;
		for (const auto& item : list)
		{
			sel_arr.Add(wxDataViewItem((void*)item));
		}
		
		ItemsDeleted(dvroot, sel_arr);
	}

	void UpdateItems(const NotyfyTable& list)
	{
		wxDataViewItemArray arr;
		for (const auto& item : list)
			arr.Add(wxDataViewItem((void*)item));
		ItemsChanged(arr);
	}

	const ClsNode* FindNode(const int64_t& id)const
	{
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
	}

	const ClsNode* GetTopItem()const
	{
		if (mCurrent && mCurrent->GetChilds() && !mCurrent->GetChilds()->empty())
			return mCurrent->GetChilds()->front().get();

		return nullptr;
	}
	
	void SetCurrent(const ClsNode*	current)
	{
		if (mCurrent)
			Clear();	
		mCurrent = current;
	}

	const ClsNode*	GetCurrent()
	{
		return mCurrent;
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
	auto renderer3 = new wxDataViewTextRenderer();

	//table->AppendTextColumn("#", 0,wxDATAVIEW_CELL_INERT,-1, wxALIGN_LEFT);

	auto col1 = new wxDataViewColumn("Имя"
		, renderer1, 1, 150, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	table->AppendColumn(col1);
	auto col2 = new wxDataViewColumn("Количество"
		, renderer2, 2, 150, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	table->AppendColumn(col2);
	
	auto col3 = new wxDataViewColumn("Ед.изм"
		, renderer3, 3, 80, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	table->AppendColumn(col3);

	//table->SetExpanderColumn(col1);

	table->GetTargetWindow()->SetToolTip("ToolTip");


	table->Bind(wxEVT_MOTION, &ViewTableBrowser::OnCmd_MouseMove, this);

	table->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED
		, &ViewTableBrowser::OnCmd_Select, this);

	table->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED
		, &ViewTableBrowser::OnCmd_Activate, this);


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
	mClsSelected.clear();
	wxDataViewItemArray sel_arr;
	int sel_count = mTable->GetSelections(sel_arr);
	for (const auto& item : sel_arr)
	{
		const ClsNode* node = static_cast<const ClsNode*> (item.GetID());
		mClsSelected.emplace_back(node->GetValue()->GetId());
	}

	bool focus = mTable->HasFocus();
}
//-----------------------------------------------------------------------------
void ViewTableBrowser::RestoreSelect()
{
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return;

	wxDataViewItemArray arr;
	for (const auto& sel : mClsSelected)
	{
		const ClsNode* finded = dvmodel->FindNode(sel);
		if (finded)
		{
			wxDataViewItem dvitem((void*)finded);
			arr.Add(dvitem);
		}
	}

	if (arr.empty())
	{
		const ClsNode* finded = dvmodel->GetTopItem();
		wxDataViewItem dvitem((void*)finded);
		arr.Add(dvitem);
	}

	
	mTable->SetSelections(arr);
	mTable->EnsureVisible(arr[0]);
	mTable->SetCurrentItem(arr[0]);
	
	
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
		const ClsNode* node = static_cast<const ClsNode*> (item.GetID());
		if (node)
			sigActivate(*node);
	}
	wxLogMessage(wxString::Format("%d\t %s ", GetTickCount() - p0, __FUNCTION__));
}
//-----------------------------------------------------------------------------
//virtual
void ViewTableBrowser::SetBeforePathChange(const ClsNode& node)// override;
{
	auto p0 = GetTickCount();
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return;

	if (dvmodel->GetCurrent())
		mClsSelected.emplace_back(dvmodel->GetCurrent()->GetValue()->GetId());
	
	dvmodel->Clear();
	wxLogMessage(wxString::Format("%d\t %s ", GetTickCount() - p0, __FUNCTION__));
}
//-----------------------------------------------------------------------------
//virtual
void ViewTableBrowser::SetAfterPathChange(const ClsNode& node)// override;
{
	auto p0 = GetTickCount();
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return;

	mTable->Unselect(mTable->GetSelection());
	mTable->UnselectAll();

	dvmodel->SetCurrent(&node);


	wxLogMessage(wxString::Format("%d\t %s ", GetTickCount() - p0, __FUNCTION__));
}

//-----------------------------------------------------------------------------
//virtual
void ViewTableBrowser::SetGroupByType(bool enable)// override;
{

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
}
//-----------------------------------------------------------------------------
//virtual
void ViewTableBrowser::SetAfterInsert(const ClsNode& parent, const NotyfyTable& obj_list)// override;
{
	auto p0 = GetTickCount();
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return;

	dvmodel->AddItems(parent, obj_list);
	
	RestoreSelect();

	if (mColAutosize)
	{
		for (size_t i = 0; i < mTable->GetColumnCount(); i++)
			mTable->GetColumn(i)->SetWidth(mTable->GetBestColumnWidth(i));
	}

	
	
	wxLogMessage(wxString::Format("%d\t %s ", GetTickCount() - p0, __FUNCTION__));
}
//-----------------------------------------------------------------------------
//virtual
void ViewTableBrowser::SetAfterUpdate(const ClsNode& parent, const NotyfyTable& list)// override;
{
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return;

	dvmodel->UpdateItems(list);

	RestoreSelect();
}
//-----------------------------------------------------------------------------
//virtual
void ViewTableBrowser::SetBeforeDelete(const ClsNode& parent, const NotyfyTable& list)// override;
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

	long style = wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_PLAIN_BACKGROUND
		| wxAUI_TB_TEXT
		//| wxAUI_TB_HORZ_TEXT
		| wxAUI_TB_OVERFLOW
		;
	auto tool_bar = new wxAuiToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);

	tool_bar->AddTool(wxID_REFRESH, "Обновить", mgr->m_ico_refresh24, "Обновить(CTRL+R)");
	tool_bar->AddTool(wxID_UP,"Вверх", mgr->m_ico_back24, "Вверх(BACKSPACE)");

	tool_bar->AddTool(wxID_EXECUTE, "Выполнить", mgr->m_ico_act24, "Выполнить(F9)");
	tool_bar->AddTool(wxID_REPLACE, "Переместить", mgr->m_ico_move24, "Переместить(F6)");

	tool_bar->AddTool(wxID_NEW_TYPE, "Создать тип", mgr->m_ico_add_type24, "Создать тип(CTRL+T)");
	tool_bar->AddTool(wxID_NEW_OBJECT, "Создать объект", mgr->m_ico_add_obj_tab24, "Создать объект(CTRL+O)");
	tool_bar->AddTool(wxID_DELETE, "Удалить", mgr->m_ico_delete24, "Удалить(DELETE)");
	tool_bar->AddTool(wxID_EDIT, "Редактировать", mgr->m_ico_edit24, "Редактировать(CTRL+E)");

	tool_bar->AddTool(wxID_VIEW_LIST , "Группировать по типу", wxArtProvider::GetBitmap(wxART_LIST_VIEW, wxART_TOOLBAR), "Группировать по типу(CTRL+G)");
	tool_bar->AddTool(wxID_VIEW_LARGEICONS, "Развернуть группы", wxArtProvider::GetBitmap(wxART_FULL_SCREEN, wxART_TOOLBAR), "Развернуть группы(CTRL+H)");

	tool_bar->AddSeparator();


	auto mFindCtrl = new wxComboBtn(tool_bar, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	wxBitmap bmp(wxArtProvider::GetBitmap(wxART_FIND, wxART_MENU));
	mFindCtrl->SetButtonBitmaps(bmp, true);

	//mFindCtrl->Bind(wxEVT_COMMAND_TEXT_ENTER, fn);
	tool_bar->AddControl(mFindCtrl, "Поиск");


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
{}
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
	mPathCtrl = new wxTextCtrl(parent, wxID_ANY, wxEmptyString);
		//,wxPoint(0, 0), wxSize(150, 90),wxNO_BORDER | wxTE_MULTILINE);
}
//-----------------------------------------------------------------------------
//virtual 
void ViewPathBrowser::SetPathMode(const int mode)// override;
{
}
//-----------------------------------------------------------------------------
//virtual 
void ViewPathBrowser::SetPathString(const ClsNode& node)// override;
{
	wxString ret = "/";
	const ClsNode* curr = &node;
	while (curr != nullptr && curr->GetValue()->GetId()!=1)
	{
		const auto& title = curr->GetValue()->GetTitle();

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
	auto panel = new wxAuiPanel(parent);
	mAuiMgr = &panel->mAuiMgr;
	mPanel = panel;

	auto face_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
	panel->mAuiMgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, face_colour);

	mViewToolbarBrowser = std::make_shared<ViewToolbarBrowser>(panel);
	panel->mAuiMgr.AddPane(mViewToolbarBrowser->GetWnd(), wxAuiPaneInfo().
		Name(wxT("BrowserToolBar"))
		.CaptionVisible(false)
		.ToolbarPane()
		.Top()
		.Fixed()
		.Dockable(false)
		.PaneBorder(false)
		.Gripper(false)
		);

	
	mViewPathBrowser = std::make_shared<ViewPathBrowser>(panel);

	auto sz = mViewPathBrowser->GetWnd()->GetBestSize();

	panel->mAuiMgr.AddPane(mViewPathBrowser->GetWnd(), wxAuiPaneInfo().
		Name(wxT("PathBrowser"))
		.Top()//.Layer(1).Position(1)
		//.Floatable(false)
		.PaneBorder(false)
		.CaptionVisible(false)
		.Fixed()
		.MinSize(sz)
		);



	mViewTableBrowser = std::make_shared<ViewTableBrowser>(panel);
	panel->mAuiMgr.AddPane(mViewTableBrowser->GetWnd(), wxAuiPaneInfo().
		Name(wxT("TableBrowser")).CenterPane()
		.PaneBorder(false)
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
void ViewBrowserPage::SetPathString(const ClsNode& path_string) //override;
{

}
