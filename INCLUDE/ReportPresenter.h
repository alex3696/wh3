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
	sig::scoped_connection connModelShowNote;
public:
	ReportPresenter(std::shared_ptr<IReportView> view, std::shared_ptr<ReportModel> model);


	void Update();
	void Execute(const std::vector<wxString>& filter_vec);
	void Export();

};
//-----------------------------------------------------------------------------

} // namespace wh{
#endif // __INOTEBOOKVIEW_H