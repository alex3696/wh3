#ifndef __DETAIL_VIEW_CTRLPNL_H
#define __DETAIL_VIEW_CTRLPNL_H

#include "MHistory.h"
#include "DetailActCtrl.h"


namespace wh{
namespace detail{
namespace view{
//-----------------------------------------------------------------------------


class CtrlPnl
	: public wxPanel
	, public ctrlWithResMgr

{
	DetailActCtrl			mCtrl;

	//std::shared_ptr<MLogTable> mLogModel;
	VTable*				mLogTable;
	FilterArrayEditor*	mLogTableFilter;
	VTableToolBar*		mLogToolBar;

	VTableToolBar*			mToolBar;
	VTable*					mTableView;
	FilterArrayEditor*		mFilterEditor;
	
	
	std::shared_ptr<model::Obj> mObj;
	ObjDetailPGView*	mObjView;
	DetailActToolBar*	mActToolBar;

	wxAuiManager		mAuiMgr;

public:
	CtrlPnl(wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxTAB_TRAVERSAL,
		const wxString& name = wxPanelNameStr);
	~CtrlPnl();
	
	void SetObject(const rec::ObjInfo& oi);
	//void SetObject(const std::shared_ptr<wh::detail::model::Obj>& model);
	void SetModel(const SptrIModel& model);

	void UpdateTab();

};

//-----------------------------------------------------------------------------
}//namespace view {
}//namespace detail {
}//namespace wh{
#endif // __****_H

