#ifndef __VIEW_HISTORY_H
#define __VIEW_HISTORY_H
//-----------------------------------------------------------------------------
#include "_pch.h"
#include "IViewHistory.h"
#include "IViewFilterList.h"
#include "IViewObjPropList.h"

namespace wh{
//-----------------------------------------------------------------------------
class ViewHistory final: public IViewHistory
{
	wxPanel*		mPanel;
	wxAuiManager*   mAuiMgr;
	wxDataViewCtrl* mTable;
	wxAuiToolBar*	mToolbar;
	wxStaticText*	mPageLabel;

	wxAuiToolBarItem* mToolExportToExcel;
	std::shared_ptr<IViewFilterList> mViewFilterList;
	std::shared_ptr<IViewObjPropList> mViewObjPropList;
public:
	ViewHistory(std::shared_ptr<IViewWindow> parent);
	virtual wxWindow* GetWnd()const override;
	
	virtual void SetHistoryTable(const std::shared_ptr<const ModelHistoryTableData>& rt) override;
	virtual void SetRowsOffset(const size_t& offset) override;
	virtual void SetRowsLimit(const size_t& limit) override;
	virtual std::shared_ptr<IViewFilterList> GetViewFilterList()const override;
	virtual std::shared_ptr<IViewObjPropList> GetViewObjPropList()const override;
	virtual void SetCfg(const rec::PageHistory&) override;
			
protected:
	void ShowFilterList(bool);
	void ShowObjPropList(bool);
	bool IsShowObjPropList()const;

	void OnCmd_Update(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_Backward(wxCommandEvent& evt);
	void OnCmd_Forward(wxCommandEvent& evt);
	void OnCmd_Filter(wxCommandEvent& evt);
	void OnCmd_PropList(wxCommandEvent& evt);
	void OnCmd_SelectHistoryItem(wxDataViewEvent& evt);
	void OnCmd_DClickTable(wxMouseEvent& evt);
	void OnCmd_ShowSetup(wxCommandEvent& evt);
	
	virtual void OnShow()override;
private:
	wxAuiToolBar*	BuildToolBar(wxWindow* parent);
	wxDataViewCtrl* BuildTable(wxWindow* parent);

	size_t mOffset;
	size_t mLimit;
	size_t mStringPerRow;
	bool   mColAutosize;
	
};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H