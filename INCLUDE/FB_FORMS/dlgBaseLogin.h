///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __dlgBaseLogin__
#define __dlgBaseLogin__

#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/combobox.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

#include "_pch.h"

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class dlgBaseLogin
///////////////////////////////////////////////////////////////////////////////
class dlgBaseLogin : public wxDialog 
{
	private:
	
	protected:
		wxComboBox* m_cbxUserName;
		wxTextCtrl* m_txtPass;
		
		wxCheckBox* m_chkStorePass;
		
		wxButton* m_btnParam;
		
		wxButton* m_btnOK;
		wxButton* m_btnCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnParam( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		dlgBaseLogin( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Авторизация"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 400,270 ), long style = wxDEFAULT_DIALOG_STYLE );
		~dlgBaseLogin();
	
};

#endif //__dlgBaseLogin__
