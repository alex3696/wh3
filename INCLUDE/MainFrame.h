#ifndef __MAINFRAME_H
#define __MAINFRAME_H

#include "_pch.h"
#include "whDB.h"


#include "globaldata.h"

//---------------------------------------------------------------------------
enum GUIID
{
	// GUI controls IDs
	ID_MAINFRAME,

	// CMD
	CMD_DB_CONNECT,
	CMD_DB_DISCONNECT,
	CMD_DB_TEST,
	CMD_DB_TEST2,

	CMD_MAKETYPEWND,
	CMD_MAKEOBJWND,
	CMD_MAKEHISTORYWND,

	CMD_SHOWFAVORITES


};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// MainFrame 
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class MainFrame :  public wxFrame, public ctrlWithResMgr
{
// соблюдать порядок наследования: первыми идут WX классы
private:

	wxAuiToolBar*		m_MainToolBar;
	wxAuiToolBarItem*	m_btnFavorites;

	wxWindow*			m_wndFavorites;
public:
	wxAuiManager	m_AuiMgr;
	wxAuiNotebook*	m_Notebook;

	MainFrame( wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxString& title = wxT("Hub GUI"), 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 653,523 ), 
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
	void CreateObjCatalog(const wxString& _objclass,const wxString& _objname,const wxString& _classfilter=wxEmptyString);	



	DECLARE_EVENT_TABLE();

};

#endif // __MAINFRAME_H


