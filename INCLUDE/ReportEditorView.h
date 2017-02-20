#ifndef __REPORTEDITORVIEW_H
#define __REPORTEDITORVIEW_H
//-----------------------------------------------------------------------------
#include "IReportEditorView.h"
#include "IReportEditorView.h"
#include "IViewWindow.h"

namespace wh{
//-----------------------------------------------------------------------------
class ReportEditorView : public IReportEditorView
{
	wxDialog* mFrame;
	wxTextCtrl* mTitleText;
	wxTextCtrl* mNoteText;
	wxTextCtrl* mScriptText;
	wxStdDialogButtonSizer* mBtnSizer;
	wxButton* mBtnSizerOK;
	wxButton* mBtnSizerCancel;

	void OnCancel(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
	wxString mId;
public:
	ReportEditorView(std::shared_ptr<IViewWindow> wnd);

	virtual void SetReportItem(const rec::ReportItem&) override;

	virtual void Show()override;
	virtual void Close()override;
};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H