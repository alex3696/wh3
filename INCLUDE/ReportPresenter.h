#ifndef __REPORTPRESENTER_H
#define __REPORTPRESENTER_H

#include "IReportView.h"
#include "ReportModel.h"

namespace wh{
//-----------------------------------------------------------------------------
class ReportPresenter 
{
	sig::scoped_connection connViewUpdate;
	sig::scoped_connection connViewExport;
	sig::scoped_connection connViewSetParam;

	std::shared_ptr<IReportView> mView;
	std::shared_ptr<ReportModel> mModel;

public:
	ReportPresenter(std::shared_ptr<IReportView> view, std::shared_ptr<ReportModel> model);

	virtual std::shared_ptr<IReportView> GetView();

	void Update();
	void Export();
	void SetParam();

};
//-----------------------------------------------------------------------------

} // namespace wh{
#endif // __INOTEBOOKVIEW_H