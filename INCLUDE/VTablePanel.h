#ifndef __TABLEPANEL_H
#define __TABLEPANEL_H

#include "VTableCtrl.h"
#include "VTableToolBar.h"
#include "FilterArrayEditor.h"

namespace wh{
//-----------------------------------------------------------------------------
class VTablePanel
	: public wxPanel
	, public ctrlWithResMgr
{
public:
	VTablePanel(wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxTAB_TRAVERSAL,
		const wxString& name = wxPanelNameStr);
	~VTablePanel();

	void SetModel(std::shared_ptr<ITable> model);

	bool SetRowHeight(int height) { return mTableView->SetRowHeight(height); }

	void SetEnableSave(bool enable = true) { mToolBar->SetEnableSave(enable); }
	void SetEnableInsert(bool enable = true) { mToolBar->SetEnableInsert(enable); }
	void SetEnableChange(bool enable = true) { mToolBar->SetEnableChange(enable); }
protected:
	wxAuiManager			mAuiMgr;
	VTableToolBar*			mToolBar;
	VTable*					mTableView;
	
	std::shared_ptr<ITable>			mMTable;
	FilterArrayEditor*				mFilterEditor;

	VTableCtrl mCtrl;
	
private:

};
//-----------------------------------------------------------------------------


}//namespace wh
#endif //__*_H