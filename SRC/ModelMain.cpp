#include "_pch.h"
#include "CtrlMain.h"
#include "config.h"
#include "globaldata.h"
#include "RecentDstOidPresenter.h"
#include "whLogin.h"


using namespace wh;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModelMain::ModelMain()
{

}
//---------------------------------------------------------------------------
//virtual 
void ModelMain::Load(const boost::property_tree::wptree& app_cfg)//override
{
	whDataMgr::GetInstance()->mDbCfg->mGuiCfg->LoadDefaults(app_cfg);
	whDataMgr::GetInstance()->mDbCfg->mGuiCfg->LoadClientInfo(app_cfg);
	whDataMgr::GetInstance()->mRecentDstOidPresenter->Load(app_cfg);

	auto cnt = whDataMgr::GetInstance()->mContainer;
	auto last_login = cnt->GetObject<wxDateTime>(L"ClientInfoLastLogin");
	if (!last_login->IsValid() || wxDateTime::Now() > (*last_login + wxTimeSpan(720, 0, 0, 0)))
		ShowDoc();
}
//---------------------------------------------------------------------------
//virtual 
void ModelMain::Save(boost::property_tree::wptree& app_cfg)//override
{
	whDataMgr::GetInstance()->mDbCfg->mGuiCfg->SaveDefaults(app_cfg);
	whDataMgr::GetInstance()->mDbCfg->mGuiCfg->SaveClientInfo(app_cfg);
	whDataMgr::GetInstance()->mRecentDstOidPresenter->Save(app_cfg);
}
//---------------------------------------------------------------------------
void ModelMain::ShowDoc()const
{
	
	_TCHAR* path = L"..\\..\\DOC\\Help\\index.htm";
	_TCHAR* parametrs = L"";

	SHELLEXECUTEINFOW ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = path;
	ShExecInfo.lpParameters = parametrs;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;
	BOOL ret = ShellExecuteExW(&ShExecInfo);
}
//---------------------------------------------------------------------------
void ModelMain::ShowWhatIsNew()const
{
	_TCHAR* path = L"..\\..\\DOC\\Help\\whatsnew.htm";
	_TCHAR* parametrs = L"";

	SHELLEXECUTEINFOW ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = path;
	ShExecInfo.lpParameters = parametrs;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;
	BOOL ret = ShellExecuteExW(&ShExecInfo);
}
