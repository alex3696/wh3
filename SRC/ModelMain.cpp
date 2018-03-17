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


	const auto& version = *cnt->GetObject<std::wstring>(L"ClientInfoVersion");
	const auto appvers = GetAppVersion();
	
	auto last_login = cnt->GetObject<wxDateTime>(L"ClientInfoLastLogin");
	if (!last_login->IsValid()
		|| wxDateTime::Now() > (*last_login + wxTimeSpan(720, 0, 0, 0))
		|| appvers > version
		)

		sigShowHelp("whatsnew");
}
//---------------------------------------------------------------------------
//virtual 
void ModelMain::Save(boost::property_tree::wptree& app_cfg)//override
{
	whDataMgr::GetInstance()->mDbCfg->mGuiCfg->SaveDefaults(app_cfg);
	whDataMgr::GetInstance()->mDbCfg->mGuiCfg->SaveClientInfo(app_cfg);
	whDataMgr::GetInstance()->mRecentDstOidPresenter->Save(app_cfg);
}
