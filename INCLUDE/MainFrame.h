#ifndef __MAINFRAME_H
#define __MAINFRAME_H

#include "globaldata.h"
#include "TModel.h"
#include "AppViewConfig.h"

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

	template <class MODEL>
	bool MakePage(const wh::SptrIModel& curr)
	{
		auto model = std::dynamic_pointer_cast<MODEL>(curr);
		if (model)
			MakePage(model->GetData());
		return model ? true : false;
	}

	void MakePage(const wh::rec::PageUser& cfg);
	void MakePage(const wh::rec::PageGroup& cfg);
	void MakePage(const wh::rec::PageProp& cfg);
	void MakePage(const wh::rec::PageAct& cfg);
	void MakePage(const wh::rec::PageObjByType& cfg);
	void MakePage(const wh::rec::PageObjByPath& cfg);
	void MakePage(const wh::rec::PageObjDetail& cfg);
	void MakePage(const wh::rec::PageHistory& cfg);
	
	void OnCmd_MakePage_Group(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_MakePage_User(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_MakePage_Prop(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_MakePage_Act(wxCommandEvent& evt = wxCommandEvent());

	void OnCmd_MakePage_ObjByType(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_MakePage_ObjByPath(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_MakePage_History(wxCommandEvent& evt = wxCommandEvent());

	sig::scoped_connection mSSC_AfterDbConnected;
	sig::scoped_connection mSSC_BeforeDbDisconnect;
	void OnSig_AfterDbConnected(const whDB* const);
	void OnSig_BeforeDbDisconnect(const whDB* const);


	sig::scoped_connection mSSC_AfterChange_BaseGroup;
	void OnSigAfterChange_BaseGroup(const wh::IModel*, const BaseGroup* const);

	sig::scoped_connection mSSC_AfterInsert_Page;
	void OnSigAfterInsertPage(const wh::IModel&
		, const std::vector<wh::SptrIModel>& newItems, const wh::SptrIModel& itemBefore);

	wxAuiManager	m_AuiMgr;
	wxAuiNotebook*	m_Notebook;

public:

	MainFrame( wxWindow* parent = NULL, wxWindowID id = wxID_ANY, 
		const wxString& title = wxT("wh3"), 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,600 ), 
		long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL);
	~MainFrame();

	void DoShowConnDlg(wxCommandEvent& evt = wxCommandEvent());
	void DoMenuPreferences( wxCommandEvent& evt=wxCommandEvent() );
	void OnCmd_ConnectDB(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_DisconnectDB(wxCommandEvent& evt=wxCommandEvent());

	void OnCmd_MkTabFaforite(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_ToogleViewMainToolbar(wxCommandEvent& evt = wxCommandEvent());
	
	void AddTab(wxWindow* pnl, const wxString& lbl=wxEmptyString, const wxIcon& icon=wxNullIcon);
	void UpdateTab(wxWindow* pnl, const wxString& lbl, const wxIcon& icon);


	void OnSigPathClear(const int page_idx
		, const wh::IModel& model, const std::vector<wh::SptrIModel>&);

	void OnSigPathInsert(const int page_idx
		, const wh::IModel& model, const std::vector<wh::SptrIModel>&
		, const wh::SptrIModel&);

};

#endif // __MAINFRAME_H


