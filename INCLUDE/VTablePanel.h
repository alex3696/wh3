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

	VTableCtrl				mCtrl;
protected:
	wxAuiManager			mAuiMgr;
	
	std::shared_ptr<ITable>	mMTable;
	
	

	VTableToolBar*			mToolBar;
	VTable*					mTableView;
	FilterArrayEditor*		mFilterEditor;

	
	
private:

};
//-----------------------------------------------------------------------------


}//namespace wh
#endif //__*_H