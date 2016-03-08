#ifndef __whLogin_h
#define __whLogin__

#include "globaldata.h"
//---------------------------------------------------------------------------

class whLogin 
	: public wxDialog
{
public:
	whLogin(wxWindow* parent, wxWindowID id = wxID_ANY
		, const wxString& title = wxT("Авторизация")
		, const wxPoint& pos = wxDefaultPosition
		, const wxSize& size = wxSize(400, 270)
		, long style = wxDEFAULT_DIALOG_STYLE);
	~whLogin();

	wh::Cfg::DbConnect GetCfg()const;
	void SetCfg(const wh::Cfg::DbConnect& cfg );
	
	wxString	GetUserName()const	{ return m_cbxUserName->GetValue(); }
	wxString	GetPass()const		{ return m_txtPass->GetValue(); }
	bool		GetStorePass()const	{ return m_chkStorePass->GetValue(); }

	void SetUserName(const wxString& username)	{ m_cbxUserName->SetValue(username); }
	void SetPass(const wxString& pass)			{ m_txtPass->SetValue(pass); }
	void SetStorePass(bool storepass)			{ m_chkStorePass->SetValue(storepass); }

protected:
	// Handlers for dlgBaseLogin events.
	void OnParam( wxCommandEvent& event );
	void OnOk( wxCommandEvent& event );
	void OnCancel( wxCommandEvent& event );
	
	wxComboBox* m_cbxUserName;
	wxTextCtrl* m_txtPass;
	wxCheckBox* m_chkStorePass;

	wxPanel*		mPnlExt;
	wxTextCtrl*		mTxtRole;
	wxTextCtrl*		mTxtServer;
	wxTextCtrl*		mTxtPort;
	wxTextCtrl*		mTxtDB;

	wxButton* m_btnParam;
	wxButton* m_btnOK;
	wxButton* m_btnCancel;


};

#endif // __whLogin__
