#ifndef __VIEW_VIEWOBJPROPLIST_H
#define __VIEW_VIEWOBJPROPLIST_H
//-----------------------------------------------------------------------------
#include "_pch.h"
#include "IViewObjPropList.h"

namespace wh{
//-----------------------------------------------------------------------------
class ViewObjPropList : public IViewObjPropList
{
	wxPanel*		mPanel;
	wxPropertyGrid*	mPG;

public:
	ViewObjPropList(wxWindow* parent);
	ViewObjPropList(const std::shared_ptr<IViewWindow>& parent);
	virtual wxWindow* GetWnd()const override;
	
	virtual void SetPropList(const PropValTable& rt, const IAct*) override;

protected:
	void OnCmd_Update(wxCommandEvent& evt = wxCommandEvent());
	
	virtual void OnShow()override;
};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H