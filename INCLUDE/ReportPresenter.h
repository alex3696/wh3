#ifndef __REPORTPRESENTER_H
#define __REPORTPRESENTER_H

#include "CtrlWindowBase.h"
#include "IReportView.h"
#include "ReportModel.h"

namespace wh{
//-----------------------------------------------------------------------------
class ReportPresenter : public CtrlWindowBase<IReportView, ReportModel>
{
	sig::scoped_connection connViewUpdate;
	sig::scoped_connection connViewExport;
	sig::scoped_connection connViewSetParam;

public:
	ReportPresenter(std::shared_ptr<IReportView> view, std::shared_ptr<ReportModel> model);


	void Update();
	void Export();
	void SetParam();

};
//-----------------------------------------------------------------------------

} // namespace wh{
#endif // __INOTEBOOKVIEW_H