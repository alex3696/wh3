#ifndef __VIEW_BROWSER_H
#define __VIEW_BROWSER_H
//-----------------------------------------------------------------------------

#include "IViewBrowser.h"
#include "ModelHistoryData.h"



namespace wh{
//-----------------------------------------------------------------------------
class ViewTableBrowser : public IViewTableBrowser
{
	wxDataViewCtrl* mTable;

	bool			mColAutosize = true;
protected:
	void OnCmd_Refresh(wxCommandEvent& evt);
	void OnCmd_Up(wxCommandEvent& evt);
	void OnCmd_Select(wxDataViewEvent& evt);
	void OnCmd_MouseMove(wxMouseEvent& evt);
	void OnCmd_Activate(wxDataViewEvent& evt);
	
public:
	ViewTableBrowser(wxWindow* parent);
	ViewTableBrowser(std::shared_ptr<IViewWindow> parent);
	virtual wxWindow* GetWnd()const override
	{
		return mTable;
	}

	virtual void SetGroupByType(bool enable) override;
	virtual void SetCollapsedGroupByType(bool enable) override;
	virtual void SetSelect(const NotyfyTable& list) override;

	virtual void SetClear() override;
	virtual void SetAfterInsert(const NotyfyTable& list) override;
	virtual void SetAfterUpdate(const NotyfyTable& list) override;
	virtual void SetBeforeDelete(const NotyfyTable& list) override;
	virtual void SetPathMode(const int mode) override;
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
	virtual void SetCollapsedGroupByType(bool enable) override;


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
	virtual void SetPathString(const wxString& path_string) override;
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
	virtual void SetPathString(const wxString& path_string) override;
};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H