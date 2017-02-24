#ifndef __IREPORTLISTVIEW_H
#define __IREPORTLISTVIEW_H
//-----------------------------------------------------------------------------
#include "_pch.h"
#include "IViewWindow.h"
#include "ReportData.h"

namespace wh{
//-----------------------------------------------------------------------------
class IReportListView : public IViewWindow
{
public:
	virtual void SetReportList(const rec::ReportList& rl) = 0;
	virtual void OnMkReport(const std::shared_ptr<const rec::ReportItem>&) = 0;
	virtual void OnRmReport(const std::shared_ptr<const rec::ReportItem>&) = 0;
	virtual void OnChReport(const std::shared_ptr<const rec::ReportItem>&, const wxString& old_rep_id) = 0;
	
	sig::signal<void()>					sigUpdateList;
	sig::signal<void(const wxString&)>	sigExecReport;
	sig::signal<void()>					sigMkReport;
	sig::signal<void(const wxString&)>	sigRmReport;
	sig::signal<void(const wxString&)>	sigChReport;

};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H