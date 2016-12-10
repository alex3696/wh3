#include "_pch.h"
#include "MainFrame.h"


#include "whLogin.h"
#include "ConnectionCfgDlg.h"

#include "VObjCatalogCtrl.h"

#include "VPropCtrlPanel.h"
#include "VActCtrlPanel.h"
#include "VGroupCtrlPanel.h"
#include "VUserCtrlPanel.h"


#include "VTablePanel.h"
#include "MProp2.h"
#include "MHistory.h"
#include "config.h"
#include "AppViewConfig.h"

#include "detail_ctrlpnl.h"

#include "EmptyPresenter.h"
#include "NotebookModel.h"
#include "NotebookPresenter.h"
#include "NotebookView.h"

//---------------------------------------------------------------------------
enum GUIID
{
	// GUI controls IDs
	ID_MAINFRAME = 1100,

	// CMD
	CMD_DB_CONNECT,
	CMD_DB_DISCONNECT,

	CMD_MAKETYPEWND,
	CMD_MAKEOBJWND,
	CMD_MAKEHISTORYWND,

	CMD_VIEW_TOOGLE_MAINTOOLBAR,

	CMD_MKTAB_FAVORITES,

	CMD_PNLSHOWGROUP,
	CMD_PNLSHOWUSER,
	CMD_PNLSHOWPROP,
	CMD_PNLSHOWACT

};
//---------------------------------------------------------------------------
MainFrame::MainFrame(	wxWindow* parent, wxWindowID id, const wxString& title, 
						const wxPoint& pos, const wxSize& size, long style)
	: wxFrame(parent, ID_MAINFRAME, title, wxDefaultPosition, size, style)
{
	m_AuiMgr.SetManagedWindow(this);

	auto face_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
	m_AuiMgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, face_colour);

	Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCmd_MkTabFaforite, this, CMD_MKTAB_FAVORITES);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCmd_ToogleViewMainToolbar, this, CMD_VIEW_TOOGLE_MAINTOOLBAR);
	
	Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCmd_MakePage_Group, this, CMD_PNLSHOWGROUP);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCmd_MakePage_User, this, CMD_PNLSHOWUSER);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCmd_MakePage_Prop, this, CMD_PNLSHOWPROP);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCmd_MakePage_Act, this, CMD_PNLSHOWACT);
	
	Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCmd_MakePage_ObjByType, this, CMD_MAKETYPEWND);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCmd_MakePage_ObjByPath, this, CMD_MAKEOBJWND);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCmd_MakePage_History, this, CMD_MAKEHISTORYWND);
	

	Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCmd_ConnectDB, this, CMD_DB_CONNECT);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCmd_DisconnectDB, this, CMD_DB_DISCONNECT);

	BuildMenu();
	BuildStatusbar();
	BuildToolbar();
	BuildDevToolBar();

	m_Notebook = new wxAuiNotebook(this);
	m_AuiMgr.AddPane(m_Notebook, wxAuiPaneInfo().
					Name(wxT("NotebookPane")).Caption(wxT("NotebookPane")).
					CenterPane().Layer(1).Position(1).CloseButton(true).MaximizeButton(true).PaneBorder(false));
	m_AuiMgr.Update();

	auto dmgr = whDataMgr::GetInstance();
	namespace ph = std::placeholders;
	
	wh::MBaseGroup::Slot fnAC_basegroup = 
		std::bind(&MainFrame::OnSigAfterChange_BaseGroup, this, ph::_1, ph::_2);
	mSSC_AfterChange_BaseGroup =
		dmgr->mDbCfg->mBaseGroup
		->DoConnect(wh::ModelOperation::moAfterUpdate, fnAC_basegroup);
	
	auto fnAC = std::bind(&MainFrame::OnSig_AfterDbConnected, this, ph::_1);
	auto fnBD = std::bind(&MainFrame::OnSig_BeforeDbDisconnect, this, ph::_1);
	mSSC_AfterDbConnected = dmgr->GetDB().SigAfterConnect.connect(fnAC);
	mSSC_BeforeDbDisconnect = dmgr->GetDB().SigBeforeDisconnect.connect(fnBD);


	auto fnAI_page = std::bind(&MainFrame::OnSigAfterInsertPage, this, ph::_1, ph::_2, ph::_3);
	mSSC_AfterInsert_Page = dmgr->mDbCfg->mGuiCfg->mNotepadCfg->ConnAfterInsert(fnAI_page);
	

	//auto ep = new EmptyPresenter();
	//auto ev = new EmptyView(ep);
	//ev->SetWindow(this);
	//ep->SetView(ev);


	//auto new_wnd = whDataMgr::GetInstance()->mNotebookPresenter->GetView()->GetWnd();

	//m_AuiMgr.AddPane(new_wnd, wxAuiPaneInfo().
	//	Name(wxT("TestNotebookPane")).Caption(wxT("TestNotebookPane"))
	//	.Left().MinSize(300,300));
	//m_AuiMgr.Update();
	
}
//---------------------------------------------------------------------------

MainFrame::~MainFrame()
{
	m_AuiMgr.UnInit();
}
//---------------------------------------------------------------------------

void MainFrame::BuildMenu()
{
	wxMenuBar*	menu_bar = new wxMenuBar(0);
	wxMenuItem* item = NULL;

	wxMenu* dbmenu = new wxMenu();
	item = new wxMenuItem(dbmenu, wxID_SETUP, "Настройки соединения");
	dbmenu->Append(item);
	item = new wxMenuItem(dbmenu, CMD_DB_CONNECT, "Подключиться к БД");
	item->SetBitmap(m_ResMgr->m_ico_connect16);
	dbmenu->Append(item);
	item = new wxMenuItem(dbmenu, CMD_DB_DISCONNECT, "Отключиться от БД");
	item->SetBitmap(m_ResMgr->m_ico_disconnect16);
	dbmenu->Append(item);
	dbmenu->AppendSeparator();
	item = new wxMenuItem(dbmenu, wxID_EXIT, "Выход");
	dbmenu->Append(item);
	menu_bar->Append(dbmenu, "Подключение");

	wxMenu* menu_cfg = new wxMenu();
	item = new wxMenuItem(menu_cfg, CMD_VIEW_TOOGLE_MAINTOOLBAR, "Показать/скрыть основную панель", wxEmptyString, wxITEM_CHECK);
	menu_cfg->Append(item);
	menu_cfg->AppendSeparator();
	item = new wxMenuItem(menu_cfg, wxID_PREFERENCES, "...");
	menu_cfg->Append(item);
	menu_bar->Append(menu_cfg, "Вид");

	wxMenu* dir = new wxMenu();
	item = new wxMenuItem(dir, CMD_PNLSHOWGROUP, "Группы пользователей");
	item->SetBitmap(m_ResMgr->m_ico_usergroup16);
	dir->Append(item);
	item = new wxMenuItem(dir, CMD_PNLSHOWUSER, "Пользователи");
	item->SetBitmap(m_ResMgr->m_ico_user16);
	dir->Append(item);
	dir->AppendSeparator();
	item = new wxMenuItem(dir, CMD_PNLSHOWPROP, "Свойства");
	item->SetBitmap(m_ResMgr->m_ico_list_prop16);
	dir->Append(item);
	item = new wxMenuItem(dir, CMD_PNLSHOWACT, "Действия");
	item->SetBitmap(m_ResMgr->m_ico_acts16);
	dir->Append(item);
	dir->AppendSeparator();
	item = new wxMenuItem(dir, CMD_MAKEHISTORYWND, "Общая история");
	item->SetBitmap(m_ResMgr->m_ico_history24);
	dir->Append(item);
	item = new wxMenuItem(dir, CMD_MAKEOBJWND, "Каталог объектов по местоположению");
	item->SetBitmap(m_ResMgr->m_ico_add_obj_tab24);
	dir->Append(item);
	item = new wxMenuItem(dir, CMD_MAKETYPEWND, "Каталог объектов по типу");
	item->SetBitmap(m_ResMgr->m_ico_add_obj_tab24);
	dir->Append(item);
	item = new wxMenuItem(dir, CMD_MKTAB_FAVORITES, "Показать избранное");
	item->SetBitmap(m_ResMgr->m_ico_favorites24);
	dir->Append(item);
	menu_bar->Append(dir, "Каталоги");

	wxMenu* menu_report = new wxMenu();
	item = new wxMenuItem(menu_report, wxID_ANY, "Редактор отчётов");
	menu_report->Append(item);
	item = new wxMenuItem(menu_report, wxID_ANY, "Список отчётов");
	menu_report->Append(item);
	menu_bar->Append(menu_report, "Отчёты");


	SetMenuBar(menu_bar);

	Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::DoMenuPreferences, this, wxID_PREFERENCES);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::DoShowConnDlg, this, wxID_SETUP);

}
//---------------------------------------------------------------------------
void MainFrame::BuildToolbar()
{
	long main_toolbar_style = wxAUI_TB_DEFAULT_STYLE
		//| wxAUI_TB_OVERFLOW
		//| wxAUI_TB_TEXT
		//| wxAUI_TB_HORZ_TEXT
		| wxAUI_TB_PLAIN_BACKGROUND
		| wxAUI_TB_GRIPPER;
	
	m_MainToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, main_toolbar_style);

	//m_MainToolBar->AddTool(CMD_DB_CONNECT, "Подключиться к БД", m_ResMgr->m_ico_connect24, "Подключиться к БД", wxITEM_RADIO);
	//m_MainToolBar->AddTool(CMD_DB_DISCONNECT, "Отключиться от БД", m_ResMgr->m_ico_disconnect24, "Отключиться от БД", wxITEM_RADIO);
	//m_MainToolBar->AddSeparator();

	const wxString str_mktab_objdir = "Каталог объектов по местоположению";
	m_MainToolBar->AddTool(CMD_MAKEOBJWND, str_mktab_objdir
		, m_ResMgr->m_ico_folder_obj24, str_mktab_objdir);

	const wxString cat_obj_by_type = "Каталог объектов по типу";
	m_MainToolBar->AddTool(CMD_MAKETYPEWND, cat_obj_by_type
		, m_ResMgr->m_ico_folder_type24, cat_obj_by_type);


	const wxString str_mktab_favorites = "Открыть избранное";
	m_MainToolBar->AddTool(CMD_MKTAB_FAVORITES, str_mktab_favorites
		, m_ResMgr->m_ico_favorites24, str_mktab_favorites);
	
	m_MainToolBar->Realize();

	m_AuiMgr.AddPane(m_MainToolBar, wxAuiPaneInfo().
		Name(wxT("mMainToolBar")).Caption(wxT("mMainToolBar"))
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
void MainFrame::BuildDevToolBar()
{
	if (!m_DevToolBar)
	{ 
		m_DevToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
			wxAUI_TB_DEFAULT_STYLE 
			| wxAUI_TB_PLAIN_BACKGROUND
			| wxAUI_TB_GRIPPER);

		m_DevToolBar->AddTool(CMD_PNLSHOWGROUP, "Группы", m_ResMgr->m_ico_usergroup24);
		m_DevToolBar->AddTool(CMD_PNLSHOWUSER, "Пользователи", m_ResMgr->m_ico_user24);
		m_DevToolBar->AddTool(CMD_PNLSHOWPROP, "Свойства", m_ResMgr->m_ico_list_prop24);
		m_DevToolBar->AddTool(CMD_PNLSHOWACT, "Действия", m_ResMgr->m_ico_acts24);
		m_DevToolBar->AddTool(CMD_MAKEHISTORYWND, "История", m_ResMgr->m_ico_history24);

		m_DevToolBar->Realize();

		m_AuiMgr.AddPane(m_DevToolBar
			, wxAuiPaneInfo().Name(wxT("DevToolBar"))
			.Caption(wxT("DevToolBar")).CaptionVisible(false)
			.ToolbarPane()
			.Top()
			.Fixed()
			.Dockable(false)
			.PaneBorder(false)
			.Layer(1)
			.Position(1)
			);
	}
}
//---------------------------------------------------------------------------
void MainFrame::DoShowConnDlg(wxCommandEvent& evt)
{
	ConnectionCfgDlg conn_dlg(this);
	conn_dlg.ShowModal();
}
//---------------------------------------------------------------------------
void MainFrame::DoMenuPreferences( wxCommandEvent& evt) 
{

}

//---------------------------------------------------------------------------

void MainFrame::OnCmd_ConnectDB(wxCommandEvent& evt)
{
	whLogin dlg(this);
	dlg.ShowModal();
}
//---------------------------------------------------------------------------
void MainFrame::OnCmd_DisconnectDB(wxCommandEvent& evt)
{
	whDataMgr* mgr = whDataMgr::GetInstance();
	mgr->mDb.Close();
}

//---------------------------------------------------------------------------
void MainFrame::OnCmd_MkTabFaforite(wxCommandEvent& evt)
{
	wxWindowUpdateLocker	wndUpdateLocker(m_Notebook);
	using namespace wh;

	auto view = new wxPanel(m_Notebook);

	m_Notebook->AddPage(view, "Избранное", true, ResMgr::GetInstance()->m_ico_favorites24);
	view->SetFocus();
	m_AuiMgr.Update();

}
//---------------------------------------------------------------------------

void MainFrame::OnCmd_ToogleViewMainToolbar(wxCommandEvent& evt)
{
	if (evt.IsChecked())
	{
		wxAuiPaneInfo&  pi_main = m_AuiMgr.GetPane(m_MainToolBar);
		if (pi_main.IsOk())
			pi_main.Show();

		wxAuiPaneInfo&  pi_dev = m_AuiMgr.GetPane(m_DevToolBar);
		if (pi_dev.IsOk())
			pi_dev.Show();
	}
	else
	{
		wxAuiPaneInfo&  pi_main = m_AuiMgr.GetPane(m_MainToolBar);
		if (pi_main.IsOk())
			pi_main.Hide();

		wxAuiPaneInfo&  pi_dev = m_AuiMgr.GetPane(m_DevToolBar);
		if (pi_dev.IsOk())
			pi_dev.Hide();

	}

	m_AuiMgr.Update();
}
//---------------------------------------------------------------------------

void MainFrame::AddTab(wxWindow* wnd,const wxString& lbl, const wxIcon& icon)
{
	if (!wnd)
		return;
	wxBusyCursor busyCursor;
	//wxWindowUpdateLocker	wndUpdateLocker(m_Notebook);
	wxWindowUpdateLocker	wndUpdateLocker(this);
	m_Notebook->AddPage(wnd, lbl, true, icon);
	//wnd->SetFocus();
	m_AuiMgr.Update();
}

//---------------------------------------------------------------------------
void MainFrame::UpdateTab(wxWindow* wnd, const wxString& lbl, const wxIcon& icon)
{
	if (!wnd)
		return;
	wxWindowUpdateLocker	wndUpdateLocker(m_Notebook);
	auto page_idx = m_Notebook->GetPageIndex(wnd);
	if (wxNOT_FOUND != page_idx)
	{
		m_Notebook->SetPageBitmap(page_idx, icon);
		m_Notebook->SetPageText(page_idx, lbl);
		m_Notebook->Update();
	}
	m_AuiMgr.Update();
}
//---------------------------------------------------------------------------

void MainFrame::OnSig_AfterDbConnected(const whDB&)
{
	const auto& dbcfg = whDataMgr::GetInstance()->mConnectCfg->GetData();
	
	//wxAuiToolBarItem* tool = m_MainToolBar->FindTool(CMD_DB_CONNECT);
	//if (tool)
	//	tool->SetState(wxAUI_BUTTON_STATE_CHECKED);

	const wxString conn_str = wxString::Format("%s %s %d %s"
		, dbcfg.mUser, dbcfg.mServer, dbcfg.mPort, dbcfg.mDB);
	SetStatusText(conn_str, 0);

	wxMenuBar* menu_bar = this->GetMenuBar();
	if (menu_bar)
	{
		int catalog_menu_id = menu_bar->FindMenu("Каталоги");
		if (wxNOT_FOUND != catalog_menu_id)
			menu_bar->EnableTop(catalog_menu_id, true);
	}
	m_MainToolBar->Enable();
	m_DevToolBar->Enable();

}
//---------------------------------------------------------------------------

void MainFrame::OnSig_BeforeDbDisconnect(const whDB&)
{
	

	const auto& dbcfg = whDataMgr::GetInstance()->mConnectCfg->GetData();

	//wxAuiToolBarItem* tool = m_MainToolBar->FindTool(CMD_DB_CONNECT);
	//if (tool)
	//	tool->SetState(wxAUI_BUTTON_STATE_NORMAL);

	const wxString conn_str = wxString::Format("DISCONNECTED %s %d %s"
		, dbcfg.mServer, dbcfg.mPort, dbcfg.mDB);
	SetStatusText(conn_str, 0);

	wxMenuBar* menu_bar = this->GetMenuBar();
	if (menu_bar)
	{
		int catalog_menu_id = menu_bar->FindMenu("Каталоги");
		if (wxNOT_FOUND != catalog_menu_id)
			menu_bar->EnableTop(catalog_menu_id, false);
	}
	m_MainToolBar->Disable();
	m_DevToolBar->Disable();

	while (m_Notebook->GetPageCount())
	{
		// TODO сохранить вкладки пользователя;
		m_Notebook->DeletePage(0);
	}

}
//---------------------------------------------------------------------------

void MainFrame::OnSigAfterChange_BaseGroup(const wh::IModel*, const BaseGroup* const pbg)
{
	/*
	BaseGroup bg = pbg ? *pbg : bgNull;
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
	m_AuiMgr.Update();
	*/
}
//---------------------------------------------------------------------------

void MainFrame::OnCmd_MakePage_Group(wxCommandEvent& evt)
{
	auto notepadCfg = whDataMgr::GetInstance()->mDbCfg->mGuiCfg->mNotepadCfg;
	auto model = std::make_shared<wh::MPageGroup>();
	model->SetData(wh::rec::PageGroup());
	notepadCfg->Insert(model);

	//auto mdl= mNotebookPresenter->GetModel();
	//mNotebookPresenter->SetModel(mdl);
	whDataMgr::GetInstance()->mNotebookPresenter->DoAddPage(wh::rec::PageGroup());
}
//---------------------------------------------------------------------------

void MainFrame::OnCmd_MakePage_User(wxCommandEvent& evt)
{
	auto notepadCfg = whDataMgr::GetInstance()->mDbCfg->mGuiCfg->mNotepadCfg;
	auto model = std::make_shared<wh::MPageUser>();
	model->SetData(wh::rec::PageUser());
	notepadCfg->Insert(model);

	whDataMgr::GetInstance()->mNotebookPresenter->DoAddPage(wh::rec::PageUser());
}
//---------------------------------------------------------------------------

void MainFrame::OnCmd_MakePage_Prop(wxCommandEvent& evt)
{
	auto notepadCfg = whDataMgr::GetInstance()->mDbCfg->mGuiCfg->mNotepadCfg;
	auto model = std::make_shared<wh::MPageProp>();
	model->SetData(wh::rec::PageProp());
	notepadCfg->Insert(model);

	whDataMgr::GetInstance()->mNotebookPresenter->DoAddPage(wh::rec::PageProp());
}
//---------------------------------------------------------------------------

void MainFrame::OnCmd_MakePage_Act(wxCommandEvent& evt)
{
	auto notepadCfg = whDataMgr::GetInstance()->mDbCfg->mGuiCfg->mNotepadCfg;
	auto model = std::make_shared<wh::MPageAct>();
	model->SetData(wh::rec::PageAct());
	notepadCfg->Insert(model);

	whDataMgr::GetInstance()->mNotebookPresenter->DoAddPage(wh::rec::PageAct());
}
//---------------------------------------------------------------------------

void MainFrame::OnCmd_MakePage_ObjByType(wxCommandEvent& evt)
{
	auto notepadCfg = whDataMgr::GetInstance()->mDbCfg->mGuiCfg->mNotepadCfg;

	bool hideSysRoot = true;
	const auto& currBaseGroup = whDataMgr::GetInstance()->mDbCfg->mBaseGroup->GetData();
	if ((int)bgUser < (int)currBaseGroup)
		hideSysRoot = false;
	
	wh::rec::PageObjByType page_obj_by_type;
	page_obj_by_type.mParent_Cid = hideSysRoot? 1 : 0;

	auto model_page_obj_by_type = std::make_shared<wh::MPageObjByType>();
	model_page_obj_by_type->SetData(page_obj_by_type);

	notepadCfg->Insert(model_page_obj_by_type);

	whDataMgr::GetInstance()->mNotebookPresenter->DoAddPage(page_obj_by_type);
}
//---------------------------------------------------------------------------

void MainFrame::OnCmd_MakePage_ObjByPath(wxCommandEvent& evt)
{
	auto notepadCfg = whDataMgr::GetInstance()->mDbCfg->mGuiCfg->mNotepadCfg;

	bool hideSysRoot = true;
	const auto& currBaseGroup = whDataMgr::GetInstance()->mDbCfg->mBaseGroup->GetData();
	if ((int)bgUser < (int)currBaseGroup)
		hideSysRoot = false;


	wh::rec::PageObjByPath page_obj_by_path;
	page_obj_by_path.mParent_Oid = hideSysRoot ? 1 : 0;

	auto model_page_obj_by_path = std::make_shared<wh::MPageObjByPath>();
	model_page_obj_by_path->SetData(page_obj_by_path);

	notepadCfg->Insert(model_page_obj_by_path);

	whDataMgr::GetInstance()->mNotebookPresenter->DoAddPage(page_obj_by_path);
}
//---------------------------------------------------------------------------

void MainFrame::OnCmd_MakePage_History(wxCommandEvent& evt)
{
	auto notepadCfg = whDataMgr::GetInstance()->mDbCfg->mGuiCfg->mNotepadCfg;
	auto model = std::make_shared<wh::MPageHistory>();
	model->SetData(wh::rec::PageHistory());
	notepadCfg->Insert(model);

	whDataMgr::GetInstance()->mNotebookPresenter->DoAddPage(wh::rec::PageHistory());
}
//---------------------------------------------------------------------------

void MainFrame::OnSigAfterInsertPage(const wh::IModel& notepad_imodel
	, const std::vector<wh::SptrIModel>& newItems, const wh::SptrIModel& itemBefore)
{
	wxBusyCursor			busyCursor;
	wxWindowUpdateLocker	wndUpdateLocker(m_Notebook);


	auto notepad_model = dynamic_cast<const wh::MNotepadCfg*>(&notepad_imodel);
		
	for (const auto& curr : newItems)
	{
		bool ok = false;
		
		if (MakePage<wh::MPageUser>(curr))
			continue;
		if (MakePage<wh::MPageGroup>(curr))
			continue;
		if (MakePage<wh::MPageProp>(curr))
			continue;
		if (MakePage<wh::MPageAct>(curr))
			continue;
		if (MakePage<wh::MPageObjByType>(curr))
			continue;
		if (MakePage<wh::MPageObjByPath>(curr))
			continue;
		if (MakePage<wh::MPageObjDetail>(curr))
			continue;
		if (MakePage<wh::MPageHistory>(curr))
			continue;

	}

}
//---------------------------------------------------------------------------

void MainFrame::MakePage(const wh::rec::PageUser& cfg)
{
	wxWindowUpdateLocker	wndUpdateLocker(m_Notebook);
	using namespace wh;
	using namespace wh::view;
	auto model = std::make_shared<MUserArray>();
	auto view = new VUserCtrlPanel(m_Notebook);
	const auto& bg = whDataMgr::GetInstance()->mDbCfg->mBaseGroup->GetData();
	if ((int)bg < (int)bgAdmin)
		view->DisableCreateDelete();
	model->Load();
	view->SetModel(model);
	m_Notebook->AddPage(view, "Пользователи", true, ResMgr::GetInstance()->m_ico_user24);
	view->SetFocus();
	m_AuiMgr.Update();
}
//---------------------------------------------------------------------------

void MainFrame::MakePage(const wh::rec::PageGroup& cfg)
{
	wxWindowUpdateLocker	wndUpdateLocker(m_Notebook);
	using namespace wh;
	using namespace wh::view;
	auto model = std::make_shared<MGroupArray>();
	auto view = new VGroupCtrlPanel(m_Notebook);
	const auto& bg = whDataMgr::GetInstance()->mDbCfg->mBaseGroup->GetData();
	if ((int)bg < (int)bgAdmin)
		view->DisableCreateDelete();
	model->Load();
	view->SetModel(model);
	m_Notebook->AddPage(view, "Группы", true, ResMgr::GetInstance()->m_ico_usergroup24);
	view->SetFocus();
	m_AuiMgr.Update();
}
//---------------------------------------------------------------------------

void MainFrame::MakePage(const wh::rec::PageProp& cfg)
{
	wxBusyCursor			busyCursor;
	m_Notebook->Freeze(); //wxWindowUpdateLocker	wndUpdateLocker(m_Notebook);
	auto wnd = new wh::VTablePanel(m_Notebook);
	auto model = std::make_shared<wh::MPropTable>();
	const auto currBaseGroup = whDataMgr::GetInstance()->mDbCfg->mBaseGroup->GetData();
	if ((int)currBaseGroup <= (int)bgUser)
	{
		wnd->mCtrl.fnOnCmdSave = nullptr; //wnd->SetEnableSave(false);
		wnd->mCtrl.fnOnCmdInsert = nullptr;
		wnd->mCtrl.fnOnCmdEdit = nullptr;
		wnd->mCtrl.fnOnCmdRemove = nullptr;
	}

	m_Notebook->AddPage(wnd, "Свойства", true, ResMgr::GetInstance()->m_ico_list_prop24);
	m_AuiMgr.Update();
	m_Notebook->Thaw();
	wnd->SetModel(model);
	model->Load();

}
//---------------------------------------------------------------------------

void MainFrame::MakePage(const wh::rec::PageAct& cfg)
{
	wxWindowUpdateLocker	wndUpdateLocker(m_Notebook);
	using namespace wh;
	using namespace wh::view;
	auto model = std::make_shared<MActArray>();
	auto view = new VActCtrlPanel(m_Notebook);
	const auto& bg = whDataMgr::GetInstance()->mDbCfg->mBaseGroup->GetData();
	if ((int)bg < (int)bgTypeDesigner)
		view->DisableCreateDelete();
	model->Load();
	view->SetModel(model);
	m_Notebook->AddPage(view, "Действия", true, ResMgr::GetInstance()->m_ico_acts24);
	view->SetFocus();
	m_AuiMgr.Update();
}
//---------------------------------------------------------------------------

void MainFrame::MakePage(const wh::rec::PageObjByType& cfg)
{
	auto obj_cat = new wh::view::VObjCatalogCtrl(m_Notebook);

	bool hideSysRoot = true;
	const auto& currBaseGroup = whDataMgr::GetInstance()->mDbCfg->mBaseGroup->GetData();
	if ((int)bgUser < (int)currBaseGroup)
		hideSysRoot = false;

	auto mcat = std::make_shared<wh::object_catalog::MObjCatalog>();
	mcat->SetCfg(wh::rec::CatCfg(wh::rec::catCls, true, true, hideSysRoot));
	mcat->SetCatFilter(cfg.mParent_Cid, true);
	mcat->Load();
	obj_cat->SetModel(mcat);

	m_Notebook->AddPage(obj_cat, "Type catalog", true
		, ResMgr::GetInstance()->m_ico_folder_type24);
	obj_cat->HideCatalogSelect(true);
	m_AuiMgr.Update();

	auto onClearPath = std::bind(&MainFrame::OnSigPathClear, this, obj_cat,
		std::placeholders::_1, std::placeholders::_2);
	auto onChangePath = std::bind(&MainFrame::OnSigPathInsert, this, obj_cat,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	onClearPath(*mcat->mPath.get(), std::vector<wh::SptrIModel>());
	onChangePath(*mcat->mPath.get(), std::vector<wh::SptrIModel>(), nullptr);

	mcat->mPath->ConnAfterInsert(onChangePath);
	mcat->mPath->ConnectAfterRemove(onClearPath);
}
//---------------------------------------------------------------------------

void MainFrame::MakePage(const wh::rec::PageObjByPath& cfg)
{
	auto obj_cat = new wh::view::VObjCatalogCtrl(m_Notebook);

	bool hideSysRoot = true;
	const auto& currBaseGroup = whDataMgr::GetInstance()->mDbCfg->mBaseGroup->GetData();
	if ((int)bgUser < (int)currBaseGroup)
		hideSysRoot = false;

	auto mcat = std::make_shared<wh::object_catalog::MObjCatalog>();
	mcat->SetCfg(wh::rec::CatCfg(wh::rec::catObj, true, true, hideSysRoot));
	mcat->SetCatFilter(cfg.mParent_Oid, true);
	mcat->Load();
	obj_cat->SetModel(mcat);

	m_Notebook->AddPage(obj_cat, "Place catalog", true
		, ResMgr::GetInstance()->m_ico_folder_obj24);
	obj_cat->HideCatalogSelect(true);
	m_AuiMgr.Update();


	auto onClearPath = std::bind(&MainFrame::OnSigPathClear, this, obj_cat,
		std::placeholders::_1, std::placeholders::_2);
	auto onChangePath = std::bind(&MainFrame::OnSigPathInsert, this, obj_cat,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	onClearPath(*mcat->mPath.get(), std::vector<wh::SptrIModel>());
	onChangePath(*mcat->mPath.get(), std::vector<wh::SptrIModel>(), nullptr);

	mcat->mPath->ConnAfterInsert(onChangePath);
	mcat->mPath->ConnectAfterRemove(onClearPath);
}
//---------------------------------------------------------------------------

void MainFrame::MakePage(const wh::rec::PageObjDetail& cfg)
{
	// Setup Data
	wh::rec::ObjInfo obj_info;
	obj_info.mObj.mId = cfg.mOid;
	obj_info.mObj.mParent.mId = cfg.mParentOid;

	// Setup GUI
	wxBusyCursor busyCursor;
	wxWindowUpdateLocker	wndUpdateLocker(m_Notebook);
	wh::detail::view::CtrlPnl* pnl = new wh::detail::view::CtrlPnl(m_Notebook);
	m_Notebook->AddPage(pnl, wxEmptyString, true, wxNullIcon);
	m_AuiMgr.Update();
	pnl->SetObject(obj_info);
}
//---------------------------------------------------------------------------

void MainFrame::MakePage(const wh::rec::PageHistory& cfg)
{
	wxBusyCursor			busyCursor;
	wxWindowUpdateLocker	wndUpdateLocker(m_Notebook);
	auto model = std::make_shared<wh::MLogTable>();
	auto wnd = new wh::VTablePanel(m_Notebook);
	//wnd->mCtrl.fnOnCmdSave = nullptr; //wnd->SetEnableSave(false);
	wnd->mCtrl.fnOnCmdInsert = nullptr;
	wnd->mCtrl.fnOnCmdEdit = nullptr;
	m_Notebook->AddPage(wnd, "История", true, ResMgr::GetInstance()->m_ico_history24);
	m_AuiMgr.Update();
	wnd->SetModel(model);
	model->Load();
}
//---------------------------------------------------------------------------

void MainFrame::OnSigPathClear(wxWindow* page_wnd
	, const wh::IModel& model, const std::vector<wh::SptrIModel>&)
{
	if (0 == model.size())
	{
		wxWindowUpdateLocker	wndUpdateLocker(m_Notebook);
		auto page_idx = m_Notebook->GetPageIndex(page_wnd);
		if (wxNOT_FOUND != page_idx)
			m_Notebook->SetPageText(page_idx, "/");
	}
}
//---------------------------------------------------------------------------

void MainFrame::OnSigPathInsert(wxWindow* page_wnd
	, const wh::IModel& model, const std::vector<wh::SptrIModel>&
	, const wh::SptrIModel&)
{
	#include "MObjCatPath.h"
	auto path_array = dynamic_cast<const wh::object_catalog::model::MPath*>(&model);
	if (path_array)
	{
		wxWindowUpdateLocker	wndUpdateLocker(m_Notebook);
		auto page_idx = m_Notebook->GetPageIndex(page_wnd);
		BOOST_ASSERT_MSG(wxNOT_FOUND != page_idx, "page not found?!");
		m_Notebook->SetPageText(page_idx, path_array->GetLastItemStr());
		//m_Notebook->Update();
		//m_AuiMgr.Update();
		//this->UpdateTab(obj_cat, path_array->GetLastItemStr(), ResMgr::GetInstance()->m_ico_folder_type24);
	}
}
