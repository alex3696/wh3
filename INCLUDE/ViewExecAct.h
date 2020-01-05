#ifndef __VIEWEXECACT_H
#define __VIEWEXECACT_H

#include "_pch.h"
#include "CtrlWindowBase.h"
#include "ViewBrowser.h"
#include "ViewActBrowser.h"

namespace wh {
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ViewExecActWindow final : public IViewWindow
{
	wxDialog*			mPanel;

	wxStaticText* mInfo;
	wxPanel* mObjListPanel;
	wxPanel* mActListPanel;
	wxPanel* mPropListPanel;

	wxButton*	mBtnBack;
	wxButton*	mBtnForward;


	std::shared_ptr<ViewTableBrowser>	mObjBrowser;
	std::shared_ptr<ViewActBrowser>		mActBrowser;

public:
	ViewExecActWindow(wxWindow* parent);
	ViewExecActWindow(const std::shared_ptr<IViewWindow>& parent);
	~ViewExecActWindow();
	virtual wxWindow* GetWnd()const override {	return mPanel;	}
	virtual void SetShow()override;
	virtual void SetUpdateTitle(const wxString&, const wxIcon&)override;

	virtual std::shared_ptr<ViewTableBrowser>	GetViewObjBrowser()const;
	virtual std::shared_ptr<ViewActBrowser>		GetViewActBrowser()const;

	void SetSelectPage(int);

	sig::signal<void()> sigUnlock;
	sig::signal<void()> sigSelectAct;
	sig::signal<void()> sigExecute;
protected:
	void OnClose(wxCloseEvent& evt);
	void OnCancel(wxCommandEvent& evt = wxCommandEvent());
	void OnOk(wxCommandEvent& evt = wxCommandEvent());
	void OnBack(wxCommandEvent& evt = wxCommandEvent());
	void OnActivated(wxDataViewEvent &evt = wxDataViewEvent());
private:
	


	
};//class ViewUndoWindow




}//namespace wh{
#endif // __****_H