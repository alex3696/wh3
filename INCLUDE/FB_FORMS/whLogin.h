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

	void SetAuthInfo(const wxString& name, const wxString& pass, bool store = true);
	wxString GetUserName()const;
	wxString GetUserPass()const;
	bool GetStorePass()const;

protected:
	void OnParam( wxCommandEvent& event );
	
	wxComboBox* m_cbxUserName;
	wxTextCtrl* m_txtPass;
	wxCheckBox* m_chkStorePass;

	wxButton* m_btnParam;
	wxButton* m_btnOK;
	wxButton* m_btnCancel;


};

#endif // __whLogin__
