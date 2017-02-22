#ifndef __REPORTVIEW_H
#define __REPORTVIEW_H
//-----------------------------------------------------------------------------
#include "_pch.h"
#include "IReportView.h"

namespace wh{
//-----------------------------------------------------------------------------
class ReportView : public IReportView
{
	wxPanel* mPanel;
	wxDataViewCtrl* mTable;
public:
	ReportView(std::shared_ptr<IViewWindow> parent);
	virtual wxWindow* GetWnd()const override;

	virtual void SetReportTable(const rec::ReportTable& rt) override;
protected:
	void OnCmd_Update(wxCommandEvent& evt);
	void OnCmd_Export(wxCommandEvent& evt);
private:
	wxAuiToolBar*	BuildToolBar(wxWindow* parent);
	wxDataViewCtrl* BuildReportTable(wxWindow* parent);
};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H