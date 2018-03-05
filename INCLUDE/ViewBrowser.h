#ifndef __VIEW_BROWSER_H
#define __VIEW_BROWSER_H

#include "IViewBrowser.h"
namespace wh{
//-----------------------------------------------------------------------------
struct ActColumn
{
	int64_t			mAid;
	FavAPropInfo	mAInfo;
	int				mColumn;
	
	ActColumn() 
		:mAid(0), mAInfo(FavAPropInfo::UnnownProp), mColumn(0)
	{}
	ActColumn(const int64_t& aid, FavAPropInfo acol,int idx)
		:mAid(aid), mAInfo(acol), mColumn(idx)
	{}
};
//-----------------------------------------------------------------------------
using ActColumns = boost::multi_index_container
<
	ActColumn,
	indexed_by
	<
		ordered_unique < 
							composite_key
							<
								ActColumn
								, member<ActColumn, int64_t, &ActColumn::mAid>
								, member<ActColumn, FavAPropInfo, &ActColumn::mAInfo>
							> 
						>
		, ordered_unique<member<ActColumn, int, &ActColumn::mColumn>>
	>
>;
//-----------------------------------------------------------------------------
struct PropColumn
{
	int64_t			mPid;
	int				mColumn;

	PropColumn()
		:mPid(0), mColumn(0)
	{}
	PropColumn(const int64_t& pid,  int idx)
		:mPid(pid), mColumn(idx)
	{}
};
//-----------------------------------------------------------------------------
using PropColumns = boost::multi_index_container
<
	PropColumn,
	indexed_by
	<
		  ordered_unique<member<PropColumn, int64_t, &PropColumn::mPid>>
		, ordered_unique<member<PropColumn, int, &PropColumn::mColumn>>
	>
>;

//-----------------------------------------------------------------------------
class ViewTableBrowser : public IViewTableBrowser
{
	int64_t							mParentCid = 0;
	
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
	void AppendActColumn(const std::shared_ptr<const FavAProp>& aprop);
	void AppendPropColumn(PropColumns& prop_column,	const std::shared_ptr<const PropVal>& prop_val);
	wxDataViewColumn* AppendTableColumn(const wxString& title, int model_id);
	int GetTitleWidth(const wxString& title)const;
protected:
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
	virtual void SetAct() override;
	virtual void SetMove() override;
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
	

	wxSearchCtrl* mCtrlFind;
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
	

	virtual void SetPathMode(const int mode) override;
	virtual void SetPathString(const ICls64& node) override;
	virtual void SetAfterRefreshCls(const std::vector<const IIdent64*>&, const IIdent64*, const wxString&, bool) override;
};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H