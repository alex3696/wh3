#ifndef __DLG_ACT_VIEW_FRAME_H
#define __DLG_ACT_VIEW_FRAME_H

#include "dlg_act_view_PnlActArray.h"
#include "dlg_act_view_PnlPropGrid.h"
#include "BaseOkCancelDialog.h"

//-------------------------------------------------------------------------
namespace wh{
namespace dlg_act {
namespace view {
//-------------------------------------------------------------------------


//---------------------------------------------------------------------------
/// 
class Frame
	: public wxDialog
{
	wxStaticText*			mCaptionTxt;
	
	wxButton*				mbtnNextOK;
	wxButton*				mbtnPrevios;

	PnlActArray*			mPnlActArray;
	PnlPropGrid*			mPnlPropGrid;

	wxAuiToolBar*			mToolBar = nullptr;

	std::shared_ptr<model::Obj > mActObj;
	std::shared_ptr<model::Act > mSelectedAct;

	void OnCancel(wxCommandEvent& evt = wxCommandEvent());
	void OnClose(wxCloseEvent& evt);
	void OnBackward(wxCommandEvent& evt = wxCommandEvent());
	void OnForward(wxCommandEvent& evt = wxCommandEvent());
	void OnActivated(wxDataViewEvent &evt);

public:

	Frame(wxWindow* parent = nullptr,
		wxWindowID id = wxID_ANY,
		const wxString& title = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(500, 400),//wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
		const wxString& name = wxDialogNameStr);

	
	void SetModel(std::shared_ptr<model::Obj>& model);

};





//-------------------------------------------------------------------------
} // namespace view {
} // namespace dlg_move {
} // namespace wh{
#endif // __****_H