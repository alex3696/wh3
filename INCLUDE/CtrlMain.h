#ifndef __CTRLMAIN_H
#define __CTRLMAIN_H

//#include "IViewWindow.h"
#include "ViewMain.h"

#include "ICtrlWindow.h"
#include "ModelMain.h"

namespace wh{
//---------------------------------------------------------------------------
class CtrlMain : public CtrlWindowBase<ViewMain, ModelMain>
{
	std::shared_ptr<CtrlNotebook> mCtrlNotebook;

public:
	CtrlMain(std::shared_ptr<ViewMain> view, std::shared_ptr<ModelMain> model)
		: CtrlWindowBase(view, model)
	{
		auto view_notebook = mView->GetViewNotebook();
		auto model_notebook = std::make_shared<ModelNotebook>();


		mCtrlNotebook = std::make_shared<CtrlNotebook>(view_notebook, model_notebook);
		
		whDataMgr::GetInstance()->mContainer->RegInstance("CtrlNotebook", mCtrlNotebook);

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


};



//---------------------------------------------------------------------------
} //namespace wh
#endif // __***_H