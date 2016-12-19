#ifndef __INOTEBOOKVIEW_H
#define __INOTEBOOKVIEW_H

#include "IMVP.h"
namespace mvp{
//---------------------------------------------------------------------------
class INotebookView :public IView
{
public:

	virtual void AddPage(wxWindow*, const wxString& lbl, const wxIcon&) = 0;
	virtual void DelPage(wxWindow*) = 0;

	virtual void UpdatePageCaption(wxWindow* wnd, const wxString& lbl, const wxIcon& icon) = 0;

	using SigDelPage = sig::signal<void(const INotebookView* pm, wxWindow* page)>;
	
	virtual sig::connection ConnectSigDelPage(const SigDelPage::slot_type& slot) = 0;
};
} //namespace mvp{

#endif // __INOTEBOOKVIEW_H