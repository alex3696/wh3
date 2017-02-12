#ifndef __REPORTPRESENTER_H
#define __REPORTPRESENTER_H

#include "PagePresenter.h"
#include "IReportListView.h"
#include "ReportListModel.h"

namespace wh{
//-----------------------------------------------------------------------------
class ReportListPresenter : public mvp::PagePresenter
{
	sig::scoped_connection connViewUpdateList;
	sig::scoped_connection connViewExecReport;
	sig::scoped_connection connViewMkReport;
	sig::scoped_connection connViewRmReport;
	sig::scoped_connection connViewChReport;

	sig::scoped_connection connModelUpdate;
	
	std::shared_ptr<IReportListView> mView;
	std::shared_ptr<ReportListModel> mModel;

	void OnListUpdated(const rec::ReportList&);
public:
	ReportListPresenter(std::shared_ptr<IReportListView> view, std::shared_ptr<ReportListModel> model);

	virtual void SetView(mvp::IView* view) override;
	virtual void SetModel(const std::shared_ptr<mvp::IModel>& model)override;
	virtual std::shared_ptr<mvp::IModel> GetModel() override;
	virtual mvp::IView* GetView() override;

	void UpdateList();
	void ExecReport(size_t idx);
	size_t MkReport();
	void RmReport(size_t idx);
	void ChReport(size_t idx);

};
//-----------------------------------------------------------------------------

} // namespace wh{
#endif // __INOTEBOOKVIEW_H