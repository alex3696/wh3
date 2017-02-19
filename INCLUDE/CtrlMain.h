#ifndef __CTRLMAIN_H
#define __CTRLMAIN_H

//#include "IViewWindow.h"
#include "ViewMain.h"

#include "ICtrlWindow.h"
#include "ModelMain.h"

namespace wh{
//---------------------------------------------------------------------------
class CtrlMain :public ICtrlWindow
{
	sig::scoped_connection connViewUpdateTitle;
	sig::scoped_connection connViewClose;
	sig::scoped_connection connViewShow;

	sig::scoped_connection connModelUpdateTitle;
	sig::scoped_connection connModelClose;
	sig::scoped_connection connModelShow;

	sig::scoped_connection connModelAfterMkNotebook;

	
	std::shared_ptr<ViewMain>	mView;
	std::shared_ptr<ModelMain>	mModel;


	std::shared_ptr<CtrlNotebook> mCtrlNotebook;
public:
	CtrlMain(std::shared_ptr<ViewMain> view, std::shared_ptr<ModelMain> model)
		: mView(view)
		, mModel(model)
	{
		namespace ph = std::placeholders;
		connViewUpdateTitle = mView->sigUpdateTitle
			.connect(std::bind(&IModelWindow::UpdateTitle, mModel.get()));
		connViewClose = mView->sigClose
			.connect(std::bind(&IModelWindow::Close, mModel.get()));
		connViewShow = mView->sigShow
			.connect(std::bind(&IModelWindow::Show, mModel.get()));

		connModelUpdateTitle = mModel->sigUpdateTitle
			.connect(std::bind(&IViewWindow::OnUpdateTitle, mView.get(), ph::_1, ph::_2));
		connModelClose = mModel->sigClose
			.connect(std::bind(&IViewWindow::OnClose, mView.get()));
		connModelShow = mModel->sigShow
			.connect(std::bind(&IViewWindow::OnShow, mView.get()));


		auto view_notebook = mView->GetViewNotebook();
		auto model_notebook = std::make_shared<ModelNotebook>();


		mCtrlNotebook = std::make_shared<CtrlNotebook>(view_notebook, model_notebook);
		
		whDataMgr::GetInstance()->mContainer->RegInstance("CtrlNotebook", mCtrlNotebook);

	}

	virtual std::shared_ptr<IViewWindow> GetView()const override { return mView; }

	virtual void UpdateTitle() override { mModel->UpdateTitle(); }
	virtual void Show() override{ mModel->Show(); }
	virtual void Close() override{ mModel->Close();  }
	
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


	//sig::signal<void(wxWindow*, const wxString&, const wxIcon&)>	sigUpdateTitle;
	//sig::signal<void(wxWindow*)>	sigClose;
	//sig::signal<void(wxWindow*)>	sigShow;

};



//---------------------------------------------------------------------------
} //namespace wh
#endif // __***_H