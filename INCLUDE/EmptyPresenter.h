#ifndef __EMPTYPRESENTER_H
#define __EMPTYPRESENTER_H

#include "IMVP.h"
namespace mvp{
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class EmptyView
	: public IView
{
public:
	EmptyView(IPresenter* presenter, wxWindow* wnd)
		: IView(presenter)
		, mWnd(wnd)
	{}

	virtual wxWindow* GetWnd() override { return mWnd; }
	virtual void SetWnd(wxWindow* wnd)override{ mWnd = wnd; }
private:
	wxWindow* mWnd;
};
//---------------------------------------------------------------------------

class EmptyPresenter : public IPresenter
{
public:
	EmptyPresenter(wxWindow* wnd)
		:IPresenter(nullptr)
	{
		mView.reset(new EmptyView(this, wnd));
	}

	virtual IView* MakeView()override
	{
		mView.reset(new EmptyView(this, nullptr));
		return mView.get();
	};
	virtual void SetView(IView* view)override 
	{ 
		throw;
	}
	virtual IView* GetView() override { return mView.get(); }

	virtual void SetModel(const std::shared_ptr<IModel>& model)override { mModel = model; }
	virtual std::shared_ptr<IModel> GetModel() override { return mModel; }
	
private:
	std::unique_ptr<EmptyView>	mView;
	std::shared_ptr<IModel>		mModel;

};
//---------------------------------------------------------------------------
} // namespace mvp{
#endif // __IMVP_H