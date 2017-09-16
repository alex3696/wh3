#include "_pch.h"
#include "ViewBrowser.h"
#include "globaldata.h"
#include "wxDataViewIconMLTextRenderer.h"
#include "wxComboBtn.h"
#include <wx/srchctrl.h>

using namespace wh;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class wxDVTableBrowser
	: public wxDataViewModel
{
	NotyfyTable mRoot;
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

		return false;
	}


	virtual void GetValue(wxVariant &variant,
		const wxDataViewItem &dvitem, unsigned int col) const override
	{
		if (!dvitem.IsOk())
		{
			variant << wxDataViewIconText2("*ERROR*", wxNullIcon);
			return;
		}
			
		const auto node = static_cast<const IIdent64*> (dvitem.GetID());
		const auto cls = dynamic_cast<const ICls64*> (node);
		if (!cls)
		{
			variant << wxDataViewIconText2("*ERROR*", wxNullIcon);
			return;
		}

		const wxIcon*  ico(&wxNullIcon);

		switch (col)
		{
		case 1:		variant << wxDataViewIconText2(cls->GetTitle(), *ico);
			break;
		case 2:		variant << wxDataViewIconText2(cls->GetIdAsString(), wxNullIcon);
			break;
		default: variant << wxDataViewIconText2(wxEmptyString, wxNullIcon);
			break;
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

		const auto node = static_cast<const IIdent64*> (item.GetID());
		const auto cls = dynamic_cast<const ICls64*> (node);
		if (cls)
			return wxDataViewItem(nullptr);

		return wxDataViewItem(nullptr);
	}
	virtual unsigned int GetChildren(const wxDataViewItem &parent
		, wxDataViewItemArray &array) const override
	{
		if (mRoot.empty())
			return 0;
		if (!parent.IsOk())
		{
			for (const auto& child : mRoot)
			{
				//IDbIdent* obj_ptr = obj.get();
				const IIdent64* node_ptr = child;
				wxDataViewItem dvitem((void*)node_ptr);
				array.push_back(dvitem);
			}
			return array.size();

		}

		//const IDbIdent* node = static_cast<const IDbIdent*> (parent.GetID());
		//const ICls* cls = dynamic_cast<const ICls*> (node);

		return 0;
	}

	void Clear()
	{
		mRoot.clear();
		Cleared();
	}


	void AddItems(const NotyfyTable& list)
	{
		wxDataViewItem dvparent(nullptr);
		wxDataViewItemArray arr;
		for (const auto& item : list)
		{
			arr.Add(wxDataViewItem((void*)item));
			mRoot.emplace_back(item);
		}
		ItemsAdded(dvparent, arr);
	}

	void DelItems(const NotyfyTable& list)
	{
		std::vector<NotyfyTable::const_iterator> to_del;
		
		wxDataViewItem dvroot(nullptr);

		wxDataViewItemArray sel_arr;
		for (const auto& item : list)
		{
			const IIdent64* ff = item;
			auto it = std::find(mRoot.cbegin(), mRoot.cend(), ff);
			if (mRoot.cend() != it)
			{
				to_del.emplace_back(it);
				sel_arr.Add(wxDataViewItem((void*)item));
			}
		}

		ItemsDeleted(dvroot, sel_arr);
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

	auto renderer1 = new wxDataViewIconMLTextRenderer();
	renderer1->SetAlignment(wxALIGN_TOP);
	auto attr = renderer1->GetAttr();
	attr.SetColour(*wxBLACK);
	renderer1->SetAttr(attr);

	auto renderer2 = new wxDataViewIconMLTextRenderer();
	renderer2->SetAlignment(wxALIGN_TOP);
	auto renderer3 = new wxDataViewIconMLTextRenderer();
	renderer3->SetAlignment(wxALIGN_TOP);
	auto renderer4 = new wxDataViewIconMLTextRenderer();
	renderer4->SetAlignment(wxALIGN_TOP);

	auto col1 = new wxDataViewColumn("Имя"
		, renderer1, 1, 150, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	table->AppendColumn(col1);
	auto col2 = new wxDataViewColumn("Тип(кратко/подробно)"
		, renderer2, 2, 150, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	table->AppendColumn(col2);
	
	auto col3 = new wxDataViewColumn("Количество"
		, renderer3, 3, 80, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	table->AppendColumn(col3);

	auto col4 = new wxDataViewColumn("Местоположение(кратко/подробно)"
		, renderer4, 4, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	table->AppendColumn(col4);

	//table->SetExpanderColumn(col2);


	//table->SetCanFocus(false);

	table->GetTargetWindow()->SetToolTip("ToolTip");


	table->Bind(wxEVT_MOTION, &ViewTableBrowser::OnCmd_MouseMove, this);

	table->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED
		, &ViewTableBrowser::OnCmd_Select, this);

	table->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED
		, &ViewTableBrowser::OnCmd_Activate, this);


	mTable = table;

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
		mTable->GetModel()->GetValue(var, item, col->GetModelColumn());
		wxDataViewIconText2 ico_txt;
		ico_txt << var;
		str = ico_txt.GetText();
	}
	mTable->GetTargetWindow()->GetToolTip()->SetTip(str);
}
//-----------------------------------------------------------------------------
void ViewTableBrowser::OnCmd_Select(wxDataViewEvent& evt)
{
	NotyfyTable sel_table;
	wxDataViewItemArray sel_arr;
	int sel_count = mTable->GetSelections(sel_arr);
	for (const auto& item : sel_arr)
	{
		const IIdent64* node = static_cast<const IIdent64*> (item.GetID());
		sel_table.emplace_back(node);
	}
	sigSelect(sel_table);
}
//-----------------------------------------------------------------------------
void ViewTableBrowser::OnCmd_Activate(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	const IIdent64* node = static_cast<const IIdent64*> (item.GetID());
	if (node)
	{
		sigActivate(node);
	}



	
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
void ViewTableBrowser::SetSelect(const NotyfyTable& list)// override;
{
	wxDataViewItemArray arr;
	for (const auto& item : list)
	{
		wxDataViewItem dvitem((void*)item);
		arr.Add(dvitem);
	}
	mTable->SetSelections(arr);
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
void ViewTableBrowser::SetAfterInsert(const NotyfyTable& obj_list)// override;
{
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return;

	dvmodel->AddItems(obj_list);

	if (mColAutosize)
	{
		for (size_t i = 0; i < mTable->GetColumnCount(); i++)
			mTable->GetColumn(i)->SetWidth(mTable->GetBestColumnWidth(i));
	}

}
//-----------------------------------------------------------------------------
//virtual
void ViewTableBrowser::SetAfterUpdate(const NotyfyTable& list)// override;
{

}
//-----------------------------------------------------------------------------
//virtual
void ViewTableBrowser::SetBeforeDelete(const NotyfyTable& list)// override;
{
	auto dvmodel = dynamic_cast<wxDVTableBrowser*>(mTable->GetModel());
	if (!dvmodel)
		return;

	dvmodel->DelItems(list);


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


	//wxComboBtn* cbtn = new wxComboBtn(tool_bar);
	//wxBitmap bmp(wxArtProvider::GetBitmap(wxART_FIND, wxART_TOOLBAR));
	//cbtn->SetButtonBitmaps(bmp, true);
	auto cbtn = new wxSearchCtrl(tool_bar, wxID_ANY, wxEmptyString);
	cbtn->ShowSearchButton(true);
	cbtn->ShowCancelButton(true);
	cbtn->SetDescriptiveText(wxEmptyString);
	tool_bar->AddControl(cbtn, "Поиск");
	//tool_bar->AddTool(wxID_FIND, "Поиск", wxArtProvider::GetBitmap(wxART_FIND, wxART_TOOLBAR), "Быстрый поиск(CTRL+F)");


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
void ViewPathBrowser::SetPathString(const wxString& path_string)// override;
{
	mPathCtrl->SetValue(path_string);
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
void ViewBrowserPage::SetPathString(const wxString& path_string) //override;
{

}
