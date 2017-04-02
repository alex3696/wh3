#ifndef __VIEWMAIN_H
#define __VIEWMAIN_H

#include "IViewWindow.h"
#include "MainFrame.h"
#include "CtrlNotebook.h"
#include "ViewNotebook.h"

namespace wh{
//---------------------------------------------------------------------------
class ViewMain :public IViewWindow
{
	MainFrame* mMainFrame;

	std::shared_ptr<IViewNotebook> mViewNotebook;

	void OnCmd_MkPage_History(wxCommandEvent& evt);
public:
	ViewMain();

	virtual wxWindow* GetWnd()const override;
	//IViewWindow virtual void OnUpdateTitle(const wxString&, const wxIcon&) {};
	virtual void OnShow()  override;

	std::shared_ptr<IViewNotebook> GetViewNotebook()const;


	//sig::signal<void()>	sigUpdateTitle;
	//sig::signal<void()>	sigClose;
	//sig::signal<void()>	sigShow;


};



//---------------------------------------------------------------------------
} //namespace wh
#endif // __***_H