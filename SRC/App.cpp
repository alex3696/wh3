#include "_pch.h"
#include "App.h"
#include "MainFrame.h"
#include "ResManager.h" 

IMPLEMENT_APP( App );

ResMgr*		mgr=NULL;
whDataMgr*	gdm=NULL;

class wxLogGuiError : public wxLogGui
{
public: 
	wxLogGuiError()
		:wxLogGui()
	{}



	void SetThisLogLevel(wxLogLevelValues lvl)
	{
		mShowLevel = lvl;
	}
	wxLogLevelValues  GetThisLogLevel()const
	{
		return mShowLevel;
	}
protected:
	wxLogLevelValues mShowLevel = wxLOG_Warning;

	virtual void DoLogRecord(wxLogLevel level,
		const wxString& msg,
		const wxLogRecordInfo& info)override
	{
		if ((unsigned long)mShowLevel >= level)
			wxLogGui::DoLogRecord(level, msg, info);
	}

};

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
		auto guiErrorLogger = new wxLogGuiError();
		wxLog::SetActiveTarget(mLogger);
		
		wxLogChain *logChain = new wxLogChain(guiErrorLogger);
		wxLog::SetActiveTarget(logChain);
		
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
	delete wxLog::SetActiveTarget(nullptr);
	fclose(mLogFile);
	mLogFile = nullptr;

	//delete mLogger;
	mLogger = nullptr;

	return 0;
}
