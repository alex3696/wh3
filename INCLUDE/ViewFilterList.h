#ifndef __VIEW_FILTERLIST_H
#define __VIEW_FILTERLIST_H
//-----------------------------------------------------------------------------
#include "_pch.h"
#include "IViewFilterList.h"

namespace wh{
//-----------------------------------------------------------------------------
class ViewFilterList : public IViewFilterList
{
	wxPanel*		mPanel;
	wxAuiManager*   mAuiMgr;
	wxPropertyGrid* mPG;
public:
	using NotyfyItem = ModelFilterList::NotyfyItem;

	ViewFilterList(std::shared_ptr<IViewWindow> parent);
	ViewFilterList(wxWindow* parent);
	virtual wxWindow* GetWnd()const override;

	virtual void Update(const std::vector<NotyfyItem>& data) override;
protected:
	void OnCmd_Update(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_UpdateAll(wxCommandEvent& evt = wxCommandEvent());

	virtual void OnShow()override;
private:
};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H