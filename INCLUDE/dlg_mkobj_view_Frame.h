#ifndef __DLG_MKOBJ_VIEW_FRAME_H
#define __DLG_MKOBJ_VIEW_FRAME_H

//#include "dlg_mkobj_model_Obj.h"
#include "MObjItem.h"
#include "BaseOkCancelDialog.h"
#include "BtnProperty.h"

//-------------------------------------------------------------------------
namespace wh{
//namespace dlg_mkobj {
namespace object_catalog {
namespace view {
//-------------------------------------------------------------------------


//---------------------------------------------------------------------------
/// 
class Frame
	: public wxDialog
{
	wxButton*				mbtnOK;
	wxButton*				mbtnCancel;

	wxPropertyGrid*			mPropGrid;
	BtnProperty*			mPGPath;
	wxPGProperty*			mPGPid;
	wxFloatProperty*		mPGQty;

	std::shared_ptr<MObjItem> mObj;

	bool OnSelectPath(wxPGProperty* prop);

	boost::signals2::scoped_connection				mChangeConnection;
	void GetData(MObjItem::T_Data& data) const;
	void SetData(const MObjItem::T_Data& data);
	void OnChangeModel(const IModel& model);

//	void OnClose(wxCloseEvent& evt);
//	void OnCancel(wxCommandEvent& evt = wxCommandEvent());
	void OnOk(wxCommandEvent& evt = wxCommandEvent());

public:

	Frame(wxWindow* parent = nullptr,
		wxWindowID id = wxID_ANY,
		const wxString& title = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(500, 400),//wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
		const wxString& name = wxDialogNameStr);

	void SetModel(std::shared_ptr<MObjItem>& newModel);

};





//-------------------------------------------------------------------------
} // namespace view {
} // namespace dlg_move {
} // namespace wh{
#endif // __****_H