#ifndef __VIEWMAIN_H
#define __VIEWMAIN_H

#include "IViewWindow.h"
#include "CtrlNotebook.h"
#include "ViewNotebook.h"

namespace wh{
//---------------------------------------------------------------------------
class ViewMain :public IViewWindow
{
	wxFrame*		mMainFrame;
	wxAuiToolBar*	mToolBar = nullptr;
	wxAuiManager	mAuiMgr;

	std::shared_ptr<IViewNotebook> mViewNotebook;

	void BuildMenu();
	void BuildToolbar();
	void BuildStatusbar();

public:
	ViewMain();
	~ViewMain();

	virtual wxWindow* GetWnd()const override;
	//IViewWindow virtual void SetUpdateTitle(const wxString&, const wxIcon&) {};
	virtual void SetShow()  override;

	std::shared_ptr<IViewNotebook> GetViewNotebook()const;

	virtual void UpdateConnectStatus(const wxString&);

	sig::signal<void()>		sigMkPageGroup;
	sig::signal<void()>		sigMkPageUser;
	sig::signal<void()>		sigMkPageProp;
	sig::signal<void()>		sigMkPageAct;
	sig::signal<void()>		sigMkPageObjByType;
	sig::signal<void()>		sigMkPageObjByPath;
	sig::signal<void()>		sigMkPageHistory;
	sig::signal<void()>		sigMkPageReportList;
	sig::signal<void()>		sigMkPageBrowser;

	sig::signal<void()>		sigDoConnectDB;
	sig::signal<void()>		sigDoDisconnectDB;

	sig::signal<void()>		sigShowDoc;
	sig::signal<void()>		sigShowWhatIsNew;

	//sig::signal<void()>	sigUpdateTitle;
	//sig::signal<void()>	sigClose;
	//sig::signal<void()>	sigShow;


};



//---------------------------------------------------------------------------
} //namespace wh
#endif // __***_H