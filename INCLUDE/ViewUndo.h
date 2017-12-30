#ifndef __VIEW_DETAIL_H
#define __VIEW_DETAIL_H
//-----------------------------------------------------------------------------
#include "_pch.h"
#include "IViewUndo.h"

namespace wh{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ViewUndoWindow final : public IViewUndoWindow
{
	wxDialog*		mPanel;
	wxPropertyGrid*	mPG;

public:
	ViewUndoWindow(wxWindow* parent);
	ViewUndoWindow(const std::shared_ptr<IViewWindow>& parent);
	virtual wxWindow* GetWnd()const override {	return mPanel;	}
	virtual void OnShow()override;
	virtual void OnUpdateTitle(const wxString&, const wxIcon&)override;

	virtual void SetHistoryRecord(const ModelHistoryRecord&) override;

	void OnCmd_Load(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_ExecuteUndo(wxCommandEvent& evt = wxCommandEvent());
protected:

private:

	
};//class ViewUndoWindow





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H