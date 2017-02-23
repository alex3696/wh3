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
public:
	ViewMain()
		:mMainFrame(new MainFrame(NULL))
	{
		mViewNotebook = std::make_shared<ViewNotebook>(mMainFrame);
		mMainFrame->m_AuiMgr.AddPane(mViewNotebook->GetWnd(), wxAuiPaneInfo().
			Name(wxT("NotebookPane")).Caption(wxT("NotebookPane")).
			CenterPane()
			.PaneBorder(false)
			);
		mMainFrame->m_AuiMgr.Update();

		whDataMgr::GetInstance()->mContainer->RegInstance("ViewNotebook", mViewNotebook);

		mMainFrame->Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent&)
		{ 
			sigClose(); 
			mMainFrame->Destroy();
			mMainFrame = nullptr;
		});

	}

	virtual wxWindow* GetWnd()const override { return mMainFrame; }
	//IViewWindow virtual void OnUpdateTitle(const wxString&, const wxIcon&) {};
	virtual void OnShow()  override {	mMainFrame->Show();	}

	std::shared_ptr<IViewNotebook> GetViewNotebook()const
	{
		return mViewNotebook;
	}


	//sig::signal<void()>	sigUpdateTitle;
	//sig::signal<void()>	sigClose;
	//sig::signal<void()>	sigShow;


};



//---------------------------------------------------------------------------
} //namespace wh
#endif // __***_H