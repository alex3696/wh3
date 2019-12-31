#ifndef __MOVEOBJVIEW_H
#define __MOVEOBJVIEW_H

#include "IMoveObjView.h"

namespace wh{

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class XMoveObjView : public IMoveObjView
{
	wxDialog*				mFrame;
	wxButton*				mbtnOK;
	wxButton*				mbtnCancel;
	wxStdDialogButtonSizer*	msdbSizer;
	wxStaticText*			mLblMovableObj;
	wxStaticText*			mLblDstObj;

	wxSpinCtrl* mqtySpin;
	wxTextCtrl* mqtyCtrl;

	wxTextCtrl*		mFindCtrl;
	wxAuiToolBar*	mToolBar;
	wxDataViewCtrl* mTree;

	//void OnTbRefresh(wxCommandEvent& evt = wxCommandEvent());
	//void OnTbRecent(wxCommandEvent& evt = wxCommandEvent());
	//void OnTbFindFilter(wxCommandEvent& evt = wxCommandEvent());
	//void OnTbFindFilterPopup(wxCommandEvent& evt = wxCommandEvent());
	
	void OnClose(wxCloseEvent& evt);
	void OnCancel(wxCommandEvent& evt = wxCommandEvent());
	void OnOk(wxCommandEvent& evt = wxCommandEvent());
	void OnActivated(wxDataViewEvent &evt = wxDataViewEvent());

	void BuildTree();
	void BuildToolBar();
	void AutosizeColumns();
	void ExpandTree(const wxDataViewItem& dvitem,bool recursive=true);
	void ExpandAll();
	void OnClickSearchBtn(wxCommandEvent& event);
	
public:
	XMoveObjView(wxWindow* parent);
	XMoveObjView(const std::shared_ptr<IViewWindow>& parent);
	~XMoveObjView();

	virtual void ShowDialog()override;
	virtual void UpdateRecent(const ObjTree& tree)override;
	virtual void UpdateDst(const ObjTree& tree)override;
	virtual void UpdateMoveable(const rec::PathItem& moveable)override;
	virtual void EnableRecent(bool enable)override;
};
//---------------------------------------------------------------------------




}//namespace wh{
#endif // __****_H