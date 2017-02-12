#ifndef __IREPORTLISTVIEW_H
#define __IREPORTLISTVIEW_H
//-----------------------------------------------------------------------------
#include "_pch.h"
#include "IPageView.h"
#include "ReportData.h"

namespace wh{
//-----------------------------------------------------------------------------
class IReportListView : public mvp::IPageView
{
public:
	virtual void SetWnd(wxWindow* wnd)override	{ throw; }
	virtual wxWindow* GetWnd() override { throw;  return nullptr; }

	virtual void SetReportList(const rec::ReportList& rl) = 0;


	using SigUpdateList = sig::signal<void()>;
	using SigExecReport = sig::signal<void(size_t)>;
	using SigMakeReport = sig::signal<size_t()>;
	using SigRemoveReport = sig::signal<void(size_t idx)>;
	using SigChangeReport = sig::signal<void(size_t idx)>;
	
	SigUpdateList	sigUpdateList;
	SigExecReport	sigExecReport;
	SigMakeReport	sigMkReport;
	SigRemoveReport sigRmReport;
	SigChangeReport sigChReport;

};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H