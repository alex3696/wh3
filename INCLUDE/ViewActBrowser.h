#ifndef __VIEWACTBROWSER_H
#define __VIEWACTBROWSER_H

#include "IViewWindow.h"
#include "ModelActTable.h"
#include "ViewActBrowserDVModel.h"

namespace wh {
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class ViewActBrowser : public IViewWindow
{
	// tooltip 
	wxTimer	mToolTipTimer;
	void ShowToolTip();
	void OnCmd_MouseMove(wxMouseEvent& evt);

	wxDataViewColumn* mSortCol = nullptr;
	bool mSortAsc = true;
	wxDataViewCtrl*			mTable;
	wxDVTableActBrowser*	mDvModel;
	bool mColAutosize = true;
	
	void StoreSelect();
	void RestoreSelect();
	void AutosizeColumns();

	void ResetColumns();
	void RebuildColumns();
	wxDataViewColumn* AppendTableColumn(const wxString& title, int model_id);
	int GetTitleWidth(const wxString& title)const;
	
	bool IsSelectedItem(const wxDataViewItem& item)const;
	void SetSelected(const wxDataViewItem& item, bool select)const;
	void SetSelected()const;
protected:
	void OnCmd_Activate(wxDataViewEvent& evt);
	void OnCmd_SelectionChanged(wxDataViewEvent& evt);
public:
	ViewActBrowser(wxWindow* parent);
	ViewActBrowser(const std::shared_ptr<IViewWindow>& parent);
	virtual wxWindow* GetWnd()const override
	{
		return mTable;
	}

	void SetBeforeRefresh(std::shared_ptr<const ModelActTable> );
	void SetAfterRefresh(std::shared_ptr<const ModelActTable> );
	void SetSelectCurrent()const;

	sig::signal<void(int64_t)>	sigActivate;

	
	sig::signal<void()>		sigRefresh;
	sig::signal<void(int64_t, bool)> sigSelect;
};



}//namespace wh{
#endif // __****_H