#include "_pch.h"
#include "MainFrame.h"


#include "whLogin.h"

#include "whPnlCtrl_MovHistory.h"

#include "favorites.h"

#include "VObjCatalogCtrl.h"

#include "VPropCtrlPanel.h"
#include "VActCtrlPanel.h"
#include "VGroupCtrlPanel.h"
#include "VUserCtrlPanel.h"

#include "RDBS.h"
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE( MainFrame, wxFrame )

	EVT_MENU(CMD_MAKETYPEWND,		MainFrame::OnMakeTypeWnd)
	EVT_MENU(CMD_MAKEOBJWND,		MainFrame::OnMakeObjWnd)
	EVT_MENU(CMD_MAKEHISTORYWND,	MainFrame::OnMakeHistoryWnd)
	EVT_MENU(CMD_SHOWFAVORITES,		MainFrame::OnShowFavorites)
	

	EVT_MENU(CMD_DB_CONNECT,	MainFrame::OnShowLoginWnd)
	EVT_MENU(CMD_DB_DISCONNECT,	MainFrame::OnDisconnectDB)

	


END_EVENT_TABLE()

//---------------------------------------------------------------------------
MainFrame::MainFrame(	wxWindow* parent, wxWindowID id, const wxString& title, 
						const wxPoint& pos, const wxSize& size, long style)
	: wxFrame(parent, ID_MAINFRAME, title, wxDefaultPosition, size, style)
{
	m_AuiMgr.SetManagedWindow(this);

	BuildMenu();
	BuildToolbar();
	BuildStatusbar();
 

	m_Notebook = new wxAuiNotebook(this);


	m_AuiMgr.AddPane(m_Notebook, wxAuiPaneInfo().
					Name(wxT("NotebookPane")).Caption(wxT("Notebook")).
					CenterPane().Layer(1).Position(1).CloseButton(true).MaximizeButton(true).PaneBorder(false));

	m_AuiMgr.Update();
	OnShowLoginWnd();
	

	/*
	m_wndFavorites = new wh::favorites::Panel(this);

	m_AuiMgr.AddPane(m_wndFavorites,wxAuiPaneInfo().
										Name("favorites_pane").Caption("Избранное")
											.Left()
											 .Layer(1).Position(1)
											 //.Icon(GetResMgr()->m_ico_favorites16)
											 .MinSize(250,200)
											 .CloseButton(false)
											 .Dockable(false)
											 .Floatable(false)
											 .Hide()
											 );
	
	
	OnShowFavorites();
	*/

	//DoMenuPreferences();


	ShowDevToolBar();
}
//---------------------------------------------------------------------------
MainFrame::~MainFrame()
{
//	m_KindModel->DecRef();  // avoid memory leak !!
	
	while(m_Notebook->GetPageCount())
		m_Notebook->DeletePage(0);
	//bp1->Disconnect();
	m_AuiMgr.UnInit();

	whDataMgr* mgr = whDataMgr::GetInstance();
	mgr->mDb.Close();

}
//---------------------------------------------------------------------------
void MainFrame::BuildMenu()
{
	wxMenuBar*	menu_bar = new wxMenuBar(0);
	wxMenuItem* item = NULL;

	wxMenu* dbmenu = new wxMenu();
	item = new wxMenuItem(dbmenu, CMD_DB_CONNECT, "Подключиться к БД");
	item->SetBitmap(m_ResMgr->m_ico_connect16);
	dbmenu->Append(item);
	item = new wxMenuItem(dbmenu, CMD_DB_DISCONNECT, "Отключиться от БД");
	item->SetBitmap(m_ResMgr->m_ico_disconnect16);
	dbmenu->Append(item);
	dbmenu->AppendSeparator();
	item = new wxMenuItem(dbmenu, wxID_PREFERENCES, "Конфигурация");
	item->SetBitmap(m_ResMgr->m_ico_db16);
	dbmenu->Append(item);
	dbmenu->AppendSeparator();
	item = new wxMenuItem(dbmenu, wxID_EXIT, "Выход");
	dbmenu->Append(item);
	menu_bar->Append(dbmenu, "Подключение");

	wxMenu* view = new wxMenu();
	item = new wxMenuItem(view, CMD_SHOWFAVORITES, "Показать/скрыть избранное", wxEmptyString, wxITEM_CHECK);
	view->Append(item);

	item = new wxMenuItem(view, CMD_SHOWEDITOROPTIONS, "Опции редактора", wxEmptyString, wxITEM_CHECK);
	view->Append(item);
	menu_bar->Append(view, "Вид");


	wxMenu* dir = new wxMenu();
	item = new wxMenuItem(dir, CMD_MAKEOBJWND, "Открыть каталог объектов");
	dir->Append(item);
	item = new wxMenuItem(dir, CMD_MAKEHISTORYWND, "Открыть историю");
	dir->Append(item);
	menu_bar->Append(dir, "Каталоги");


	wxMenu* xrcmenu = new wxMenu();
	item = new wxMenuItem(xrcmenu, wxID_ANY, "About");
	xrcmenu->Append(item);
	menu_bar->Append(xrcmenu, "Help");


	SetMenuBar(menu_bar);



}
//---------------------------------------------------------------------------
void MainFrame::BuildToolbar()
{
	m_MainToolBar = new wxAuiToolBar(this, 555, wxDefaultPosition, wxDefaultSize,
		wxAUI_TB_DEFAULT_STYLE
		//| wxAUI_TB_OVERFLOW
		//| wxAUI_TB_TEXT
		//| wxAUI_TB_HORZ_TEXT
		| wxAUI_TB_GRIPPER
		);

	m_MainToolBar->AddTool(CMD_DB_CONNECT, "Подключиться к БД", m_ResMgr->m_ico_connect24, "Подключиться к БД", wxITEM_RADIO);
	m_MainToolBar->AddTool(CMD_DB_DISCONNECT, "Отключиться от БД", m_ResMgr->m_ico_disconnect24, "Отключиться от БД", wxITEM_RADIO);
	m_MainToolBar->AddSeparator();

	m_btnFavorites = m_MainToolBar->AddTool(CMD_SHOWFAVORITES, "Открыть избранное", m_ResMgr->m_ico_favorites24, "Открыть избранное", wxITEM_CHECK);

	m_MainToolBar->AddTool(CMD_MAKEOBJWND, "Открыть каталог объектов", m_ResMgr->m_ico_add_obj_tab24, "Открыть каталог объектов");

	Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::DoMenuPreferences, this, wxID_PREFERENCES);

	m_MainToolBar->Realize();

	m_AuiMgr.AddPane(m_MainToolBar, wxAuiPaneInfo().
		Name(wxT("m_MainToolBar")).Caption(wxT("Main toolbar"))
		.CaptionVisible(false)
		.ToolbarPane()
		.Top()
		.Fixed()
		.Dockable(false)
		.PaneBorder(false)
		.Layer(1)
		.Position(1)
		//.LeftDockable(false)
		//.RightDockable(false)
		);
}
//---------------------------------------------------------------------------

void MainFrame::BuildStatusbar()
{
	/*
	wxStatusBar *statbarNew = NULL;
	statbarNew = new wxStatusBar(this, wxID_ANY, style, "wxStatusBar");
	statbarNew->SetFieldsCount(2);
	*/

	CreateStatusBar(3);
}

//---------------------------------------------------------------------------
void MainFrame::ShowDevToolBar(bool show)
{
	if (!m_DevToolBar)
	{ 
		m_DevToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
			wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_GRIPPER);

		m_DevToolBar->AddTool(CMD_PNLSHOWGROUP, "Групп",
			m_ResMgr->m_ico_usergroup24, wxEmptyString, wxITEM_NORMAL);
		Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::PnlShowGroup, this, CMD_PNLSHOWGROUP);
		m_DevToolBar->AddTool(CMD_PNLSHOWUSER, "Пользователи",
			m_ResMgr->m_ico_user24, wxEmptyString, wxITEM_NORMAL);
		Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::PnlShowUser, this, CMD_PNLSHOWUSER);
		m_DevToolBar->AddTool(CMD_PNLSHOWPROP, "Свойства",
			m_ResMgr->m_ico_list_prop24, wxEmptyString, wxITEM_NORMAL);
		Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::PnlShowProp, this, CMD_PNLSHOWPROP);
		m_DevToolBar->AddTool(CMD_PNLSHOWACT, "Действия",
			m_ResMgr->m_ico_acts24, wxEmptyString, wxITEM_NORMAL);
		Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::PnlShowAct, this, CMD_PNLSHOWACT);
		m_DevToolBar->Realize();

		m_AuiMgr.AddPane(m_DevToolBar, wxAuiPaneInfo().Name(wxT("DevToolBar")).
			Caption(wxT("Developer toolbar")).CaptionVisible(false)
			.ToolbarPane()
			.Top()
			.Fixed()
			.Dockable(false)
			.PaneBorder(false)
			.Layer(1)
			.Position(1)
			);
	}

	BaseGroup bg = whDataMgr::GetInstance()->mCfg.Prop.mBaseGroup;
	switch (bg)
	{
	default:
	case bgNull:
		m_DevToolBar->EnableTool(CMD_PNLSHOWGROUP, false);
		m_DevToolBar->EnableTool(CMD_PNLSHOWUSER, false);
		m_DevToolBar->EnableTool(CMD_PNLSHOWPROP, false);
		m_DevToolBar->EnableTool(CMD_PNLSHOWACT, false);
		break;
	case bgAdmin:
		m_DevToolBar->EnableTool(CMD_PNLSHOWGROUP, true);
		m_DevToolBar->EnableTool(CMD_PNLSHOWUSER, true);
	case bgTypeDesigner:
		m_DevToolBar->EnableTool(CMD_PNLSHOWPROP, true);
		m_DevToolBar->EnableTool(CMD_PNLSHOWACT, true);
	case bgObjDesigner:
	case bgUser:
	case bgGuest:
		break;
	}

	show ? m_DevToolBar->Show() : m_DevToolBar->Hide();

	m_AuiMgr.Update();
}
//---------------------------------------------------------------------------
void MainFrame::DoMenuPreferences( wxCommandEvent& evt) 
{

}
//---------------------------------------------------------------------------
void MainFrame::OnMakeTypeWnd(wxCommandEvent& evt)
{
	CreateTypeCatalog("Object");
}
//---------------------------------------------------------------------------
void MainFrame::CreateTypeCatalog(const wxString& _type)
{
	//wxWindowUpdateLocker	wndDisabler(m_Notebook);
	//ListPanel* typ_panel=new ListPanel(m_Notebook);
	//typ_panel->SetParentType(_type);
	////bp1->SetDataModel(new whDir_TypeTreeFolder(whClass("Object","Object"),NULL,&mgr->mDb));
	//m_Notebook->AddPage(typ_panel,"Каталог типов");
	//
	//typ_panel->SetFocus();
	//m_AuiMgr.Update();	
}
//---------------------------------------------------------------------------
void MainFrame::CreateObjCatalog(const wxString& _objclass,const wxString& _objname,const wxString& _classfilter)
{
	wxBusyCursor		busyCursor;
	wxWindowUpdateLocker	wndUpdateLocker(m_Notebook);

	auto obj_cat = new wh::view::VObjCatalogCtrl(m_Notebook);

	auto node = std::make_shared<wh::object_catalog::MObjCatalog>();

	wh::rec::PathItem root;
	root.mCls.mID = "1";
	root.mCls.mLabel = "Object";
	root.mObj.mID = "1";
	root.mObj.mLabel = "Object0";

	node->SetData(root);
	node->MarkSaved();

	node->Load();

	obj_cat->SetModel(node);


	m_Notebook->AddPage(obj_cat, "каталог объектов");
	obj_cat->SetFocus();
	m_AuiMgr.Update();	


}
//---------------------------------------------------------------------------
void MainFrame::OnMakeObjWnd(wxCommandEvent& evt)
{
	CreateObjCatalog("Object","Object0");
}	
//---------------------------------------------------------------------------
void MainFrame::OnMakeHistoryWnd(wxCommandEvent& evt)
{
	wxBusyCursor		busyCursor;
	wxWindowUpdateLocker	wndDisabler(m_Notebook);

	whPnlCtrl_MovHistory* hist=new whPnlCtrl_MovHistory(m_Notebook);
	hist->SetLogQueryDataDefault();

	whDataMgr* mgr = whDataMgr::GetInstance();
	hist->ConnectDB(&mgr->mDb);

	m_Notebook->AddPage(hist,"История");
	hist->SetFocus();
	m_AuiMgr.Update();	

}
//---------------------------------------------------------------------------


void MainFrame::OnShowLoginWnd(wxCommandEvent& evt)
{

	
	whDataMgr* mgr = whDataMgr::GetInstance();

	if(mgr->mDb.IsOpen())
		OnDisconnectDB();
	
	wh::Cfg::DbConnect& dbcfg = whDataMgr::GetInstance()->mCfg.mConnect;
	dbcfg.Load();

	whLogin dlg(this);

	dlg.SetUserName( dbcfg.mUser);
	if( !dbcfg.mPass.IsEmpty() )
	{
		dlg.SetPass(dbcfg.mPass);
		dlg.SetStorePass(true);
	}

	if(dlg.ShowModal()==wxID_OK)
	{
		mgr->mDb.Open(dbcfg.mServer, dbcfg.mPort, dbcfg.mDB, dlg.GetUserName(),dlg.GetPass());

		if(mgr->mDb.IsOpen())
		{
			int toolId=CMD_DB_CONNECT;
			wxAuiToolBarItem* tool=	m_MainToolBar->FindTool(toolId);
			if(tool)
				tool->SetState(wxAUI_BUTTON_STATE_CHECKED);

			dbcfg.mUser = dlg.GetUserName();
			dbcfg.mPass = dlg.GetStorePass() ? dlg.GetPass(): wxEmptyString;
			
			whDataMgr::GetInstance()->mCfg.Prop.Load();

		}//if(mgr->mDb.IsOpen())
		
	
	}//if(dlg.ShowModal()==wxID_OK)
	else
		::exit(0);

	dbcfg.Save();
	
	
}
//---------------------------------------------------------------------------
void MainFrame::OnDisconnectDB(wxCommandEvent& evt)
{
	while(m_Notebook->GetPageCount())
	{
		// TODO сохранить вкладки пользователя;
		m_Notebook->DeletePage(0);
	}
	
	whDataMgr* mgr = whDataMgr::GetInstance();
	mgr->mDb.Close();

	wxAuiToolBarItem* tool=	m_MainToolBar->FindTool(CMD_DB_DISCONNECT);
	tool->SetState(wxAUI_BUTTON_STATE_CHECKED);


}

//---------------------------------------------------------------------------
void MainFrame::OnShowFavorites(wxCommandEvent& evt)
{
	wxAuiPaneInfo& pi=m_AuiMgr.GetPane("favorites_pane");

	if(pi.IsOk())
	{
		bool visible = !pi.IsShown();
		pi.Show(visible);
		
		wxAuiToolBarItem* tool=	m_MainToolBar->FindTool(CMD_SHOWFAVORITES);
		visible ?  tool->SetState(wxAUI_BUTTON_STATE_CHECKED) : tool->SetState(wxAUI_BUTTON_STATE_NORMAL) ;
		
		int mnu=GetMenuBar()->FindMenuItem("Вид","Показать/скрыть избранное");
		wxMenuItem* itm=GetMenuBar()->FindItem(mnu);
		itm->Check(visible);
		
		m_AuiMgr.Update();	
	}//if(!pi.IsOk())
	
}

//---------------------------------------------------------------------------
void MainFrame::PnlShowGroup(wxCommandEvent& WXUNUSED(evt))
{
	wxWindowUpdateLocker	wndUpdateLocker(m_Notebook);
	using namespace wh;
	using namespace wh::view;
	auto model = std::make_shared<MGroupArray>();
	auto view = new VGroupCtrlPanel(m_Notebook);
	BaseGroup bg = whDataMgr::GetInstance()->mCfg.Prop.mBaseGroup;
	if ((int)bg < (int)bgAdmin)
		view->DisableCreateDelete();
	model->Load();
	view->SetModel(model);
	m_Notebook->AddPage(view, "Группы", true, ResMgr::GetInstance()->m_ico_usergroup24);
	view->SetFocus();
	m_AuiMgr.Update();
}
//---------------------------------------------------------------------------
void MainFrame::PnlShowUser(wxCommandEvent& WXUNUSED(evt))
{
	wxWindowUpdateLocker	wndUpdateLocker(m_Notebook);
	using namespace wh;
	using namespace wh::view;
	auto model = std::make_shared<MUserArray>();
	auto view = new VUserCtrlPanel(m_Notebook);
	BaseGroup bg = whDataMgr::GetInstance()->mCfg.Prop.mBaseGroup;
	if ((int)bg < (int)bgAdmin)
		view->DisableCreateDelete();
	model->Load();
	view->SetModel(model);
	m_Notebook->AddPage(view, "Пользователи", true, ResMgr::GetInstance()->m_ico_user24);
	view->SetFocus();
	m_AuiMgr.Update();
}
//---------------------------------------------------------------------------
void MainFrame::PnlShowProp(wxCommandEvent& WXUNUSED(evt))
{
	wxWindowUpdateLocker	wndUpdateLocker(m_Notebook);
	using namespace wh;
	using namespace wh::view;
	auto model = std::make_shared<MPropArray>();
	auto view = new VPropCtrlPanel(m_Notebook);
	BaseGroup bg = whDataMgr::GetInstance()->mCfg.Prop.mBaseGroup;
	if ((int)bg < (int)bgTypeDesigner)
		view->DisableCreateDelete();
	model->Load();
	view->SetModel(model);
	m_Notebook->AddPage(view, "Свойства", true, ResMgr::GetInstance()->m_ico_list_prop24);
	view->SetFocus();
	m_AuiMgr.Update();
}
//---------------------------------------------------------------------------
void MainFrame::PnlShowAct(wxCommandEvent& WXUNUSED(evt))
{
	wxWindowUpdateLocker	wndUpdateLocker(m_Notebook);
	using namespace wh;
	using namespace wh::view;
	auto model = std::make_shared<MActArray>();
	auto view = new VActCtrlPanel(m_Notebook);
	BaseGroup bg = whDataMgr::GetInstance()->mCfg.Prop.mBaseGroup;
	if ((int)bg < (int)bgTypeDesigner)
		view->DisableCreateDelete();
	model->Load();
	view->SetModel(model);
	m_Notebook->AddPage(view, "Действия", true, ResMgr::GetInstance()->m_ico_acts24);
	view->SetFocus();
	m_AuiMgr.Update();
}

