#ifndef __DLG_FAVPROP_CTRLPNL_H
#define __DLG_FAVPROP_CTRLPNL_H

#include "dlg_favprop_dvtable.h"

//-----------------------------------------------------------------------------
namespace wh{
namespace dlg{
namespace favprop {
namespace view {
//-----------------------------------------------------------------------------

class CtrlPnl
	: public wxPanel
	, public ctrlWithResMgr
	
{
public:
	CtrlPnl(wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxTAB_TRAVERSAL,
		const wxString& name = wxPanelNameStr);
	~CtrlPnl();

	void SetModel(std::shared_ptr<IModel> model);

private:
	// hold parent node 
	std::shared_ptr<model::FavPropArray> mModel;
	wxAuiToolBar*		mToolBar = nullptr;
	DvTable*			mTableView = nullptr;

	void OnCmdReload(wxCommandEvent& evt);
};

//-----------------------------------------------------------------------------
} //namespace wh{
} //namespace dlg{
} //namespace favprop {
} //namespace view {
#endif // __****_H