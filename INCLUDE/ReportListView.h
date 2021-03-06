#ifndef __REPORTLISTVIEW_H
#define __REPORTLISTVIEW_H
//-----------------------------------------------------------------------------
#include "_pch.h"
#include "IReportListView.h"
#include "IViewNotebook.h"

namespace wh{
//-----------------------------------------------------------------------------
class ReportListView : public IReportListView
{
	wxPanel* mPanel;
	wxDataViewCtrl* mTable;
public:
	ReportListView(std::shared_ptr<IViewNotebook> wnd);
	virtual wxWindow* GetWnd()const override;

	virtual void SetReportList(const rec::ReportList& rl) override;
	virtual void OnMkReport(const std::shared_ptr<const rec::ReportItem>&) override;
	virtual void OnRmReport(const std::shared_ptr<const rec::ReportItem>&) override;
	virtual void OnChReport(const std::shared_ptr<const rec::ReportItem>&, const wxString& old_rep_id) override;

	void OnActivated(wxDataViewEvent &evt = wxDataViewEvent());
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