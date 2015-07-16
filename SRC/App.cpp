#include "_pch.h"
#include "App.h"
#include "MainFrame.h"
#include "ResManager.h" 

IMPLEMENT_APP( App );

ResMgr*		mgr=NULL;
whDataMgr*	gdm=NULL;



//---------------------------------------------------------------------------
bool App::OnInit()
{
#if wxUSE_IMAGE
	wxInitAllImageHandlers();
#endif	
	mgr = ResMgr::GetInstance();
	gdm = whDataMgr::GetInstance();


	// создаём вид
	MainFrame* mainframe = new MainFrame(NULL);
	mainframe->Show();


	gdm->m_MainFrame=mainframe;
	
	SetTopWindow(mainframe);
	
	return true;
}
//---------------------------------------------------------------------------
App::~App()
{
//	delete login;
//	delete mainframe;
}
//---------------------------------------------------------------------------
int App::OnExit()
{
	return 0;
}
