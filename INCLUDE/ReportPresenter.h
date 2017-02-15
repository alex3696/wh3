#ifndef __REPORTPRESENTER_H
#define __REPORTPRESENTER_H

#include "PagePresenter.h"
#include "IReportView.h"
#include "ReportModel.h"

namespace wh{
//-----------------------------------------------------------------------------
class ReportPresenter : public mvp::PagePresenter
{
	sig::scoped_connection connViewUpdate;
	sig::scoped_connection connViewExport;
	sig::scoped_connection connViewSetParam;

	std::shared_ptr<IReportView> mView;
	std::shared_ptr<ReportModel> mModel;

public:
	ReportPresenter(std::shared_ptr<IReportView> view, std::shared_ptr<ReportModel> model);

	virtual void SetView(mvp::IView* view) override;
	virtual void SetModel(const std::shared_ptr<mvp::IModel>& model)override;
	virtual std::shared_ptr<mvp::IModel> GetModel() override;
	virtual mvp::IView* GetView() override;

	void Update();
	void Export();
	void SetParam();

};
//-----------------------------------------------------------------------------

} // namespace wh{
#endif // __INOTEBOOKVIEW_H