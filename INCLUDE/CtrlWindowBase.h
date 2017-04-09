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

	void OnSig_ModelUpdateTitle(const wxString& title, const wxIcon& ico)
	{
		mView->OnUpdateTitle(title, ico);
		sigUpdateTitle(this, title, ico);
	}
	void OnSig_ModelShow()
	{
		mView->OnShow();
		sigShow(this);
	}
	void OnSig_ModelClose()
	{
		mView->OnCloseModel();
		sigCloseModel(this);
		DisconnectModel();
	}


	void OnSig_OnCloseView()
	{
		sigCloseView(this);
		RmView();
	}

	virtual void ConnectView()
	{
		if (!mView)
			return;
		namespace ph = std::placeholders;
		connViewUpdateTitle = mView->sigUpdateTitle
			.connect(std::bind(&IModelWindow::UpdateTitle, mModel.get()));
		connViewClose = mView->sigClose
			.connect(std::bind(&CtrlWindowBase::OnSig_OnCloseView, this));
		connViewShow = mView->sigShow
			.connect(std::bind(&IModelWindow::Show, mModel.get()));
	}
	virtual void DisconnectView()
	{
		connViewUpdateTitle.disconnect();
		connViewClose.disconnect();
		connViewShow.disconnect();
	}

	virtual void ConnectModel()
	{
		namespace ph = std::placeholders;
		connModelUpdateTitle = mModel->sigUpdateTitle
			.connect(std::bind(&CtrlWindowBase::OnSig_ModelUpdateTitle, this, ph::_1, ph::_2));
		connModelClose = mModel->sigClose
			.connect(std::bind(&CtrlWindowBase::OnSig_ModelClose, this));
		connModelShow = mModel->sigShow
			.connect(std::bind(&CtrlWindowBase::OnSig_ModelShow, this));
	}
	virtual void DisconnectModel()
	{
		connModelUpdateTitle.disconnect();
		connModelClose.disconnect();
		connModelShow.disconnect();;
	}
public:
	CtrlWindowBase(const std::shared_ptr<TVIEW>& view, const std::shared_ptr<TMODEL>& model)
		:mView(view)
		, mModel(model)
	{
		ConnectModel();
		ConnectView();

	}

	//virtual std::shared_ptr<IModelWindow > GetModel()const override	{ return mModel; }
	virtual std::shared_ptr<IViewWindow> GetView()const override	{ return mView; }

	virtual void UpdateTitle()override	{ mModel->UpdateTitle(); }
	virtual void Show()override			{ mModel->Show(); }
	virtual void RmView()override		
	{ 
		DisconnectView();
		mView.reset();
	}

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