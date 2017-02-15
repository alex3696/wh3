#ifndef __IREPORTVIEW_H
#define __IREPORTVIEW_H
//-----------------------------------------------------------------------------
#include "_pch.h"
#include "IPageView.h"
#include "ReportData.h"

namespace wh{
//-----------------------------------------------------------------------------
class IReportView : public mvp::IPageView
{
public:
	virtual void SetWnd(wxWindow* wnd)override	{ throw; }
	virtual wxWindow* GetWnd() override { throw;  return nullptr; }

	virtual void SetReportTable(const rec::ReportTable& rt) = 0;

	sig::signal<void()> sigUpdate;
	sig::signal<void()> sigExport;
	sig::signal<void()> sigSetParam;

};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H