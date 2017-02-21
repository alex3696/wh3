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
	
	//std::shared_ptr<IReportListView> mView;
	//std::shared_ptr<ReportListModel> mModel;

	void OnListUpdated(const rec::ReportList&);

	virtual void ConnectView()override;
	virtual void DisconnectView()override;
public:
	ReportListPresenter(std::shared_ptr<IReportListView> view, std::shared_ptr<ReportListModel> model);

	virtual std::shared_ptr<IViewWindow> GetView()const override;

	void UpdateList();
	void ExecReport(size_t idx);
	size_t MkReport();
	void RmReport(size_t idx);
	void ChReport(size_t idx);
		
	virtual void MkView()override;

};
//-----------------------------------------------------------------------------

} // namespace wh{
#endif // __INOTEBOOKVIEW_H