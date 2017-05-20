#ifndef __VIEW_HISTORY_H
#define __VIEW_HISTORY_H
//-----------------------------------------------------------------------------
#include "_pch.h"
#include "IViewHistory.h"
#include "IViewFilterList.h"
#include "IViewObjPropList.h"

namespace wh{
//-----------------------------------------------------------------------------
class ViewTableHistory final: public IViewTableHistory
{
	wxDataViewCtrl* mTable;
public:
	ViewTableHistory(wxWindow* parent);
	ViewTableHistory(std::shared_ptr<IViewWindow> parent)
		:ViewTableHistory(parent->GetWnd())
	{}
	virtual wxWindow* GetWnd()const override
	{
		return mTable;
	}

	virtual void SetHistoryTable(const std::shared_ptr<const ModelHistoryTableData>& rt) override;
	virtual void SetCfg(const rec::PageHistory&) override;
			
protected:
	void OnCmd_Update(wxCommandEvent& evt = wxCommandEvent())
	{
		wxBusyCursor busyCursor;
		wxWindowUpdateLocker lock(mTable);
		sigUpdate();
	}
	void OnCmd_Backward(wxCommandEvent& evt)
	{
		sigPageBackward();
		OnCmd_Update();
	}
	void OnCmd_Forward(wxCommandEvent& evt)
	{
		sigPageForward();
		OnCmd_Update();
	}
	void OnCmd_SelectHistoryItem(wxDataViewEvent& evt);
	void OnCmd_DClickTable(wxMouseEvent& evt);
	
	virtual void OnShow()override
	{

	}
private:

	bool   mColAutosize;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ViewToolbarHistory final : public IViewToolbarHistory
{
	wxAuiToolBar*		mToolbar;
	wxStaticText*		mPageLabel;
	wxAuiToolBarItem*	mToolExportToExcel;

public:
	ViewToolbarHistory(wxWindow* parent);
	ViewToolbarHistory(std::shared_ptr<IViewWindow> parent)
		:ViewToolbarHistory(parent->GetWnd())
	{}
	virtual wxWindow* GetWnd()const override
	{
		return mToolbar;
	}

	virtual void SetHistoryTable(const std::shared_ptr<const ModelHistoryTableData>& rt) override;
	virtual void SetRowsOffset(const size_t& offset) override;
	virtual void SetRowsLimit(const size_t& limit) override;
	virtual void SetCfg(const rec::PageHistory&) override;



	void OnCmd_Update(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_Backward(wxCommandEvent& evt);
	void OnCmd_Forward(wxCommandEvent& evt);
	void OnCmd_Filter(wxCommandEvent& evt);
	void OnCmd_PropList(wxCommandEvent& evt);
	void OnCmd_Convert(wxCommandEvent& evt);
	void OnCmd_ShowSetup(wxCommandEvent& evt);
protected:
	virtual void OnShow()override
	{

	}
private:
	size_t mOffset;
	size_t mLimit;

};


//-----------------------------------------------------------------------------
class ViewHistory final: public IViewHistory
{
	wxPanel*		mPanel;
	wxAuiManager*   mAuiMgr;

	std::shared_ptr<IViewToolbarHistory>	mViewToolbarHistory;
	std::shared_ptr<IViewTableHistory>		mViewTableHistory;
	std::shared_ptr<IViewFilterList>		mViewFilterList;
	std::shared_ptr<IViewObjPropList>		mViewObjPropList;
public:
	ViewHistory(std::shared_ptr<IViewWindow> parent);
	virtual wxWindow* GetWnd()const override
	{
		return mPanel;
	}
	
	virtual std::shared_ptr<IViewToolbarHistory>	GetViewToolbarHistory()const override;
	virtual std::shared_ptr<IViewTableHistory>		GetViewTableHistory()const override;
	virtual std::shared_ptr<IViewFilterList>		GetViewFilterList()const override;
	virtual std::shared_ptr<IViewObjPropList>		GetViewObjPropList()const override;
	virtual void SetCfg(const rec::PageHistory&) override;
	
	void ShowFilterList(bool);
	void ShowObjPropList(bool);

protected:
	virtual void OnShow()override
	{

	}
private:
	rec::PageHistory mCfg;

	
};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H