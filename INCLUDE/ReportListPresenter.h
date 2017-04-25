#ifndef __REPORTLISTPRESENTER_H
#define __REPORTLISTPRESENTER_H

#include "CtrlWindowBase.h"
#include "IReportListView.h"
#include "ReportListModel.h"

namespace wh{
//-----------------------------------------------------------------------------
class ReportListPresenter : public CtrlWindowBase<IReportListView, ReportListModel>
{
	sig::scoped_connection connViewUpdateList;
	sig::scoped_connection connViewExecReport;
	sig::scoped_connection connViewMkReport;
	sig::scoped_connection connViewRmReport;
	sig::scoped_connection connViewChReport;

	sig::scoped_connection connModelUpdate;
	sig::scoped_connection connModelMk;
	sig::scoped_connection connModelRm;
	sig::scoped_connection connModelCh;
	
	void OnListUpdated(const rec::ReportList&);
	void OnMkReport(const std::shared_ptr<const rec::ReportItem>&);
	void OnRmReport(const std::shared_ptr<const rec::ReportItem>&);
	void OnChReport(const std::shared_ptr<const rec::ReportItem>&, const wxString& old_rep_id);

	virtual void ConnectView()override;
	virtual void DisconnectView()override;
public:
	ReportListPresenter(const std::shared_ptr<IReportListView>& view,const std::shared_ptr<ReportListModel>& model);

	virtual std::shared_ptr<IViewWindow> GetView()const override;

	void UpdateList();
	void ExecReport(const wxString& rep_id);
	void MkReport();
	void RmReport(const wxString& rep_id);
	void ChReport(const wxString& rep_id);
		
	virtual void MkView()override;

};
//-----------------------------------------------------------------------------

} // namespace wh{
#endif // __INOTEBOOKVIEW_H