#include "_pch.h"
#include "App.h"
#include "config.h" 

#include "CtrlMain.h"

#include "whLogin.h"
#include "ConnectionCfgDlg.h"


IMPLEMENT_APP( App );
//---------------------------------------------------------------------------
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

	// inint global data
	auto gmgr = whDataMgr::GetInstance();
	// Load local config
	gmgr->mConnectCfg->Load();
	// init factories
	gmgr->InitContainer();

	// init main controller
	using namespace wh;
	auto model_main = std::make_shared<ModelMain>();
	auto view_main = std::make_shared<ViewMain>();
	auto ctrl_main = std::make_shared<CtrlMain>(view_main, model_main);
	gmgr->mContainer->RegInstance("CtrlMain", ctrl_main);
	gmgr->mContainer->RegInstance("ViewMain", view_main);
	
	wxToolTip::Enable(true);
	wxToolTip::SetAutoPop(15000);
	SetTopWindow(view_main->GetWnd());
	ctrl_main->Show();

	whLogin dlg(ctrl_main->GetView()->GetWnd());
	dlg.ShowModal();
	//auto ctrl_login = data_mgr->mContainer->GetObject<ConnectionCfgDlg>("CtrlLogin");
	//ctrl_login.ShowModal();

	return true;
}
//---------------------------------------------------------------------------
App::~App()
{
}
//---------------------------------------------------------------------------
int App::OnExit()
{
	// сохраняем конфиг
	auto data_mgr = whDataMgr::GetInstance();
	data_mgr->mContainer->Clear();

	// отключаем логер
	mLogger->Flush();
	wxLog::EnableLogging(false);
	delete wxLog::SetActiveTarget(nullptr);
	fclose(mLogFile);
	mLogFile = nullptr;

	//delete mLogger;
	mLogger = nullptr;

	return 0;
}
