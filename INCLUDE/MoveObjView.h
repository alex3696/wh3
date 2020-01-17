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
	wxButton*				mBtnBack;
	wxButton*				mBtnForward;
	wxStdDialogButtonSizer*	msdbSizer;
	wxStaticText*			mInfo;
	wxPanel*				mObjListPanel;
	wxPanel*				mDstPanel;
	std::shared_ptr<ViewTableBrowser>	mObjBrowser;

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
	void OnBack(wxCommandEvent& evt);
	void OnActivated(wxDataViewEvent &evt = wxDataViewEvent());

	void BuildTree(wxWindow* parent);
	void BuildToolBar(wxWindow* parent);
	void AutosizeColumns();
	void ExpandTree(const wxDataViewItem& dvitem,bool recursive=true);
	void ExpandAll();
	void OnClickSearchBtn(wxCommandEvent& event);

	
	
	wxTimer		mTimer;
	int			mMillSecLeft;
	void StartCountdown();
	void StepCountdown();
	void StopCountdown();
	void OnTimer(wxTimerEvent &evt = wxTimerEvent());
public:
	XMoveObjView(wxWindow* parent);
	XMoveObjView(const std::shared_ptr<IViewWindow>& parent);
	~XMoveObjView();

	virtual void SetSelectPage(int);
	virtual std::shared_ptr<ViewTableBrowser>	GetViewObjBrowser()const;
	virtual void UpdateRecent(const ObjTree& tree)override;
	virtual void UpdateDst(const ObjTree& tree)override;
	virtual void EnableRecent(bool enable)override;
	virtual void GetSelection(std::set<int64_t>& sel)const override;

	virtual wxWindow* GetWnd()const override { return mFrame; }
	virtual void SetShow()override;
	virtual void SetClose()override;

};
//---------------------------------------------------------------------------




}//namespace wh{
#endif // __****_H