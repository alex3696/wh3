#ifndef __INOTEBOOKVIEW_H
#define __INOTEBOOKVIEW_H

#include "IMVP.h"
namespace mvp{
//---------------------------------------------------------------------------
class INotebookView :public IView
{
public:
	INotebookView(IPresenter* parent)
		:IView(parent)
	{}

	virtual void AddPage(wxWindow*, const wxString& lbl, const wxIcon&) = 0;
	virtual void DelPage(wxWindow*) = 0;

	virtual void UpdatePageCaption(wxWindow* wnd, const wxString& lbl, const wxIcon& icon) = 0;
};
} //namespace mvp{

#endif // __INOTEBOOKVIEW_H