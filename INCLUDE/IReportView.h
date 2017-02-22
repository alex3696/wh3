#ifndef __IREPORTVIEW_H
#define __IREPORTVIEW_H
//-----------------------------------------------------------------------------
#include "_pch.h"
#include "ReportData.h"

namespace wh{
//-----------------------------------------------------------------------------
class IReportView 
{
public:
	virtual wxWindow* GetWnd() { throw;  return nullptr; }

	virtual void SetReportTable(const rec::ReportTable& rt) = 0;

	sig::signal<void()> sigUpdate;
	sig::signal<void()> sigExport;
	sig::signal<void()> sigSetParam;

};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H