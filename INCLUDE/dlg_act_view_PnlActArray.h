#ifndef __DLG_ACT_VIEW_PNLACTARRAY_H
#define __DLG_ACT_VIEW_PNLACTARRAY_H

#include "BaseOkCancelDialog.h"
#include "dlg_act_model_Obj.h"
#include "dlg_act_view_ActList.h"

//-------------------------------------------------------------------------
namespace wh{
namespace dlg_act {
namespace view {
//-------------------------------------------------------------------------


//---------------------------------------------------------------------------
/// 
class PnlActArray
	: public wxPanel
	, public ctrlWithResMgr
	
{
	// hold parent node 
	std::shared_ptr<model::ActArray> mActArray;

	ActList*			mActList = nullptr;

	void OnActivated(wxDataViewEvent &evt);
public:

	PnlActArray(wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxTAB_TRAVERSAL,
		const wxString& name = wxPanelNameStr);
	~PnlActArray();

	void SetModel(std::shared_ptr<model::ActArray>& model);

	void GetSelected(std::vector<unsigned int>& vec);

};






//-------------------------------------------------------------------------
} // namespace view {
} // namespace dlg_move {
} // namespace wh{
#endif // __****_H