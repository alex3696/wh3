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
		, const wxSize& size = wxDefaultSize
		, long style = wxDEFAULT_DIALOG_STYLE);
	~whLogin();

protected:
	void OnParam( wxCommandEvent& evt );
	void OnShow(wxShowEvent& evt);
	void OnOk(wxCommandEvent& evt);
	
	wxComboBox* m_cbxUserName;
	wxTextCtrl* m_txtPass;
	wxCheckBox* m_chkStorePass;

	wxButton* m_btnParam;
	wxButton* m_btnOK;
	wxButton* m_btnCancel;


};

#endif // __whLogin__
