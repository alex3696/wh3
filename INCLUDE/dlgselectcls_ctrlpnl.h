#ifndef __DLGSELECTCLS_CTRLPNL_H
#define __DLGSELECTCLS_CTRLPNL_H

#include "VObjCatalogCtrl.h"

namespace wh{
namespace select{

class ClsDlg:
	public wxDialog
	,public ctrlWithResMgr
{
protected:
	wxButton*				m_btnOK;
	wxButton*				m_btnCancel;
	wxStdDialogButtonSizer*	m_sdbSizer;
	view::VObjCatalogCtrl*	mMainPanel;

public:	


	ClsDlg(wxWindow* parent,
				wxWindowID id = wxID_ANY,
				const wxString& title=wxEmptyString,
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxSize( 400,300 ),//wxDefaultSize,
				long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER,
				const wxString& name = wxDialogNameStr);


	void SetModel(std::shared_ptr<IModel> model);


	bool GetSelectedCls(wh::rec::Cls& cls);
	bool GetSelectedObj(wh::rec::ObjTitle& obj);
};


}//namespace dlgselectcls
}//namespace wh
#endif //__*_H