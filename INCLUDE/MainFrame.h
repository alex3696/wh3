#ifndef __MAINFRAME_H
#define __MAINFRAME_H

#include "globaldata.h"
#include "TModel.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// MainFrame 
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class MainFrame :  public wxFrame, public ctrlWithResMgr
{
// соблюдать порядок наследования: первыми идут WX классы
private:

	wxAuiToolBar*		m_MainToolBar = nullptr;
	wxAuiToolBar*		m_DevToolBar = nullptr;

	wxAuiToolBarItem*	m_btnFavorites = nullptr;;

	wxWindow*			m_wndFavorites = nullptr;;

	void BuildMenu();
	void BuildToolbar();
	void BuildDevToolBar();
	void BuildStatusbar();
	
	void OnCmd_MakePage_Group(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_MakePage_User(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_MakePage_Prop(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_MakePage_Act(wxCommandEvent& evt = wxCommandEvent());

	void OnCmd_MakePage_ObjByType(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_MakePage_ObjByPath(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_MakePage_History(wxCommandEvent& evt = wxCommandEvent());

	void OnCmd_MkTabFaforite(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_ToogleViewMainToolbar(wxCommandEvent& evt = wxCommandEvent());

	sig::scoped_connection mSSC_AfterDbConnected;
	sig::scoped_connection mSSC_BeforeDbDisconnect;
	void OnSig_AfterDbConnected(const whDB&);
	void OnSig_BeforeDbDisconnect(const whDB&);


	sig::scoped_connection mSSC_AfterChange_BaseGroup;
	void OnSigAfterChange_BaseGroup(const wh::IModel*, const BaseGroup* const);

	wxAuiManager	m_AuiMgr;
	wxAuiNotebook*	m_Notebook;
public:
	void AddNotebook(wxAuiNotebook*);

	MainFrame( wxWindow* parent = NULL, wxWindowID id = wxID_ANY, 
		const wxString& title = wxT("wh3"), 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,600 ), 
		long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL);
	~MainFrame();

	void DoShowConnDlg(wxCommandEvent& evt = wxCommandEvent());
	void DoMenuPreferences( wxCommandEvent& evt=wxCommandEvent() );
	void OnCmd_ConnectDB(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_DisconnectDB(wxCommandEvent& evt=wxCommandEvent());

	
	
	

};

#endif // __MAINFRAME_H


