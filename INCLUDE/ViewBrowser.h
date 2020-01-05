#ifndef __VIEW_BROWSER_H
#define __VIEW_BROWSER_H

#include "IViewBrowser.h"
#include "ViewBrowserDVModel.h"
namespace wh{

//-----------------------------------------------------------------------------
class ViewTableBrowser : public IViewTableBrowser
{
	// selection 
	std::set<int64_t>	mClsSelected;
	std::set<ObjectKey>	mObjSelected;
	
	void StoreSelect();
	void RestoreSelect();
	void SetSelected()const;
	void OnCmd_SelectionChanged(wxDataViewEvent& evt);
	virtual void GetSelection(std::vector<const IIdent64*>&)const override;

	// tooltip 
	wxTimer	mToolTipTimer;
	void ShowToolTip();
	void OnCmd_MouseMove(wxMouseEvent& evt);
	void OnCmd_LeftUp(wxMouseEvent& evt);
	// link
	bool mIsCursorHand = false;
	bool IsCursorHand()const;
	void SetCursorHand();
	void SetCursorStandard();

	int64_t							mParentCid = 0;
	
	wxDataViewColumn* mSortCol = nullptr;
	bool mSortAsc = true;
	

	std::set<int64_t> mExpandedCls;

	wxDataViewCtrl*		mTable;
	wxDVTableBrowser*	mDvModel;

	bool mColAutosize = true;
	
	const IIdent64* FindChildCls(const int64_t& id)const;
	const IIdent64* GetTopChildCls()const;
	
	void AutosizeColumns();

	void ResetColumns();
	void RebuildClsColumns(const std::vector<const IIdent64*>&);
	void AppendActColumn(const std::shared_ptr<const FavAProp>& aprop);
	void AppendPropColumn(PropColumns& prop_column,	const std::shared_ptr<const PropVal>& prop_val);
	wxDataViewColumn* AppendTableColumn(const wxString& title, int model_id);
	int GetTitleWidth(const wxString& title)const;
	

protected:
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

	virtual void SetBeforeRefreshCls(const std::vector<const IIdent64*>&
		, const IIdent64*, const wxString&, bool, int mode) override;
	virtual void SetAfterRefreshCls(const std::vector<const IIdent64*>&
		, const IIdent64*, const wxString&, bool, int mode) override;

	virtual void SetObjOperation(Operation, const std::vector<const IIdent64*>&) override;
	virtual void SetShowDetail()override;
	virtual void SetShowFav() override;
	virtual void SetInsertType()const override;
	virtual void SetInsertObj()const override;
	virtual void SetDeleteSelected()const override;
	virtual void SetUpdateSelected()const override;

	
};
//-----------------------------------------------------------------------------
class ViewToolbarBrowser : public IViewToolbarBrowser
{
	wxAuiToolBar*		mToolbar;

	void OnCmd_GroupByType(wxCommandEvent& evt);
public:
	ViewToolbarBrowser(wxWindow* parent);
	ViewToolbarBrowser(const std::shared_ptr<IViewWindow>& parent);
	virtual wxWindow* GetWnd()const override
	{
		return mToolbar;
	}

	virtual void SetVisibleFilters(bool enable) override;
	virtual void SetAfterRefreshCls(const std::vector<const IIdent64*>&
		, const IIdent64*, const wxString&, bool, int mode) override;


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
	virtual void SetPathString(const wxString& str) override;
};
//-----------------------------------------------------------------------------
class ViewBrowserPage : public IViewBrowserPage
{
	wxPanel*		mPanel;
	wxAuiManager*   mAuiMgr;

	std::shared_ptr<IViewToolbarBrowser>	mViewToolbarBrowser;
	std::shared_ptr<IViewPathBrowser>		mViewPathBrowser;
	std::shared_ptr<IViewTableBrowser>		mViewTableBrowser;
	

	wxSearchCtrl* mCtrlFind;

	wxAuiToolBar* mModeToolBar;
	wxAuiToolBarItem* mModeTool;
	void OnCmd_Find(wxCommandEvent& evt = wxCommandEvent());
protected:
	virtual void SetShow()override
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
	

	virtual void SetAfterRefreshCls(const std::vector<const IIdent64*>&
		, const IIdent64*, const wxString&, bool, int mode) override;
};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H