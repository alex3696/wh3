#ifndef __CTRLWINDOWBASE_H
#define __CTRLWINDOWBASE_H

#include "IModelWindow.h"
#include "IViewWindow.h"
#include "ICtrlWindow.h"

namespace wh{
//---------------------------------------------------------------------------

template <class TVIEW, class TMODEL>
class CtrlWindowBase : public ICtrlWindow
{
protected:
	std::shared_ptr<TVIEW>  mView;
	std::shared_ptr<TMODEL> mModel;

	sig::scoped_connection connViewUpdateTitle;
	sig::scoped_connection connViewClose;
	sig::scoped_connection connViewShow;

	sig::scoped_connection connModelUpdateTitle;
	sig::scoped_connection connModelClose;
	sig::scoped_connection connModelShow;

	sig::scoped_connection connCtrlUpdateTitle;
	sig::scoped_connection connCtrlClose;
	sig::scoped_connection connCtrlShow;

public:
	CtrlWindowBase(std::shared_ptr<TVIEW> view, std::shared_ptr<TMODEL> model)
		:mView(view)
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

		connCtrlUpdateTitle = mModel->sigUpdateTitle
			.connect(std::bind(std::ref(sigUpdateTitle), this, ph::_1, ph::_2));
		connCtrlClose = mModel->sigClose
			.connect(std::bind(std::ref(sigClose), this));
		connCtrlShow = mModel->sigShow
			.connect(std::bind(std::ref(sigShow), this));
	}

	//virtual std::shared_ptr<IModelWindow > GetModel()const override	{ return mModel; }
	virtual std::shared_ptr<IViewWindow> GetView()const override	{ return mView; }

	virtual void UpdateTitle()override	{ mModel->UpdateTitle(); }
	virtual void Show()override			{ mModel->Show(); }
	virtual void Close()override		{ mModel->Close(); }

	virtual void Load(const boost::property_tree::ptree& val) override
	{
		mModel->Load(val);
	}

	virtual void Save(boost::property_tree::ptree& val) override
	{
		mModel->Save(val);
	}

};


//---------------------------------------------------------------------------
} //namespace mvp{
#endif // __IMVP_H