#ifndef __VIEW_HISTORY_H
#define __VIEW_HISTORY_H
//-----------------------------------------------------------------------------
#include "_pch.h"
#include "IViewHistory.h"

namespace wh{
//-----------------------------------------------------------------------------
class ViewHistory : public IViewHistory
{
	wxPanel*		mPanel;
	wxAuiManager*   mAuiMgr;
	wxDataViewCtrl* mTable;
	wxAuiToolBar*	mToolbar;
public:
	ViewHistory(std::shared_ptr<IViewWindow> parent);
	virtual wxWindow* GetWnd()const override;

	virtual void SetHistoryTable(const std::shared_ptr<const ModelHistoryTableData>& rt) override;
	//virtual void SetFilterTable(const rec::ReportFilterTable& ft) override;
	//virtual void SetNote(const wxString&) override;
protected:
	void OnCmd_Update(wxCommandEvent& evt);
private:
	wxAuiToolBar*	BuildToolBar(wxWindow* parent);
	wxDataViewCtrl* BuildTable(wxWindow* parent);
};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H