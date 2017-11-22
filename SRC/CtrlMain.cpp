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
CtrlMain::CtrlMain(const std::shared_ptr<ViewMain>& view, const std::shared_ptr<ModelMain>& model)
	: CtrlWindowBase(view, model)
{
	auto view_notebook = mView->GetViewNotebook();
	auto model_notebook = std::make_shared<ModelNotebook>();


	mCtrlNotebook = std::make_shared<CtrlNotebook>(view_notebook, model_notebook);

	whDataMgr::GetInstance()->mContainer->RegInstance("CtrlNotebook", mCtrlNotebook);


	auto& db = whDataMgr::GetInstance()->GetDB();
	connAfterDbConnected = db.SigAfterConnect
		.connect(std::bind(&CtrlMain::OnConnectDb, this, std::placeholders::_1));
	connBeforeDbDisconnected = db.SigBeforeDisconnect
		.connect(std::bind(&CtrlMain::OnDicsonnectDb, this, std::placeholders::_1));

	connViewCmd_MkPageGroup = mView->sigMkPageGroup
		.connect(std::bind(&CtrlMain::MkPageGroup, this));
	connViewCmd_MkPageUser = mView->sigMkPageUser
		.connect(std::bind(&CtrlMain::MkPageUser, this));
	connViewCmd_MkPageProp = mView->sigMkPageProp
		.connect(std::bind(&CtrlMain::MkPageProp, this));
	connViewCmd_MkPageAct = mView->sigMkPageAct
		.connect(std::bind(&CtrlMain::MkPageAct, this));
	connViewCmd_MkPageObjByType = mView->sigMkPageObjByType
		.connect(std::bind(&CtrlMain::MkPageObjByType, this));
	connViewCmd_MkPageObjByPath = mView->sigMkPageObjByPath
		.connect(std::bind(&CtrlMain::MkPageObjByPath, this));
	connViewCmd_MkPageHistory = mView->sigMkPageHistory
		.connect(std::bind(&CtrlMain::MkPageHistory, this));
	connViewCmd_MkPageReportList = mView->sigMkPageReportList
		.connect(std::bind(&CtrlMain::MkPageReportList, this));
	connViewCmd_MkPageBrowser = mView->sigMkPageBrowser
		.connect(std::bind(&CtrlMain::MkPageBrowser, this));


	connViewCmd_DoConnectDB = mView->sigDoConnectDB
		.connect(std::bind(&CtrlMain::ConnectDB, this));
	connViewCmd_DoDisconnectDB = mView->sigDoDisconnectDB
		.connect(std::bind(&CtrlMain::DisconnectDB, this));
	
}
//---------------------------------------------------------------------------
void CtrlMain::ConnectDB()
{
	whLogin dlg(this->GetView()->GetWnd());
	dlg.ShowModal();
}
//---------------------------------------------------------------------------
void CtrlMain::DisconnectDB()
{
	whDataMgr::GetInstance()->mDb.Close();
}
//---------------------------------------------------------------------------
void CtrlMain::OnConnectDb(const whDB& db)
{
	mCtrlNotebook->CloseAllPages();
	whDataMgr::GetInstance()->mDbCfg->Load();
	whDataMgr::GetInstance()->mRecentDstOidPresenter->GetFromConfig();
	Load();

	const auto& dbcfg = whDataMgr::GetInstance()->mConnectCfg->GetData();
	const wxString conn_str = wxString::Format("%s %s %d %s"
		, dbcfg.mUser, dbcfg.mServer, dbcfg.mPort, dbcfg.mDB);

	mView->UpdateConnectStatus(conn_str);
}
//---------------------------------------------------------------------------
void CtrlMain::OnDicsonnectDb(const whDB& db)
{
	Save();
	whDataMgr::GetInstance()->mRecentDstOidPresenter->SetToConfig();
	whDataMgr::GetInstance()->mDbCfg->Save();
	whDataMgr::GetInstance()->mConnectCfg->Save();
	mCtrlNotebook->CloseAllPages();

	const auto& dbcfg = whDataMgr::GetInstance()->mConnectCfg->GetData();
	const wxString conn_str = wxString::Format("DISCONNECTED %s %d %s"
		, dbcfg.mServer, dbcfg.mPort, dbcfg.mDB);
	mView->UpdateConnectStatus(conn_str);
}
//---------------------------------------------------------------------------
void CtrlMain::MkPageGroup()
{
	mCtrlNotebook->MkWindow("CtrlPageGroupList");
}
//---------------------------------------------------------------------------
void CtrlMain::MkPageUser()
{
	mCtrlNotebook->MkWindow("CtrlPageUserList");
}
//---------------------------------------------------------------------------
void CtrlMain::MkPageProp()
{
	mCtrlNotebook->MkWindow("CtrlPagePropList");
}
//---------------------------------------------------------------------------
void CtrlMain::MkPageAct()
{
	mCtrlNotebook->MkWindow("CtrlPageActList");
}
//---------------------------------------------------------------------------
void CtrlMain::MkPageObjByType()
{
	mCtrlNotebook->MkWindow("CtrlPageObjByTypeList");
}
//---------------------------------------------------------------------------
void CtrlMain::MkPageObjByPath()
{
	mCtrlNotebook->MkWindow("CtrlPageObjByPathList");
}
//---------------------------------------------------------------------------
void CtrlMain::MkPageHistory()
{
	//auto container = whDataMgr::GetInstance()->mContainer;
	//auto nb2 = container->GetObject<wh::CtrlNotebook>("CtrlNotebook");
	//if (nb2)
	//	nb2->MkWindow("CtrlPageHistory");
	mCtrlNotebook->MkWindow("CtrlPageHistory");
}
//---------------------------------------------------------------------------
void CtrlMain::MkPageReportList()
{
	mCtrlNotebook->MkWindow("CtrlPageReportList");
}
//---------------------------------------------------------------------------
void CtrlMain::MkPageBrowser()
{
	mCtrlNotebook->MkWindow("CtrlPageBrowser");
}
//---------------------------------------------------------------------------
std::shared_ptr<CtrlNotebook> CtrlMain::GetNotebook()
{ 
	return mCtrlNotebook; 
}
//---------------------------------------------------------------------------
void CtrlMain::Load()
{
	using ptree = boost::property_tree::wptree;
	const ptree& app_cfg = whDataMgr::GetInstance()->mDbCfg->mGuiCfg->GetData();
	Load(app_cfg);
}
//---------------------------------------------------------------------------
void CtrlMain::Save()
{
	using ptree = boost::property_tree::wptree;
	ptree app_cfg;
	Save(app_cfg);
	whDataMgr::GetInstance()->mDbCfg->mGuiCfg->SetData(app_cfg);
}
//---------------------------------------------------------------------------
void CtrlMain::RmView()//override
{
	whDataMgr::GetInstance()->GetDB().Close();
	//Save();
	mCtrlNotebook->RmView();
	//CtrlWindowBase::RmView();
}
//---------------------------------------------------------------------------
void CtrlMain::Load(const boost::property_tree::wptree& app_cfg) //override
{
	mModel->Load(app_cfg);

	auto it = app_cfg.find(L"CtrlNotebook");
	if (app_cfg.not_found() != it)
		mCtrlNotebook->Load(it->second);

	it = app_cfg.find(L"Default");
	if (app_cfg.not_found() != it)
		whDataMgr::GetInstance()->mDbCfg->mGuiCfg->LoadDefaults(it->second);

}
//---------------------------------------------------------------------------
void CtrlMain::Save(boost::property_tree::wptree& app_cfg) //override
{
	mModel->Save(app_cfg);

	using ptree = boost::property_tree::wptree;
	ptree notebook;
	mCtrlNotebook->Save(notebook);
	app_cfg.add_child(L"CtrlNotebook", notebook);

	ptree defaults;
	whDataMgr::GetInstance()->mDbCfg->mGuiCfg->SaveDefaults(defaults);
	app_cfg.add_child(L"Default", defaults);
}

