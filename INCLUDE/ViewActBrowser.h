#ifndef __VIEWACTBROWSER_H
#define __VIEWACTBROWSER_H

#include "IViewWindow.h"
#include "ModelBrowserData.h"
#include "ViewActBrowserDVModel.h"

namespace wh {
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class ViewActBrowser : public IViewWindow
{
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

	virtual void SetBeforeRefresh();
	virtual void SetAfterRefresh();
	virtual void SetOperation(Operation, const std::vector<const IIdent64*>&);
	virtual void SetInsert()const;
	virtual void SetDelete()const;
	virtual void SetUpdate()const;
	virtual void SetSelectCurrent()const;
	
	sig::signal<void()>		sigRefresh;
	sig::signal<void(int64_t, bool)> sigSelect;
};



}//namespace wh{
#endif // __****_H