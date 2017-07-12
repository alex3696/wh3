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
#include "config.h"

#include "CtrlNotebook.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
MainFrame::MainFrame(	wxWindow* parent, wxWindowID id, const wxString& title, 
						const wxPoint& pos, const wxSize& size, long style)
	: wxFrame(parent, ID_MAINFRAME, title, wxDefaultPosition, size, style)
{
	m_AuiMgr.SetManagedWindow(this);

	auto face_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
	m_AuiMgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, face_colour);
	m_AuiMgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);

	//Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCmd_MkTabFaforite, this, CMD_MKTAB_FAVORITES);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCmd_ToogleViewMainToolbar, this, CMD_VIEW_TOOGLE_MAINTOOLBAR);
	
	Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCmd_MakePage_Group, this, CMD_PNLSHOWGROUP);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCmd_MakePage_User, this, CMD_PNLSHOWUSER);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCmd_MakePage_Prop, this, CMD_PNLSHOWPROP);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCmd_MakePage_Act, this, CMD_PNLSHOWACT);
	
	Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCmd_MakePage_ObjByType, this, CMD_MAKETYPEWND);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCmd_MakePage_ObjByPath, this, CMD_MAKEOBJWND);
	//Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCmd_MakePage_History, this, CMD_MAKEHISTORYWND);
	//Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCmd_MkPage_History, this, CMD_MKPAGE_HISTORY);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCmd_MakePage_Report, this, CMD_MKPAGE_REPORT);
	

	Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCmd_ConnectDB, this, CMD_DB_CONNECT);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCmd_DisconnectDB, this, CMD_DB_DISCONNECT);

	BuildMenu();
	BuildStatusbar();
	BuildToolbar();

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

	/*
	m_Notebook = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize
		, wxAUI_NB_DEFAULT_STYLE | wxNO_BORDER);
	m_AuiMgr.AddPane(m_Notebook, wxAuiPaneInfo().
		Name(wxT("NotebookPane")).Caption(wxT("NotebookPane")).
		CenterPane()
		//.Layer(1).Position(1).CloseButton(true).MaximizeButton(true)
		.PaneBorder(false)
		);
	*/

	m_AuiMgr.Update();



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
	item = new wxMenuItem(dir, CMD_MKPAGE_HISTORY, "Общая история");
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

	SetMenuBar(menu_bar);

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

	m_MainToolBar->AddTool(CMD_PNLSHOWGROUP, "Группы", m_ResMgr->m_ico_usergroup24);
	m_MainToolBar->AddTool(CMD_PNLSHOWUSER, "Пользователи", m_ResMgr->m_ico_user24);
	m_MainToolBar->AddTool(CMD_PNLSHOWPROP, "Свойства", m_ResMgr->m_ico_list_prop24);
	m_MainToolBar->AddTool(CMD_PNLSHOWACT, "Действия", m_ResMgr->m_ico_acts24);
	m_MainToolBar->AddTool(CMD_MKPAGE_HISTORY, "История", m_ResMgr->m_ico_history24);
	m_MainToolBar->AddTool(CMD_MKPAGE_REPORT, "Отчёты", m_ResMgr->m_ico_report_list24);


	
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
}
//---------------------------------------------------------------------------

void MainFrame::OnCmd_ToogleViewMainToolbar(wxCommandEvent& evt)
{
	if (evt.IsChecked())
	{
		wxAuiPaneInfo&  pi_main = m_AuiMgr.GetPane(m_MainToolBar);
		if (pi_main.IsOk())
			pi_main.Show();

	}
	else
	{
		wxAuiPaneInfo&  pi_main = m_AuiMgr.GetPane(m_MainToolBar);
		if (pi_main.IsOk())
			pi_main.Hide();
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
	if (m_MainToolBar)
		m_MainToolBar->Enable();


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
	if (m_MainToolBar)
		m_MainToolBar->Disable();

	// TODO закрыть все вкладки пользователя

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
	//whDataMgr::GetInstance()->mNotebookPresenter->DoAddPage(wh::rec::PageGroup());
	auto container = whDataMgr::GetInstance()->mContainer;
	auto nb2 = container->GetObject<wh::CtrlNotebook>("CtrlNotebook");
	if (nb2)
		nb2->MkWindow("CtrlPageGroupList");
}
//---------------------------------------------------------------------------

void MainFrame::OnCmd_MakePage_User(wxCommandEvent& evt)
{
	//whDataMgr::GetInstance()->mNotebookPresenter->DoAddPage(wh::rec::PageUser());
	auto container = whDataMgr::GetInstance()->mContainer;
	auto nb2 = container->GetObject<wh::CtrlNotebook>("CtrlNotebook");
	if (nb2)
		nb2->MkWindow("CtrlPageUserList");
}
//---------------------------------------------------------------------------

void MainFrame::OnCmd_MakePage_Prop(wxCommandEvent& evt)
{
	//whDataMgr::GetInstance()->mNotebookPresenter->DoAddPage(wh::rec::PageProp());
	auto container = whDataMgr::GetInstance()->mContainer;
	auto nb2 = container->GetObject<wh::CtrlNotebook>("CtrlNotebook");
	if (nb2)
		nb2->MkWindow("CtrlPagePropList");
}
//---------------------------------------------------------------------------

void MainFrame::OnCmd_MakePage_Act(wxCommandEvent& evt)
{
	//whDataMgr::GetInstance()->mNotebookPresenter->DoAddPage(wh::rec::PageAct());
	auto container = whDataMgr::GetInstance()->mContainer;
	auto nb2 = container->GetObject<wh::CtrlNotebook>("CtrlNotebook");
	if (nb2)
		nb2->MkWindow("CtrlPageActList");
}
//---------------------------------------------------------------------------

void MainFrame::OnCmd_MakePage_ObjByType(wxCommandEvent& evt)
{
	
	auto container = whDataMgr::GetInstance()->mContainer;
	auto nb2 = container->GetObject<wh::CtrlNotebook>("CtrlNotebook");
	if (nb2)
		nb2->MkWindow("CtrlPageObjByTypeList");

	//bool hideSysRoot = true;
	//const auto& currBaseGroup = whDataMgr::GetInstance()->mDbCfg->mBaseGroup->GetData();
	//if ((int)bgUser < (int)currBaseGroup)
	//	hideSysRoot = false;
	//
	//wh::rec::PageObjByType page_obj_by_type;
	//page_obj_by_type.mParent_Cid = hideSysRoot? "1" : "0";

	//whDataMgr::GetInstance()->mNotebookPresenter->DoAddPage(page_obj_by_type);
}
//---------------------------------------------------------------------------

void MainFrame::OnCmd_MakePage_ObjByPath(wxCommandEvent& evt)
{
	auto container = whDataMgr::GetInstance()->mContainer;
	auto nb2 = container->GetObject<wh::CtrlNotebook>("CtrlNotebook");
	if (nb2)
		nb2->MkWindow("CtrlPageObjByPathList");
	/*
	bool hideSysRoot = true;
	const auto& currBaseGroup = whDataMgr::GetInstance()->mDbCfg->mBaseGroup->GetData();
	if ((int)bgUser < (int)currBaseGroup)
		hideSysRoot = false;

	wh::rec::PageObjByPath page_obj_by_path;
	page_obj_by_path.mParent_Oid = hideSysRoot ? "1" : "0";

	whDataMgr::GetInstance()->mNotebookPresenter->DoAddPage(page_obj_by_path);
	*/
}
//---------------------------------------------------------------------------

//void MainFrame::OnCmd_MakePage_History(wxCommandEvent& evt)
//{
//	auto container = whDataMgr::GetInstance()->mContainer;
//	auto nb2 = container->GetObject<wh::CtrlNotebook>("CtrlNotebook");
//	if (nb2)
//		nb2->MkWindow("CtrlPageLogList");
//}
////---------------------------------------------------------------------------
//void MainFrame::OnCmd_MkPage_History(wxCommandEvent& evt)
//{
//	auto container = whDataMgr::GetInstance()->mContainer;
//	auto nb2 = container->GetObject<wh::CtrlNotebook>("CtrlNotebook");
//	if (nb2)
//		nb2->MkWindow("CtrlPageLogList");
//}

//---------------------------------------------------------------------------

void MainFrame::OnCmd_MakePage_Report(wxCommandEvent& evt)
{
	auto container = whDataMgr::GetInstance()->mContainer;
	auto nb2 = container->GetObject<wh::CtrlNotebook>("CtrlNotebook");
	if (nb2)
		nb2->MkWindow("CtrlPageReportList");
}