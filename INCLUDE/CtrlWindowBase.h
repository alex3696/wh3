#ifndef __CTRLWINDOWBASE_H
#define __CTRLWINDOWBASE_H

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

	virtual void DisconnectModel()
	{
		connModelUpdateTitle.disconnect();
		connModelClose.disconnect();
		connModelShow.disconnect();
	}

	virtual void ConnectView()
	{
		if (!mView)
			return;
		namespace ph = std::placeholders;
		connViewUpdateTitle = mView->sigUpdateTitle
			.connect(std::bind(&IModelWindow::UpdateTitle, mModel.get()));
		connViewClose = mView->sigClose
			.connect(std::bind(&IModelWindow::Close , mModel.get()));
		connViewShow = mView->sigShow
			.connect(std::bind(&IModelWindow::Show, mModel.get()));
		ConnectModel();// receive signals from model if View exists
	}
	virtual void DisconnectView()
	{
		connViewUpdateTitle.disconnect();
		connViewClose.disconnect();
		connViewShow.disconnect();
		DisconnectModel();// don`t receive signals from model while View is NULL
	}

	virtual void ConnectModel()
	{
		namespace ph = std::placeholders;
		connModelUpdateTitle = mModel->sigUpdateTitle
			.connect(std::bind(&IViewWindow::SetUpdateTitle, mView.get(), ph::_1, ph::_2));
		connModelClose = mModel->sigClose
			.connect(std::bind(&IViewWindow::SetClose, mView.get()));
		connModelShow = mModel->sigShow
			.connect(std::bind(&IViewWindow::SetShow, mView.get()));
	}
public:
	CtrlWindowBase(const std::shared_ptr<TVIEW>& view, const std::shared_ptr<TMODEL>& model)
		:mView(view)
		, mModel(model)
	{
		ConnectModel();
		ConnectView();

	}

	virtual std::shared_ptr<IModelWindow > GetModel()const override { return mModel; }
	virtual std::shared_ptr<IViewWindow> GetView()const override	{ return mView; }

	virtual void UpdateTitle()override	{ mModel->UpdateTitle(); }
	virtual void Show()override			{ mModel->Show(); }
	virtual void RmView()override		
	{ 
		DisconnectView();
		mView.reset();
	}
	virtual void Init() override 
	{
		mModel->Init();
	}


	virtual void Load(const boost::property_tree::wptree& val) override
	{
		mModel->Load(val);
	}

	virtual void Save(boost::property_tree::wptree& val) override
	{
		mModel->Save(val);
	}

};


//---------------------------------------------------------------------------
} //namespace mvp{
#endif // __IMVP_H