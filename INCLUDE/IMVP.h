#ifndef __IMVP_H
#define __IMVP_H

#include "_pch.h"
namespace mvp{
//---------------------------------------------------------------------------
class IPresenter;
//---------------------------------------------------------------------------
class IModel
{
public:
	virtual ~IModel(){}
};
//---------------------------------------------------------------------------
class IView
{
public:
	IView() :mPresenter(nullptr){}
	IView(IPresenter* presenter) :mPresenter(presenter){}
	virtual ~IView(){}

	virtual wxWindow* GetWnd() = 0;
	virtual void SetWnd(wxWindow*) = 0;

	IPresenter* GetPresenter(){ return mPresenter; }
	void SetPresenter(IPresenter* presenter){ mPresenter = presenter; }

private:
	IPresenter* mPresenter;
};
//---------------------------------------------------------------------------
class IPresenter
{
public:
	IPresenter(IPresenter* parent) :mParent(parent){}
	virtual ~IPresenter()
	{
	
	};

	virtual void SetView(IView* view) = 0;
	virtual void SetModel(const std::shared_ptr<IModel>& model) = 0;
	virtual std::shared_ptr<IModel> GetModel() = 0;
	virtual IView* GetView() = 0;

	IPresenter* GetParent() { return mParent; }
private:
	IPresenter* mParent;

};
//---------------------------------------------------------------------------
} //namespace mvp{
#endif // __IMVP_H
	
