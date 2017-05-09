#ifndef __CTRLMAIN_H
#define __CTRLMAIN_H

//#include "IViewWindow.h"
#include "ViewMain.h"

#include "ICtrlWindow.h"
#include "ModelMain.h"
#include "RecentDstOidPresenter.h"

namespace wh{
//---------------------------------------------------------------------------
class CtrlMain : public CtrlWindowBase<ViewMain, ModelMain>
{
	std::shared_ptr<CtrlNotebook> mCtrlNotebook;

	sig::scoped_connection connAfterDbConnected;
	sig::scoped_connection connBeforeDbDisconnected;

	sig::scoped_connection connViewCmd_MkHistoryWindow;


	void OnConnectDb(const whDB& db)
	{
		mCtrlNotebook->CloseAllPages();
		whDataMgr::GetInstance()->mDbCfg->Load();
		whDataMgr::GetInstance()->mRecentDstOidPresenter->GetFromConfig();
		Load();
	}

	void OnDicsonnectDb(const whDB& db)
	{
		Save();
		whDataMgr::GetInstance()->mRecentDstOidPresenter->SetToConfig();
		whDataMgr::GetInstance()->mDbCfg->Save();
		mCtrlNotebook->CloseAllPages();
	}
public:
	CtrlMain(const std::shared_ptr<ViewMain>& view, const std::shared_ptr<ModelMain>& model)
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

		connViewCmd_MkHistoryWindow = mView->sigMkHistoryWindow
			.connect(std::bind(&CtrlMain::MkHistoryWindow, this));
	}

	virtual void RmView()override
	{
		whDataMgr::GetInstance()->GetDB().Close();
		//Save();
		mCtrlNotebook->RmView();
		CtrlWindowBase::RmView();
	}

	void MkHistoryWindow()
	{
		//auto container = whDataMgr::GetInstance()->mContainer;
		//auto nb2 = container->GetObject<wh::CtrlNotebook>("CtrlNotebook");
		//if (nb2)
		//	nb2->MkWindow("CtrlPageHistory");
		mCtrlNotebook->MkWindow("CtrlPageHistory");
	}

	virtual void Load(const boost::property_tree::ptree& app_cfg) override
	{
		mModel->Load(app_cfg);

		auto it = app_cfg.find("CtrlNotebook");
		if (app_cfg.not_found() == it)
			return;

		mCtrlNotebook->Load(it->second);
	}
	virtual void Save(boost::property_tree::ptree& app_cfg) override
	{
		mModel->Save(app_cfg);
		
		using ptree = boost::property_tree::ptree;
		ptree notebook;
		mCtrlNotebook->Save(notebook);
		app_cfg.add_child("CtrlNotebook", notebook);
	}

	void Load()
	{
		using ptree = boost::property_tree::ptree;
		const ptree& app_cfg = whDataMgr::GetInstance()->mDbCfg->mGuiCfg->GetData();
		Load(app_cfg);
	}
	void Save()
	{
		using ptree = boost::property_tree::ptree;
		ptree app_cfg = whDataMgr::GetInstance()->mDbCfg->mGuiCfg->GetData();
		Save(app_cfg);
		whDataMgr::GetInstance()->mDbCfg->mGuiCfg->SetData(app_cfg);
	}


	std::shared_ptr<CtrlNotebook> GetNotebook() { return mCtrlNotebook; }
};



//---------------------------------------------------------------------------
} //namespace wh
#endif // __***_H