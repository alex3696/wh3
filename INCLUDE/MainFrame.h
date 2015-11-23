#ifndef __MAINFRAME_H
#define __MAINFRAME_H

#include "_pch.h"
#include "whDB.h"


#include "globaldata.h"

//---------------------------------------------------------------------------
enum GUIID
{
	// GUI controls IDs
	ID_MAINFRAME=1100,

	// CMD
	CMD_DB_CONNECT,
	CMD_DB_DISCONNECT,
	CMD_DB_TEST,
	CMD_DB_TEST2,

	CMD_MAKETYPEWND,
	CMD_MAKEOBJWND,
	CMD_MAKEHISTORYWND,

	CMD_SHOWFAVORITES,
	CMD_SHOWEDITOROPTIONS,

	CMD_PNLSHOWGROUP,
	CMD_PNLSHOWUSER,
	CMD_PNLSHOWPROP,
	CMD_PNLSHOWACT

};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// MainFrame 
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class MainFrame :  public wxFrame, public ctrlWithResMgr
{
// ��������� ������� ������������: ������� ���� WX ������
private:

	wxAuiToolBar*		m_MainToolBar = nullptr;
	wxAuiToolBar*		m_DevToolBar = nullptr;

	wxAuiToolBarItem*	m_btnFavorites = nullptr;;

	wxWindow*			m_wndFavorites = nullptr;;

	void BuildMenu();
	void BuildToolbar();
	void BuildStatusbar();

	void ShowDevToolBar(bool show = true);
	void PnlShowGroup(wxCommandEvent& WXUNUSED(evt));
	void PnlShowUser(wxCommandEvent& WXUNUSED(evt));
	void PnlShowProp(wxCommandEvent& WXUNUSED(evt));
	void PnlShowAct(wxCommandEvent& WXUNUSED(evt));
public:
	wxAuiManager	m_AuiMgr;
	wxAuiNotebook*	m_Notebook;

	MainFrame( wxWindow* parent = NULL, wxWindowID id = wxID_ANY, 
		const wxString& title = wxT("wh3"), 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1200,600 ), 
		long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL);
	~MainFrame();


	void DoMenuPreferences( wxCommandEvent& evt=wxCommandEvent() );
	void OnMakeTypeWnd(wxCommandEvent& evt=wxCommandEvent());
	void OnMakeObjWnd(wxCommandEvent& evt=wxCommandEvent());
	void OnMakeHistoryWnd(wxCommandEvent& evt=wxCommandEvent());
	void OnShowFavorites(wxCommandEvent& evt=wxCommandEvent());

	void OnShowLoginWnd(wxCommandEvent& evt=wxCommandEvent());
	void OnDisconnectDB(wxCommandEvent& evt=wxCommandEvent());

	
	void CreateTypeCatalog(const wxString& _type);	
	void CreateObjCatalog(const wxString& _objclass,const wxString& _objname,
		const wxString& _classfilter=wxEmptyString);	


	void AddTab(wxWindow* pnl, const wxString& lbl=wxEmptyString, const wxIcon& icon=wxNullIcon);
	void UpdateTab(wxWindow* pnl, const wxString& lbl, const wxIcon& icon);



	DECLARE_EVENT_TABLE();

};

#endif // __MAINFRAME_H


