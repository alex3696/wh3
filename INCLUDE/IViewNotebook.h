#ifndef __IVIEWNOTEBOOK_H
#define __IVIEWNOTEBOOK_H

#include "IViewWindow.h"

namespace wh{
//---------------------------------------------------------------------------
class IViewNotebook :public IViewWindow
{
public:
	//IViewWindow virtual wxWindow* GetWnd() = 0;
	//IViewWindow virtual void OnUpdateTitle(const wxString&, const wxIcon&) {};
	//IViewWindow virtual void OnShow()  {};
	//IViewWindow virtual void OnClose() {};

	virtual void MkPage(wxWindow* wnd) = 0 ;
	virtual void ChPage(wxWindow* wnd, const wxString& lbl, const wxIcon& icon) = 0;
	virtual void RmPage(wxWindow* wnd) = 0;
	virtual void ShowPage(wxWindow* wnd) = 0;

	sig::signal<void(const wxString&)>	sigMkWindow;
	sig::signal<void(wxWindow* wnd)>	sigRmWindow;
	sig::signal<void(wxWindow* wnd)>	sigShowWindow;

};



//---------------------------------------------------------------------------
} //namespace mvp{
#endif // __INOTEBOOKVIEW_H