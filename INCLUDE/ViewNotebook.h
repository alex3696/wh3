#ifndef __VIEWNOTEBOOK_H
#define __VIEWNOTEBOOK_H
//-----------------------------------------------------------------------------
#include "_pch.h"
#include "IViewNotebook.h"

namespace wh{
//-----------------------------------------------------------------------------
class ViewNotebook : public IViewNotebook
{
	wxAuiNotebook* mNotebook;
public:
	ViewNotebook(std::shared_ptr<IViewWindow> parent);
	ViewNotebook(wxWindow* parent);

	virtual wxWindow* GetWnd()const override;
	virtual void MkPage(wxWindow* wnd) override;
	virtual void ChPage(wxWindow* wnd, const wxString& lbl, const wxIcon& icon) override;
	virtual void RmPage(wxWindow* wnd) override;
	virtual void ShowPage(wxWindow* wnd) override;
protected:
	void OnCmd_MkWindow(wxCommandEvent& evt);

	void OnEvt_ClosePage(wxAuiNotebookEvent& evt);

};

//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H