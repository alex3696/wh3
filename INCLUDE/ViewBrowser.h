#ifndef __VIEW_BROWSER_H
#define __VIEW_BROWSER_H
//-----------------------------------------------------------------------------

#include "IViewBrowser.h"
#include "ModelHistoryData.h"



namespace wh{
//-----------------------------------------------------------------------------
class ViewTableBrowser : public IViewTableBrowser
{
	int64_t mClsSelected = 0;
	int64_t mObjSelected = 0;

	wxDataViewCtrl* mTable;
	bool mGroupByType = true;
	const ICls64*	mNode = nullptr;
	bool mColAutosize = true;
	
	size_t FindChildClsPos(const int64_t& id)const;
	const ICls64* FindChildCls(const int64_t& id)const;
	const ICls64* GetTopChildCls()const;
	void RestoreSelect();
	void AutosizeColumns();
protected:
	void OnCmd_Refresh(wxCommandEvent& evt);
	void OnCmd_Up(wxCommandEvent& evt);
	void OnCmd_Select(wxDataViewEvent& evt);
	void OnCmd_MouseMove(wxMouseEvent& evt);
	void OnCmd_Activate(wxDataViewEvent& evt);
	void OnCmd_Expanding(wxDataViewEvent& evt);
	
public:
	ViewTableBrowser(wxWindow* parent);
	ViewTableBrowser(std::shared_ptr<IViewWindow> parent);
	virtual wxWindow* GetWnd()const override
	{
		return mTable;
	}

	virtual void SetBeforePathChange(const ICls64& node) override;
	virtual void SetAfterPathChange(const ICls64& node) override;


	virtual void SetGroupByType(bool enable) override;

	virtual void SetObjOperation(Operation, const std::vector<const IObj64*>&) override;
};
//-----------------------------------------------------------------------------
class ViewToolbarBrowser : public IViewToolbarBrowser
{
	wxAuiToolBar*		mToolbar;
public:
	ViewToolbarBrowser(wxWindow* parent);
	ViewToolbarBrowser(std::shared_ptr<IViewWindow> parent);
	virtual wxWindow* GetWnd()const override
	{
		return mToolbar;
	}

	virtual void SetVisibleFilters(bool enable) override;
	virtual void SetGroupByType(bool enable) override;


	void OnCmd_Refresh(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_Up(wxCommandEvent& evt = wxCommandEvent());

	void OnCmd_Act(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_Move(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_Find(wxCommandEvent& evt = wxCommandEvent());

	void OnCmd_AddType(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_AddObject(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_DeleteSelected(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_UpdateSelected(wxCommandEvent& evt = wxCommandEvent());

	void OnCmd_GroupByType(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_CollapseGroupByType(wxCommandEvent& evt = wxCommandEvent());

	//void OnCmd_ShowDetail(wxCommandEvent& evt = wxCommandEvent());
	//void OnCmd_ShowFilters(wxCommandEvent& evt = wxCommandEvent());
	//void OnCmd_ShowHistory(wxCommandEvent& evt = wxCommandEvent());
	//void OnCmd_ShowProperties(wxCommandEvent& evt = wxCommandEvent());


};
//-----------------------------------------------------------------------------
class ViewPathBrowser : public IViewPathBrowser
{
	wxTextCtrl* mPathCtrl;
public:
	ViewPathBrowser(wxWindow* parent);
	ViewPathBrowser(std::shared_ptr<IViewWindow> parent);
	virtual wxWindow* GetWnd()const override
	{
		return mPathCtrl;
	}

	virtual void SetPathMode(const int mode) override;
	virtual void SetPathString(const ICls64& node) override;
};
//-----------------------------------------------------------------------------
class ViewBrowserPage : public IViewBrowserPage
{
	wxPanel*		mPanel;
	wxAuiManager*   mAuiMgr;

	std::shared_ptr<IViewToolbarBrowser>	mViewToolbarBrowser;
	std::shared_ptr<IViewPathBrowser>		mViewPathBrowser;
	std::shared_ptr<IViewTableBrowser>		mViewTableBrowser;
	

protected:
	virtual void OnShow()override
	{

	}
public:
	ViewBrowserPage(wxWindow* parent);
	ViewBrowserPage(std::shared_ptr<IViewWindow> parent);
	virtual wxWindow* GetWnd()const override
	{
		return mPanel;
	}

	virtual std::shared_ptr<IViewToolbarBrowser>	GetViewToolbarBrowser()const override;
	virtual std::shared_ptr<IViewPathBrowser>		GetViewPathBrowser()const override;
	virtual std::shared_ptr<IViewTableBrowser>		GetViewTableBrowser()const override;
	

	virtual void SetPathMode(const int mode) override;
	virtual void SetPathString(const ICls64& node) override;
};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H