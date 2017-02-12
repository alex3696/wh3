#ifndef __IPAGEVIEW_H
#define __IPAGEVIEW_H

#include "IMVP.h"
#include "TModel.h"
namespace mvp{
//---------------------------------------------------------------------------
class IPageView : public IView
{
public:
	virtual wxWindow* GetWnd() override = 0;
	virtual void SetWnd(wxWindow* wnd)override = 0;
};

//---------------------------------------------------------------------------
class PageView : public IPageView
{
public:
	PageView()
		: mWnd(nullptr)
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
protected:
	wxWindow* mWnd;

};





} //namespace mvp{
#endif // __********_H