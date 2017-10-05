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
		return "wxDataViewIconText";//return "string";
	}

	virtual bool IsContainer(const wxDataViewItem &item)const override
	{
		if (!item.IsOk())
			return true;

		const auto node = static_cast<const ClsNode*> (item.GetID());
		const auto cls = std::dynamic_pointer_cast<const ICls64>(node->GetValue());
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
		, const std::shared_ptr<const ICls64>& cls) const
	{
		const wxIcon*  ico(&wxNullIcon);
		auto mgr = ResMgr::GetInstance();

		switch (col)
		{
		case 0:variant = cls->GetIdAsString();	break;
		case 1:
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

	void GetObjValue(wxVariant &variant, unsigned int col
		, const std::shared_ptr<const IObj64>& cls) const
	{
		const wxIcon*  ico(&wxNullIcon);
		auto mgr = ResMgr::GetInstance();

		switch (col)
		{
		case 0: variant = cls->GetIdAsString();	break;
		case 1: variant << wxDataViewIconText(cls->GetTitle(), *ico); break;
		case 2:		break;
		case 3:		break;
		default: break;
		}
	}

	virtual void GetValue(wxVariant &variant,
		const wxDataViewItem &dvitem, unsigned int col) const override
	{
		const auto node = static_cast<const ClsNode*> (dvitem.GetID());
		const auto ident = node->GetValue();
		if (!ident)
		{
			GetErrorValue(variant, col);
			return;
		}
		
		const auto& cls = std::dynamic_pointer_cast<const ICls64>(ident);
		if (cls)
		{
			GetClsValue(variant, col, cls);
			return;
		}
		const auto& obj = std::dynamic_pointer_cast<const IObj64>(ident);
		if (obj)
		{
			GetObjValue(variant, col, obj);
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

	virtual wxDataViewItem GetParent(const wxDataViewItem &item) const override
	{
		if (!item.IsOk())
			return wxDataViewItem(nullptr);

		const auto node = static_cast<const ClsNode*> (item.GetID());
		if (node)
		{
			const ClsNode* parent = node->GetParent().get();
			if(parent!= mCurrent)
				return wxDataViewItem((void*)node->GetParent().get());
		}
			

		return wxDataViewItem(nullptr);
	}
	virtual unsigned int GetChildren(const wxDataViewItem &parent
		, wxDataViewItemArray &array) const override
	{
		if (!mCurrent)
			return 0;

		std::shared_ptr<const ClsNode::ChildsTable> node_parent;
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
			//ItemAdded(dvparent, wxDataViewItem((void*)item));
		}
		ItemsAdded(dvparent, arr);
	}

	void DelItems(const ClsNode& parent, const NotyfyTable& list)
	{
		const ClsNode* pnode = (&parent == mCurrent) ? nullptr : &parent;
		wxDataViewItem dvparent((void*)pnode);

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

	const ClsNode* FindNode(const int64_t& id)const
	{
		wxDataViewItemArray arr;
		GetChildren(wxDataViewItem(nullptr), arr);
		for (size_t i = 0; i < arr.size(); i++)
		{
			const auto node = static_cast<const ClsNode*> (arr[i].GetID() );
			
			if (node && node->GetValue()->GetId() == id)
				return node;
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
		Cleared();
	}

	const ClsNode*	GetCurrent()
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
	auto renderer3 = new wxDataViewTextRenderer();

	//table->AppendTextColumn("#", 0,wxDATAVIEW_CELL_INERT,-1, wxALIGN_LEFT);

	auto col1 = new wxDataViewColumn("���"
		, renderer1, 1, 150, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	table->AppendColumn(col1);
	auto col2 = new wxDataViewColumn("����������"
		, renderer2, 2, 150, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	table->AppendColumn(col2);
	
	auto col3 = new wxDataViewColumn("��.���"
		, renderer3, 3, 80, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	table->AppendColumn(col3);

	auto col4 = table->AppendTextColumn("���/��������������", 4);


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
	
	const ClsNode* node = static_cast<const ClsNode*> (mTable->GetCurrentItem().GetID());
	if(node)
		mClsSelected=node->GetValue()->GetId();

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
		const ClsNode* finded = dvmodel->FindNode(mClsSelected);
		if (finded)
			dvitem = wxDataViewItem((void*)finded);
	}

	if (!dvitem.IsOk())
	{
		const ClsNode* finded = dvmodel->GetTopItem();
		if (finded)
			dvitem = wxDataViewItem((void*)finded);
	}
	
	
	mTable->SetCurrentItem(dvitem);
	mTable->Select(dvitem);
	mTable->EnsureVisible(dvitem);
	
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
			sigActivate(node->GetValue()->GetId());
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

	const ClsNode* node = static_cast<const ClsNode*> (item.GetID());
	sigActivate(node->GetValue()->GetId());
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
		mClsSelected=dvmodel->GetCurrent()->GetValue()->GetId();
	
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

	dvmodel->SetCurrent(&node);
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
void ViewTableBrowser::SetAfterInsert(const ClsNode& parent, const NotyfyTable& obj_list)// override;
{
	auto p0 = GetTickCount();
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return;

	dvmodel->AddItems(parent, obj_list);
	
	

	if (mColAutosize)
	{
		for (size_t i = 0; i < mTable->GetColumnCount(); i++)
			mTable->GetColumn(i)->SetWidth(mTable->GetBestColumnWidth(i));
	}
	
	RestoreSelect();

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
		//| wxAUI_TB_TEXT
		//| wxAUI_TB_HORZ_TEXT
		//| wxAUI_TB_OVERFLOW
		;
	auto tool_bar = new wxAuiToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);

	tool_bar->AddTool(wxID_REFRESH, "��������", mgr->m_ico_refresh24, "��������(CTRL+R)");
	tool_bar->AddTool(wxID_UP,"�����", mgr->m_ico_back24, "�����(BACKSPACE)");

	tool_bar->AddTool(wxID_EXECUTE, "���������", mgr->m_ico_act24, "���������(F9)");
	tool_bar->AddTool(wxID_REPLACE, "�����������", mgr->m_ico_move24, "�����������(F6)");

	tool_bar->AddTool(wxID_NEW_TYPE, "������� ���", mgr->m_ico_add_type24, "������� ���(CTRL+T)");
	tool_bar->AddTool(wxID_NEW_OBJECT, "������� ������", mgr->m_ico_add_obj_tab24, "������� ������(CTRL+O)");
	tool_bar->AddTool(wxID_DELETE, "�������", mgr->m_ico_delete24, "�������(DELETE)");
	tool_bar->AddTool(wxID_EDIT, "�������������", mgr->m_ico_edit24, "�������������(CTRL+E)");

	tool_bar->AddTool(wxID_VIEW_LIST , "������������ �� ����", wxArtProvider::GetBitmap(wxART_LIST_VIEW, wxART_TOOLBAR), "������������ �� ����(CTRL+G)");
	tool_bar->AddTool(wxID_VIEW_LARGEICONS, "���������� ������", wxArtProvider::GetBitmap(wxART_FULL_SCREEN, wxART_TOOLBAR), "���������� ������(CTRL+H)");

	tool_bar->AddSeparator();


	auto mFindCtrl = new wxComboBtn(tool_bar, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	wxBitmap bmp(wxArtProvider::GetBitmap(wxART_FIND, wxART_MENU));
	mFindCtrl->SetButtonBitmaps(bmp, true);

	//mFindCtrl->Bind(wxEVT_COMMAND_TEXT_ENTER, fn);
	tool_bar->AddControl(mFindCtrl, "�����");


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
