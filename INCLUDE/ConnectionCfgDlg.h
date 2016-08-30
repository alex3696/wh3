#ifndef __CONNECTIONCFGDLG_H
#define __CONNECTIONCFGDLG_H

#include "globaldata.h"
//---------------------------------------------------------------------------

class ConnectionCfgDlg
	: public wxDialog
{
public:
	ConnectionCfgDlg(wxWindow* parent, wxWindowID id = wxID_ANY
		, const wxString& title = wxEmptyString
		, const wxPoint& pos = wxDefaultPosition
		, const wxSize& size = wxDefaultSize
		, long style = wxDEFAULT_DIALOG_STYLE);
	~ConnectionCfgDlg();

protected:
	void OnOk(wxCommandEvent& evt);

	wxPropertyGrid* mPG;

	wxPGProperty*	mPGPServer;
	wxPGProperty*	mPGPPort;
	wxPGProperty*	mPGPBd;
	wxPGProperty*	mPGPRole;

	wxButton* m_btnOK;
	wxButton* m_btnCancel;

};

#endif // __CONNECTIONCFGDLG_H
