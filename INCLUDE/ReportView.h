#ifndef __REPORTVIEW_H
#define __REPORTVIEW_H
//-----------------------------------------------------------------------------
#include "_pch.h"
#include "IReportView.h"

namespace wh{
//-----------------------------------------------------------------------------
class ReportView : public IReportView
{
	class wxAuiPanel : public wxPanel
	{
	public:
		wxAuiPanel(wxWindow* wnd)
			:wxPanel(wnd)
		{
			mAuiMgr.SetManagedWindow(this);
		}
		~wxAuiPanel()
		{
			mAuiMgr.UnInit();
		}
		wxAuiManager	mAuiMgr;
	};

	wxAuiPanel*		mPanel;
	wxDataViewCtrl* mTable;
	wxStaticText*	mNote;
	wxPropertyGrid* mPG;
	wxAuiToolBar*	mToolbar;
public:
	ReportView(std::shared_ptr<IViewWindow> parent);
	~ReportView();
	virtual wxWindow* GetWnd()const override;

	virtual void SetReportTable(const rec::ReportTable& rt) override;
	virtual void SetFilterTable(const rec::ReportFilterTable& ft) override;
	virtual void SetNote(const wxString&) override;
protected:
	void OnCmd_Update(wxCommandEvent& evt);
	void OnCmd_Execute(wxCommandEvent& evt);
	void OnCmd_Export(wxCommandEvent& evt);
private:
	wxAuiToolBar*	BuildToolBar(wxWindow* parent);
	wxDataViewCtrl* BuildReportTable(wxWindow* parent);
};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H