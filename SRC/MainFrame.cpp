#include "_pch.h"
#include "MainFrame.h"


#include "whLogin.h"

#include "whPnlCtrl_MovHistory.h"

#include "favorites.h"



#include "AdminMainForm.h"
#include "VObjCatalogCtrl.h"


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
	: wxFrame( parent, ID_MAINFRAME, "wxPGTest", wxDefaultPosition, wxSize( 700,600 ), wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL )
{
	
	
	m_wndFavorites =NULL;

	


	this->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );


	m_AuiMgr.SetManagedWindow(this);

	wxMenuBar*	menu_bar = new wxMenuBar(0);
	wxMenuItem* item= NULL;

	wxMenu* dbmenu = new wxMenu();
	item = new wxMenuItem(dbmenu,CMD_DB_CONNECT,"Подключиться к БД");
	item->SetBitmap(m_ResMgr->m_ico_connect16);	
    dbmenu->Append(item);
	item = new wxMenuItem(dbmenu,CMD_DB_DISCONNECT, "Отключиться от БД");
	item->SetBitmap(m_ResMgr->m_ico_disconnect16);
    dbmenu->Append(item);
	dbmenu->AppendSeparator();
	item = new wxMenuItem(dbmenu,wxID_PREFERENCES,"Конфигурация");
	item->SetBitmap(m_ResMgr->m_ico_db16);
    dbmenu->Append(item);
	dbmenu->AppendSeparator();
	item = new wxMenuItem(dbmenu,wxID_EXIT, "Выход");
	dbmenu->Append(item);
	menu_bar->Append(dbmenu, "Подключение");

	wxMenu* view = new wxMenu();
	item = new wxMenuItem(view,CMD_SHOWFAVORITES,"Показать/скрыть избранное",wxEmptyString,wxITEM_CHECK);
	//item->SetBitmap(m_ResMgr->m_ico_favorites16);
	view->Append(item);
	menu_bar->Append(view, "Вид");

	
	wxMenu* dir = new wxMenu();
	item = new wxMenuItem(dir,CMD_MAKEOBJWND,"Открыть каталог объектов" );
    dir->Append(item);
	item = new wxMenuItem(dir,CMD_MAKETYPEWND,"Открыть каталог типов");
    dir->Append(item);
	item = new wxMenuItem(dir,CMD_MAKEHISTORYWND,"Открыть историю");
    dir->Append(item);
	menu_bar->Append(dir, "Каталоги данных");


	wxMenu* xrcmenu = new wxMenu();
	item = new wxMenuItem(xrcmenu,wxID_ANY,"About");
    xrcmenu->Append(item);
	menu_bar->Append(xrcmenu, "Help");


	SetMenuBar(menu_bar);

	


    m_MainToolBar = new wxAuiToolBar(this, 555, wxDefaultPosition, wxDefaultSize,
															wxAUI_TB_DEFAULT_STYLE 
															//| wxAUI_TB_OVERFLOW
															//| wxAUI_TB_TEXT
															//| wxAUI_TB_HORZ_TEXT
															//| wxAUI_TB_GRIPPER
															);

	//wxAuiToolBarArt* art=m_MainToolBar->GetArtProvider();
	//wxVisualAttributes atr=	m_MainToolBar->GetDefaultAttributes();
	//m_MainToolBar->SetDefaultAttributes(atr);

	//m_MainToolBar->SetToolBitmapSize(wxSize(48,48));

	//wxAuiToolBarItem* tool=	
	m_MainToolBar->AddTool(CMD_DB_CONNECT,"Подключиться к БД",m_ResMgr->m_ico_connect24,"Подключиться к БД",wxITEM_RADIO);
	m_MainToolBar->AddTool(CMD_DB_DISCONNECT,"Отключиться от БД",m_ResMgr->m_ico_disconnect24,"Отключиться от БД",wxITEM_RADIO);
	m_MainToolBar->AddSeparator();

	m_btnFavorites=m_MainToolBar->AddTool(CMD_SHOWFAVORITES,"Открыть избранное",m_ResMgr->m_ico_favorites24,"Открыть избранное",wxITEM_CHECK);
	
	m_MainToolBar->AddTool(CMD_MAKEOBJWND,"Открыть каталог объектов",m_ResMgr->m_AddObjTab24,"Открыть каталог объектов");
	m_MainToolBar->AddTool(CMD_MAKETYPEWND,"Открыть каталог типов",m_ResMgr->m_AddTypeTab24,"Открыть каталог типов");
	
	
	Bind(wxEVT_COMMAND_MENU_SELECTED,&MainFrame::DoMenuPreferences,this,wxID_PREFERENCES);


	
	m_MainToolBar->Realize();

	// create the notebook off-window to avoid flicker
	wxSize client_size = GetClientSize();
	m_Notebook = new wxAuiNotebook(this, wxID_ANY,
                                    wxPoint(client_size.x, client_size.y),
                                    wxSize(430,200),
									wxAUI_NB_DEFAULT_STYLE  | wxAUI_NB_TAB_EXTERNAL_MOVE | wxNO_BORDER);
    
	m_AuiMgr.AddPane(m_MainToolBar, wxAuiPaneInfo().
		Name(wxT("m_MainToolBar")).Caption(wxT("Main toolbar"))
					.CaptionVisible(false)
					 //ToolbarPane().
					 .Top()
					 .Fixed()
					 .Dockable(false)
					 .PaneBorder(false)
					 .Layer(1)
					 .Position(1)
					 //.LeftDockable(false)
					 //.RightDockable(false)
					 );

	m_AuiMgr.AddPane(m_Notebook, wxAuiPaneInfo().
					Name(wxT("NotebookPane")).Caption(wxT("Notebook")).
					CenterPane().Layer(1).Position(1).CloseButton(true).MaximizeButton(true).PaneBorder(false));


/*
	wxStatusBar *statbarNew = NULL;
	statbarNew = new wxStatusBar(this, wxID_ANY, style, "wxStatusBar");
	statbarNew->SetFieldsCount(2);
*/
	CreateStatusBar(3);
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

	DoMenuPreferences();


	//wh::rdbs::RunTest();
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
	mgr->m_DB.Close();
	mgr->FreeInst();

}
//---------------------------------------------------------------------------
void MainFrame::DoMenuPreferences( wxCommandEvent& evt) 
{
	wh::gui::AdminMainForm admin;
	admin.ShowModal();
}
//---------------------------------------------------------------------------
void MainFrame::OnMakeTypeWnd(wxCommandEvent& evt)
{
	CreateTypeCatalog("Object");
}
//---------------------------------------------------------------------------
void MainFrame::CreateTypeCatalog(const wxString& _type)
{
	//m_Notebook->Freeze();
	//ListPanel* typ_panel=new ListPanel(m_Notebook);
	//typ_panel->SetParentType(_type);
	////bp1->SetDataModel(new whDir_TypeTreeFolder(whClass("Object","Object"),NULL,&mgr->m_DB));
	//m_Notebook->AddPage(typ_panel,"Каталог типов");
	//
	//m_Notebook->Thaw();
	//typ_panel->SetFocus();
	//m_AuiMgr.Update();	
}
//---------------------------------------------------------------------------
void MainFrame::CreateObjCatalog(const wxString& _objclass,const wxString& _objname,const wxString& _classfilter)
{
	m_Notebook->Freeze();

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
	m_Notebook->Thaw();
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
	m_Notebook->Freeze();
	whPnlCtrl_MovHistory* hist=new whPnlCtrl_MovHistory(m_Notebook);
	hist->SetLogQueryDataDefault();

	whDataMgr* mgr = whDataMgr::GetInstance();
	hist->ConnectDB(&mgr->m_DB);
	mgr->FreeInst();

	m_Notebook->AddPage(hist,"История");
	m_Notebook->Thaw();
	hist->SetFocus();
	m_AuiMgr.Update();	

}
//---------------------------------------------------------------------------


void MainFrame::OnShowLoginWnd(wxCommandEvent& evt)
{

	
	whDataMgr* mgr = whDataMgr::GetInstance();

	if(mgr->m_DB.IsOpen())
		OnDisconnectDB();
	
	wh::Cfg::DbConnect& dbcfg = wh::Cfg::Instance()->Db;
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
		mgr->m_DB.Open(dbcfg.mServer, dbcfg.mPort, dbcfg.mDB, dlg.GetUserName(),dlg.GetPass());

		if(mgr->m_DB.IsOpen())
		{
			int toolId=CMD_DB_CONNECT;
			wxAuiToolBarItem* tool=	m_MainToolBar->FindTool(toolId);
			if(tool)
				tool->SetState(wxAUI_BUTTON_STATE_CHECKED);

			dbcfg.mUser = dlg.GetUserName();
			dbcfg.mPass = dlg.GetStorePass() ? dlg.GetPass(): wxEmptyString;
			
			wh::Cfg::Instance()->Prop.Load();

		}//if(mgr->m_DB.IsOpen())
		
	
	}//if(dlg.ShowModal()==wxID_OK)
	else
		::exit(0);

	dbcfg.Save();
	
	mgr->FreeInst();
	
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
	mgr->m_DB.Close();
	mgr->FreeInst();

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


	