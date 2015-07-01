#ifndef __DLG_ACT_VIEW_PNLPROPGRID_H
#define __DLG_ACT_VIEW_PNLPROPGRID_H

#include "BaseOkCancelDialog.h"
#include "dlg_act_model_Obj.h"

//-------------------------------------------------------------------------
namespace wh{
namespace dlg_act {
namespace view {
//-------------------------------------------------------------------------


//---------------------------------------------------------------------------
/// 
class PnlPropGrid
	: public wxPanel
	, public ctrlWithResMgr
	
{
	// hold parent node 
	std::shared_ptr<model::PropArray> mPropArray;

	wxAuiToolBar*		mToolBar = nullptr;
	wxMenu				mContextMenu;
	wxPropertyGrid*		mPropGrid;
	
public:

	PnlPropGrid(wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxTAB_TRAVERSAL,
		const wxString& name = wxPanelNameStr);
	~PnlPropGrid();

	void SetModel(std::shared_ptr<model::PropArray>& model);
	std::shared_ptr<model::PropArray> GetModel();

	bool UpdateModel();


};






//-------------------------------------------------------------------------
} // namespace view {
} // namespace dlg_move {
} // namespace wh{
#endif // __****_H