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
	wxFileName		local_cfg_file = wxFileConfig::GetLocalFile("wh3.ini");;

	
	wxStandardPathsBase& stdp = wxStandardPaths::Get();
	wxString path = wxString::Format("%s\\wh3log.txt", stdp.GetUserConfigDir());
	
	

	mLogFile = fopen(path.c_str(), "w");
	if (mLogFile)
	{
		mLogger = new wxLogStderr(mLogFile);
		wxLog::SetActiveTarget(mLogger);
	}
	else
	{
		mLogFile = nullptr;
		mLogger = nullptr;
	}

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
	mLogger->Flush();
	wxLog::EnableLogging(false);
	wxLog::SetActiveTarget(nullptr);
	fclose(mLogFile);
	mLogFile = nullptr;

	delete mLogger;
	mLogger = nullptr;

	return 0;
}
