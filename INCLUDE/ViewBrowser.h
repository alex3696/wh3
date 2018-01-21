#ifndef __VIEW_BROWSER_H
#define __VIEW_BROWSER_H
//-----------------------------------------------------------------------------

#include "IViewBrowser.h"
#include "ModelHistoryData.h"



namespace wh{


//-----------------------------------------------------------------------------
class ViewTableBrowser : public IViewTableBrowser
{
	int64_t							mParentCid = 0;
	std::vector<const IIdent64*>	mClsList;

	wxDataViewColumn* mSortCol = nullptr;
	bool mSortAsc = true;
	
	
	
	int64_t mClsSelected = 0;
	int64_t mObjSelected = 0;
	std::set<int64_t> mExpandedCls;

	wxDataViewCtrl* mTable;
	


	bool mColAutosize = true;
	
	const IIdent64* FindChildCls(const int64_t& id)const;
	const IIdent64* GetTopChildCls()const;
	
	void StoreSelect();
	void RestoreSelect();
	void AutosizeColumns();

	void ResetColumns();
	void RebuildClsColumns(const std::vector<const IIdent64*>&);
protected:
	void OnCmd_Refresh(wxCommandEvent& evt);
	void OnCmd_Up(wxCommandEvent& evt);
	void OnCmd_MouseMove(wxMouseEvent& evt);
	void OnCmd_Activate(wxDataViewEvent& evt);
	void OnCmd_Expanding(wxDataViewEvent& evt);
	void OnCmd_Expanded(wxDataViewEvent& evt);

	void OnCmd_Collapseded(wxDataViewEvent& evt);
	
public:
	ViewTableBrowser(wxWindow* parent);
	ViewTableBrowser(const std::shared_ptr<IViewWindow>& parent);
	virtual wxWindow* GetWnd()const override
	{
		return mTable;
	}

	virtual void SetBeforeRefreshCls(const std::vector<const IIdent64*>&, const IIdent64*, const wxString&, bool) override;
	virtual void SetAfterRefreshCls(const std::vector<const IIdent64*>&, const IIdent64*, const wxString&, bool) override;

	virtual void SetObjOperation(Operation, const std::vector<const IIdent64*>&) override;
	virtual void SetShowDetail()override;
	virtual void SetInsertType()const override;
	virtual void SetInsertObj()const override;
	virtual void SetDeleteSelected()const override;
	virtual void SetUpdateSelected()const override;

};
//-----------------------------------------------------------------------------
class ViewToolbarBrowser : public IViewToolbarBrowser
{
	wxAuiToolBar*		mToolbar;
public:
	ViewToolbarBrowser(wxWindow* parent);
	ViewToolbarBrowser(const std::shared_ptr<IViewWindow>& parent);
	virtual wxWindow* GetWnd()const override
	{
		return mToolbar;
	}

	virtual void SetVisibleFilters(bool enable) override;
	virtual void SetAfterRefreshCls(const std::vector<const IIdent64*>&, const IIdent64*, const wxString&, bool) override;


};
//-----------------------------------------------------------------------------
class ViewPathBrowser : public IViewPathBrowser
{
	wxTextCtrl* mPathCtrl;
public:
	ViewPathBrowser(wxWindow* parent);
	ViewPathBrowser(const std::shared_ptr<IViewWindow>& parent);
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
	

	wxTextCtrl* mCtrlFind;
	void OnCmd_Find(wxCommandEvent& evt = wxCommandEvent());
protected:
	virtual void OnShow()override
	{

	}
public:
	ViewBrowserPage(wxWindow* parent);
	ViewBrowserPage(const std::shared_ptr<IViewWindow>& parent);
	virtual wxWindow* GetWnd()const override
	{
		return mPanel;
	}

	virtual std::shared_ptr<IViewToolbarBrowser>	GetViewToolbarBrowser()const override;
	virtual std::shared_ptr<IViewPathBrowser>		GetViewPathBrowser()const override;
	virtual std::shared_ptr<IViewTableBrowser>		GetViewTableBrowser()const override;
	

	virtual void SetPathMode(const int mode) override;
	virtual void SetPathString(const ICls64& node) override;
	virtual void SetAfterRefreshCls(const std::vector<const IIdent64*>&, const IIdent64*, const wxString&, bool) override;
};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H