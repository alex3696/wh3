#ifndef __VIEW_DETAIL_H
#define __VIEW_DETAIL_H
//-----------------------------------------------------------------------------
#include "_pch.h"
#include "IViewDetail.h"

namespace wh{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ViewToolbarDetail final : public IViewToolbarDetail
{
	wxAuiToolBar*		mToolbar;
public:
	ViewToolbarDetail(wxWindow* parent);
	ViewToolbarDetail(std::shared_ptr<IViewWindow> parent)
		:ViewToolbarDetail(parent->GetWnd())
	{}
	virtual wxWindow* GetWnd()const override
	{
		return mToolbar;
	}

	virtual void SetObj(const rec::ObjInfo& rt) override;

	void OnCmd_Move(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_Act(wxCommandEvent& evt);
	void OnCmd_Redo(wxCommandEvent& evt);
protected:
private:

};
//-----------------------------------------------------------------------------
class ViewPageDetail final : public IViewPageDetail
{
	wxPanel*		mPanel;
	wxAuiManager*   mAuiMgr;

	std::shared_ptr<IViewToolbarDetail>		mViewToolbarDetail;
	std::shared_ptr<IViewToolbarHistory>	mViewToolbarHistory;
	std::shared_ptr<IViewTableHistory>		mViewTableHistory;
	std::shared_ptr<IViewFilterList>		mViewFilterList;
	std::shared_ptr<IViewObjPropList>		mViewObjPropList;
public:
	ViewPageDetail(std::shared_ptr<IViewWindow> parent);
	virtual wxWindow* GetWnd()const override
	{
		return mPanel;
	}

	virtual std::shared_ptr<IViewToolbarDetail>		GetViewToolbarDetail()const override;
	virtual std::shared_ptr<IViewToolbarHistory>	GetViewToolbarHistory()const override;
	virtual std::shared_ptr<IViewTableHistory>		GetViewTableHistory()const override;
	virtual std::shared_ptr<IViewFilterList>		GetViewFilterList()const override;
	virtual std::shared_ptr<IViewObjPropList>		GetViewObjPropList()const override;
	virtual void SetCfg(const rec::PageHistory&) override;

	void ShowFilterList(bool);
	void ShowObjPropList(bool);

protected:

private:
	rec::PageHistory mCfg;

	
};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H