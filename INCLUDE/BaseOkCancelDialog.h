#ifndef __BASEOKCANCELDIALOG_H
#define __BASEOKCANCELDIALOG_H

#include "globaldata.h"

namespace wh{
namespace view{



class DlgBaseOkCancel:
	public wxDialog
	,public ctrlWithResMgr
{
protected:
	wxButton*				m_btnOK;
	wxButton*				m_btnCancel;
	wxStdDialogButtonSizer*	m_sdbSizer;
	wxWindow*			mMainPanel;


	void Create();
public:	
	DlgBaseOkCancel(
				std::function<wxWindow*(wxWindow*)>	mainPanelCreate, 
				wxWindow* parent = nullptr, 
				wxWindowID id = wxID_ANY,
				const wxString& title=wxEmptyString,
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxSize( 400,300 ),//wxDefaultSize,
				long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER,
				const wxString& name = wxDialogNameStr);

	DlgBaseOkCancel(	wxWindow* parent, 
				wxWindowID id = wxID_ANY,
				const wxString& title=wxEmptyString,
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxSize( 400,300 ),//wxDefaultSize,
				long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER,
				const wxString& name = wxDialogNameStr);

	void InsertMainWindow( wxWindow* panel );

	wxWindow* GetMainPanel()const;
};



}//namespace wh{
}//namespace view{
#endif // __BASEOKCANCELDIALOG_H
