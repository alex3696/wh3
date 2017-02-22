#ifndef __IREPORTEDITORVIEW_H
#define __IREPORTEDITORVIEW_H
//-----------------------------------------------------------------------------
#include "_pch.h"
#include "ReportData.h"

namespace wh{
//-----------------------------------------------------------------------------
class IReportEditorView
{
public:
	virtual void SetReportItem(const rec::ReportItem&) = 0;

	sig::signal<void(const rec::ReportItem&)> sigChItem;

	
	virtual void Show()=0;
	virtual void Close() = 0;

};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H