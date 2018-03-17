#include "_pch.h"
#include "CtrlMain.h"
#include "config.h"
#include "globaldata.h"
#include "RecentDstOidPresenter.h"
#include "whLogin.h"
#include "CtrlHelp.h"

using namespace wh;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CtrlMain::CtrlMain(const std::shared_ptr<ViewMain>& view, const std::shared_ptr<ModelMain>& model)
	: CtrlWindowBase(view, model)
{
	namespace ph = std::placeholders;

	auto view_notebook = mView->GetViewNotebook();
	auto model_notebook = std::make_shared<ModelNotebook>();


	mCtrlNotebook = std::make_shared<CtrlNotebook>(view_notebook, model_notebook);

	whDataMgr::GetInstance()->mContainer->RegInstance("CtrlNotebook", mCtrlNotebook);

	conn_ShowHelp = mModel->sigShowHelp
		.connect(std::bind(&CtrlMain::ShowHelp, this, ph::_1 ));


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
	connViewCmd_MkPageBrowserCls = mView->sigMkPageBrowserCls
		.connect(std::bind(&CtrlMain::MkPageBrowserCls, this));
	connViewCmd_MkPageBrowserObj = mView->sigMkPageBrowserObj
		.connect(std::bind(&CtrlMain::MkPageBrowserObj, this));


	connViewCmd_DoConnectDB = mView->sigDoConnectDB
		.connect(std::bind(&CtrlMain::ConnectDB, this));
	connViewCmd_DoDisconnectDB = mView->sigDoDisconnectDB
		.connect(std::bind(&CtrlMain::DisconnectDB, this));

	connViewCmd_ShowDoc = mView->sigShowDoc
		.connect(std::bind(&CtrlMain::ShowDoc, this));
	connViewCmd_ShowWhatIsNew = mView->sigShowWhatIsNew
		.connect(std::bind(&CtrlMain::ShowWhatIsNew, this));

	connViewCmd_Close = mView->sigClose
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
	wxWindowUpdateLocker lock(mView->GetWnd());

	mCtrlNotebook->CloseAllPages();
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
void CtrlMain::MkPageBrowserCls()
{
	mCtrlNotebook->MkWindow("CtrlPageBrowserCls");
}
//---------------------------------------------------------------------------
void CtrlMain::MkPageBrowserObj()
{
	mCtrlNotebook->MkWindow("CtrlPageBrowserObj");
}
//---------------------------------------------------------------------------
std::shared_ptr<CtrlNotebook> CtrlMain::GetNotebook()
{ 
	return mCtrlNotebook; 
}
//---------------------------------------------------------------------------
void CtrlMain::Load()
{
	TEST_FUNC_TIME;
	using ptree = boost::property_tree::wptree;

	whDataMgr::GetInstance()->mDbCfg->Load();
	const ptree& app_cfg = whDataMgr::GetInstance()->mDbCfg->mGuiCfg->GetData();

	mModel->Load(app_cfg);
	mCtrlNotebook->Load(app_cfg);
}
//---------------------------------------------------------------------------
void CtrlMain::Save()
{
	TEST_FUNC_TIME;
	using ptree = boost::property_tree::wptree;
	
	ptree app_cfg;
	mModel->Save(app_cfg);
	mCtrlNotebook->Save(app_cfg);
	whDataMgr::GetInstance()->mDbCfg->mGuiCfg->SetData(app_cfg);
	whDataMgr::GetInstance()->mDbCfg->Save();
}
//---------------------------------------------------------------------------
void CtrlMain::ShowHelp(const wxString& index)const
{
	auto container = whDataMgr::GetInstance()->mContainer;
	auto ctrl_help = container->GetObject<CtrlHelp>("CtrlHelp");
	if (!ctrl_help)
		return;

	ctrl_help->Show(index);
}
//---------------------------------------------------------------------------
void CtrlMain::ShowDoc() const
{
	ShowHelp(L"index");
}
//---------------------------------------------------------------------------
void wh::CtrlMain::ShowWhatIsNew() const
{
	ShowHelp(L"whatsnew");
}
//---------------------------------------------------------------------------
void CtrlMain::RmView()//override
{
	whDataMgr::GetInstance()->GetDB().Close();
	mCtrlNotebook->RmView();
}
