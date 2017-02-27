#ifndef __IREPORTVIEW_H
#define __IREPORTVIEW_H
//-----------------------------------------------------------------------------

#include "IViewWindow.h"
#include "ReportData.h"

namespace wh{
//-----------------------------------------------------------------------------
class IReportView : public IViewWindow
{
public:
	virtual void SetReportTable(const rec::ReportTable& rt) = 0;
	virtual void SetFilterTable(const rec::ReportFilterTable& ft) = 0;

	sig::signal<void()> sigUpdate;
	sig::signal<void()> sigExecute;
	sig::signal<void()> sigExport;

};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H