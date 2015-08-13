#ifndef __DLG_FAVPROP_SELECTFRAME_H
#define __DLG_FAVPROP_SELECTFRAME_H

#include "globaldata.h"
#include "dlg_favprop_ctrlpnl.h"

//-----------------------------------------------------------------------------
namespace wh{
namespace dlg{
namespace favprop{
namespace view{
//-----------------------------------------------------------------------------

class SelectFrame
	:public wxDialog
{
public:

	SelectFrame(wxWindow* parent = nullptr,
		wxWindowID id = wxID_ANY,
		const wxString& title = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(500, 400),//wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
		const wxString& name = wxDialogNameStr);

	void SetModel(std::shared_ptr<IModel>& model);
private:
	std::shared_ptr<model::FavPropArray>	mModel;

	CtrlPnl*	mCtrlPnl;

	wxButton*	mbtnOK;
	wxButton*	mbtnCancel;

	void OnOk(wxCommandEvent& evt = wxCommandEvent());
};

//-----------------------------------------------------------------------------
} //namespace wh{
} //namespace dlg{
} //namespace favprop{
} //namespace view{
#endif // __****_H