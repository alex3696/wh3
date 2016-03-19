#ifndef __DLG_MOVE_VIEW_FRAME_H
#define __DLG_MOVE_VIEW_FRAME_H

#include "dlg_move_view_CtrlPanel.h"
#include "BaseOkCancelDialog.h"

//-------------------------------------------------------------------------
namespace wh{
namespace dlg_move {
namespace view {
//-------------------------------------------------------------------------


//---------------------------------------------------------------------------
/// 
class Frame
	: public wxDialog
{
	wxButton*				mbtnOK;
	wxButton*				mbtnCancel;
	wxStdDialogButtonSizer*	msdbSizer;
	wxStaticText*			mLblMovableObj;
	wxStaticText*			mLblDstObj;

	wxSpinCtrl* mqtySpin;
	wxTextCtrl* mqtyCtrl;
	CtrlPanel* mCtrlPanel;
	std::shared_ptr<model::MovableObj > mMovable;

	void OnClose(wxCloseEvent& evt);
	void OnCancel(wxCommandEvent& evt = wxCommandEvent());
	void OnOk(wxCommandEvent& evt = wxCommandEvent());
	void OnActivated(wxDataViewEvent &evt = wxDataViewEvent());
public:

	Frame(wxWindow* parent = nullptr,
		wxWindowID id = wxID_ANY,
		const wxString& title = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(500, 400),//wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
		const wxString& name = wxDialogNameStr);

	
	void SetModel(std::shared_ptr<IModel>& model);
	void SetModel(std::shared_ptr<model::MovableObj>& model);

	std::shared_ptr<model::MovableObj> GetModel(){ return mMovable; }

};





//-------------------------------------------------------------------------
} // namespace view {
} // namespace dlg_move {
} // namespace wh{
#endif // __****_H