#ifndef __IPAGEVIEW_H
#define __IPAGEVIEW_H

#include "IMVP.h"
#include "TModel.h"
namespace mvp{
//---------------------------------------------------------------------------
class IPageView : public IView
{
public:
	IPageView(IPresenter* presenter)
		:IView(presenter)
		, mWnd(nullptr)
	{}

	virtual wxWindow* GetWnd() override
	{
		return mWnd;
	}
	virtual void SetWnd(wxWindow* wnd)override
	{
		if (mWnd)
			delete mWnd;
		mWnd = wnd;
	}
private:
	wxWindow* mWnd;

};
} //namespace mvp{
#endif // __********_H