#include "_pch.h"
#include "ViewMain.h"

using namespace wh;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ViewMain::ViewMain()
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

	mMainFrame->m_MainToolBar->AddSeparator();
	mMainFrame->m_MainToolBar->AddSpacer(30);
	const wxIcon& ico = ResMgr::GetInstance()->m_ico_history24;
	mMainFrame->m_MainToolBar->AddTool(wxID_INFO, "История 3", ico);
	mMainFrame->m_MainToolBar->Realize();

	wxAuiPaneInfo&  pi = mMainFrame->m_AuiMgr.GetPane("mMainToolBar");
	if (pi.IsOk())
		pi.BestSize(mMainFrame->m_MainToolBar->GetClientSize());

	mMainFrame->m_AuiMgr.Update();

	mMainFrame->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewMain::OnCmd_MkPage_History, this, wxID_INFO);
}
//---------------------------------------------------------------------------

//virtual 
wxWindow* ViewMain::GetWnd()const //override 
{ 
	return mMainFrame; 
}
//---------------------------------------------------------------------------

//virtual 
void ViewMain::OnShow()  //override 
{ 
	mMainFrame->Show(); 
}
//---------------------------------------------------------------------------

std::shared_ptr<IViewNotebook> ViewMain::GetViewNotebook()const
{
	return mViewNotebook;
}
//---------------------------------------------------------------------------

void ViewMain::OnCmd_MkPage_History(wxCommandEvent& evt)
{
	auto container = whDataMgr::GetInstance()->mContainer;
	auto nb2 = container->GetObject<wh::CtrlNotebook>("CtrlNotebook");
	if (nb2)
		nb2->MkWindow("CtrlPageHistory");
}

