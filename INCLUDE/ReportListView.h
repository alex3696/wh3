#ifndef __REPORTVIEW_H
#define __REPORTVIEW_H
//-----------------------------------------------------------------------------
#include "_pch.h"
#include "IReportListView.h"

namespace wh{
//-----------------------------------------------------------------------------
class ReportListView : public IReportListView
{
	wxPanel* mPanel;
	wxDataViewCtrl* mTable;
public:
	ReportListView(std::shared_ptr<wxWindow*> wnd);
	virtual void SetWnd(wxWindow* wnd)override;
	virtual wxWindow* GetWnd() override;

	virtual void SetReportList(const rec::ReportList& rl) override;
protected:
	void OnCmd_UpdateList(wxCommandEvent& evt);
	void OnCmd_ExecReport(wxCommandEvent& evt);
	void OnCmd_MkReport(wxCommandEvent& evt);
	void OnCmd_RmReport(wxCommandEvent& evt);
	void OnCmd_ChReport(wxCommandEvent& evt);
private:
	wxAuiToolBar*	BuildToolBar(wxWindow* parent);
	wxDataViewCtrl* BuildReportList(wxWindow* parent);
};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H