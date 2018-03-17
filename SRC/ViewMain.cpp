#include "_pch.h"
#include "ViewMain.h"
#include "config.h" 

using namespace wh;

enum GUIID
{
	// GUI controls IDs
	ID_MAINFRAME = 1100,

	// CMD
	CMD_DB_CONNECT,
	CMD_DB_DISCONNECT,

	CMD_MKPAGE_GROUP,
	CMD_MKPAGE_USER,
	CMD_MKPAGE_PROP,
	CMD_MKPAGE_ACT,
	CMD_MKPAGE_OBJBYTYPE,
	CMD_MKPAGE_OBJBYPATH,
	CMD_MKPAGE_HISTORY,
	CMD_MKPAGE_REPORT,
	CMD_MKPAGE_BROWSERCLS,
	CMD_MKPAGE_BROWSEROBJ

};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ViewMain::ViewMain()
	:mMainFrame(new wxFrame(NULL, ID_MAINFRAME, "wh3"
	                       , wxDefaultPosition, wxSize(800, 600)
						   , wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL
	                       )
	           )
{
	mAuiMgr.SetManagedWindow(mMainFrame);

	auto face_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
	mAuiMgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, face_colour);
	mAuiMgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);


	BuildMenu();
	BuildToolbar();
	BuildStatusbar();
		
	mViewNotebook = std::make_shared<ViewNotebook>(mMainFrame);
	mAuiMgr.AddPane(mViewNotebook->GetWnd(), wxAuiPaneInfo().
		Name(wxT("NotebookPane")).Caption(wxT("NotebookPane")).
		CenterPane()
		.PaneBorder(false)
		);
	whDataMgr::GetInstance()->mContainer->RegInstance("ViewNotebook", mViewNotebook);

	mMainFrame->Bind(wxEVT_COMMAND_MENU_SELECTED,
		[this](wxCommandEvent){ sigDoConnectDB(); }, CMD_DB_CONNECT);
	mMainFrame->Bind(wxEVT_COMMAND_MENU_SELECTED,
		[this](wxCommandEvent){ sigDoDisconnectDB(); }, CMD_DB_DISCONNECT);

	mMainFrame->Bind(wxEVT_COMMAND_MENU_SELECTED
		, [this](wxCommandEvent){ sigMkPageGroup(); }, CMD_MKPAGE_GROUP);
	mMainFrame->Bind(wxEVT_COMMAND_MENU_SELECTED
		, [this](wxCommandEvent){ sigMkPageUser(); }, CMD_MKPAGE_USER);
	mMainFrame->Bind(wxEVT_COMMAND_MENU_SELECTED
		, [this](wxCommandEvent){ sigMkPageProp(); }, CMD_MKPAGE_PROP);
	mMainFrame->Bind(wxEVT_COMMAND_MENU_SELECTED
		, [this](wxCommandEvent){ sigMkPageAct(); }, CMD_MKPAGE_ACT);
	mMainFrame->Bind(wxEVT_COMMAND_MENU_SELECTED
		, [this](wxCommandEvent){ sigMkPageObjByType(); }, CMD_MKPAGE_OBJBYTYPE);
	mMainFrame->Bind(wxEVT_COMMAND_MENU_SELECTED
		, [this](wxCommandEvent){ sigMkPageObjByPath(); }, CMD_MKPAGE_OBJBYPATH);
	mMainFrame->Bind(wxEVT_COMMAND_MENU_SELECTED
		, [this](wxCommandEvent){ sigMkPageHistory(); }, CMD_MKPAGE_HISTORY);
	mMainFrame->Bind(wxEVT_COMMAND_MENU_SELECTED
		, [this](wxCommandEvent){ sigMkPageReportList(); }, CMD_MKPAGE_REPORT);
	mMainFrame->Bind(wxEVT_COMMAND_MENU_SELECTED
		, [this](wxCommandEvent){ sigMkPageBrowserCls(); }, CMD_MKPAGE_BROWSERCLS);
	mMainFrame->Bind(wxEVT_COMMAND_MENU_SELECTED
		, [this](wxCommandEvent) { sigMkPageBrowserObj(); }, CMD_MKPAGE_BROWSEROBJ);
	mMainFrame->Bind(wxEVT_COMMAND_MENU_SELECTED
		, [this](wxCommandEvent) { sigShowDoc(); }, wxID_HELP_INDEX);
	mMainFrame->Bind(wxEVT_COMMAND_MENU_SELECTED
		, [this](wxCommandEvent) { sigShowWhatIsNew(); }, wxID_HELP_PROCEDURES);

	mMainFrame->Bind(wxEVT_CLOSE_WINDOW
		, [this](wxCloseEvent& evt)
		{
			sigClose();
			mAuiMgr.UnInit();
			mMainFrame->Destroy();
			mMainFrame=nullptr;
		});

	mAuiMgr.Update();
}
//---------------------------------------------------------------------------
ViewMain::~ViewMain()
{

}
//---------------------------------------------------------------------------
//virtual 
wxWindow* ViewMain::GetWnd()const //override 
{ 
	return mMainFrame; 
}
//---------------------------------------------------------------------------

//virtual 
void ViewMain::SetShow()  //override 
{ 
	mMainFrame->Show(); 
}
//---------------------------------------------------------------------------

std::shared_ptr<IViewNotebook> ViewMain::GetViewNotebook()const
{
	return mViewNotebook;
}
//---------------------------------------------------------------------------
void ViewMain::UpdateConnectStatus(const wxString& conn_str)
{
	mMainFrame->SetStatusText(conn_str, 0);
}

//---------------------------------------------------------------------------
void ViewMain::BuildMenu()
{
	auto resmgr = ResMgr::GetInstance();

	wxMenuBar*	menu_bar = new wxMenuBar(0);
	wxMenuItem* item = NULL;

	wxMenu* dbmenu = new wxMenu();
	item = new wxMenuItem(dbmenu, wxID_SETUP, "��������� ����������");
	dbmenu->Append(item);
	item = new wxMenuItem(dbmenu, CMD_DB_CONNECT, "������������ � ��");
	item->SetBitmap(resmgr->m_ico_connect16);
	dbmenu->Append(item);
	item = new wxMenuItem(dbmenu, CMD_DB_DISCONNECT, "����������� �� ��");
	item->SetBitmap(resmgr->m_ico_disconnect16);
	dbmenu->Append(item);
	dbmenu->AppendSeparator();
	item = new wxMenuItem(dbmenu, wxID_EXIT, "�����");
	dbmenu->Append(item);
	menu_bar->Append(dbmenu, "�����������");

	wxMenu* dir = new wxMenu();
	item = new wxMenuItem(dir, CMD_MKPAGE_GROUP, "������ �������������");
	item->SetBitmap(resmgr->m_ico_usergroup16);
	dir->Append(item);
	item = new wxMenuItem(dir, CMD_MKPAGE_USER, "������������");
	item->SetBitmap(resmgr->m_ico_user16);
	dir->Append(item);
	dir->AppendSeparator();
	item = new wxMenuItem(dir, CMD_MKPAGE_PROP, "��������");
	item->SetBitmap(resmgr->m_ico_list_prop16);
	dir->Append(item);
	item = new wxMenuItem(dir, CMD_MKPAGE_ACT, "��������");
	item->SetBitmap(resmgr->m_ico_acts16);
	dir->Append(item);
	dir->AppendSeparator();
	item = new wxMenuItem(dir, CMD_MKPAGE_OBJBYPATH, "������� �������� �� ��������������(����������)");
	item->SetBitmap(resmgr->m_ico_folder_obj24);
	dir->Append(item);
	item = new wxMenuItem(dir, CMD_MKPAGE_OBJBYTYPE, "������� �������� �� ����(����������)");
	item->SetBitmap(resmgr->m_ico_folder_type24);
	dir->Append(item);
	item = new wxMenuItem(dir, CMD_MKPAGE_BROWSEROBJ, "������� �������� �� ��������������");
	item->SetBitmap(resmgr->m_ico_folder_obj24);
	dir->Append(item);
	item = new wxMenuItem(dir, CMD_MKPAGE_BROWSERCLS, "������� �������� �� ����");
	item->SetBitmap(resmgr->m_ico_folder_type24);
	dir->Append(item);
	dir->AppendSeparator();
	item = new wxMenuItem(dir, CMD_MKPAGE_HISTORY, "����� �������");
	item->SetBitmap(resmgr->m_ico_history24);
	dir->Append(item);
	item = new wxMenuItem(dir, CMD_MKPAGE_REPORT, "������ �������");
	item->SetBitmap(resmgr->m_ico_report_list24);
	dir->Append(item);
	dir->AppendSeparator();
	menu_bar->Append(dir, "��������");

	wxMenu* hlp = new wxMenu();
	item = new wxMenuItem(dbmenu, wxID_HELP_INDEX, "����������");
	hlp->Append(item);
	item = new wxMenuItem(dbmenu, wxID_HELP_PROCEDURES, "��� ������?");
	hlp->Append(item);
	menu_bar->Append(hlp, "�������");
	
	mMainFrame->SetMenuBar(menu_bar);

}

//---------------------------------------------------------------------------
void ViewMain::BuildToolbar()
{
	auto resmgr = ResMgr::GetInstance();

	long main_toolbar_style = wxAUI_TB_DEFAULT_STYLE
		| wxAUI_TB_OVERFLOW
		//| wxAUI_TB_TEXT
		| wxAUI_TB_HORZ_TEXT
		| wxAUI_TB_PLAIN_BACKGROUND
		| wxAUI_TB_GRIPPER;

	mToolBar = new wxAuiToolBar(mMainFrame, wxID_ANY, wxDefaultPosition, wxDefaultSize, main_toolbar_style);

	//m_MainToolBar->AddTool(CMD_DB_CONNECT, "������������ � ��", m_ResMgr->m_ico_connect24, "������������ � ��", wxITEM_RADIO);
	//m_MainToolBar->AddTool(CMD_DB_DISCONNECT, "����������� �� ��", m_ResMgr->m_ico_disconnect24, "����������� �� ��", wxITEM_RADIO);
	//m_MainToolBar->AddSeparator();

	const wxString str_mktab_objdir = "������� �������� �� ��������������";
	mToolBar->AddTool(CMD_MKPAGE_BROWSEROBJ, str_mktab_objdir
		, resmgr->m_ico_folder_obj24, str_mktab_objdir);

	const wxString cat_obj_by_type = "������� �������� �� ����";
	mToolBar->AddTool(CMD_MKPAGE_BROWSERCLS, cat_obj_by_type
		, resmgr->m_ico_folder_type24, cat_obj_by_type);

	//mToolBar->AddTool(CMD_MKPAGE_GROUP, "������", m_ResMgr->m_ico_usergroup24);
	//mToolBar->AddTool(CMD_MKPAGE_USER, "������������", m_ResMgr->m_ico_user24);
	//mToolBar->AddTool(CMD_MKPAGE_PROP, "��������", m_ResMgr->m_ico_list_prop24);
	//mToolBar->AddTool(CMD_MKPAGE_ACT, "��������", m_ResMgr->m_ico_acts24);
	mToolBar->AddTool(CMD_MKPAGE_HISTORY, "�������", resmgr->m_ico_history24);
	mToolBar->AddTool(CMD_MKPAGE_REPORT, "������", resmgr->m_ico_report_list24);



	mToolBar->Realize();

	mAuiMgr.AddPane(mToolBar, wxAuiPaneInfo().
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

void ViewMain::BuildStatusbar()
{
	/*
	wxStatusBar *statbarNew = NULL;
	statbarNew = new wxStatusBar(this, wxID_ANY, style, "wxStatusBar");
	statbarNew->SetFieldsCount(2);
	*/

	mMainFrame->CreateStatusBar(3);
}