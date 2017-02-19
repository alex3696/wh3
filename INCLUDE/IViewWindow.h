#ifndef __IVIEWWINDOW_H
#define __IVIEWWINDOW_H

#include "_pch.h"

namespace wh{
//---------------------------------------------------------------------------
class IViewWindow
{
public:
	virtual ~IViewWindow(){}
	virtual wxWindow* GetWnd()const = 0;
	virtual void OnUpdateTitle(const wxString&, const wxIcon&) {};
	virtual void OnShow()  {};
	virtual void OnClose() {};

	sig::signal<void()>	sigUpdateTitle;
	sig::signal<void()>	sigClose;
	sig::signal<void()>	sigShow;


};
//---------------------------------------------------------------------------
} //namespace mvp{
#endif // __IMVP_H