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
	sig::scoped_connection connViewExecute;

	sig::scoped_connection connModelExecuted;
	sig::scoped_connection connModelShowFilterTable;
public:
	ReportPresenter(std::shared_ptr<IReportView> view, std::shared_ptr<ReportModel> model);


	void Update();
	void Execute();
	void Export();

};
//-----------------------------------------------------------------------------

} // namespace wh{
#endif // __INOTEBOOKVIEW_H