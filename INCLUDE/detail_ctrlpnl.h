#ifndef __DETAIL_VIEW_CTRLPNL_H
#define __DETAIL_VIEW_CTRLPNL_H

#include "detail_model.h"

namespace wh{
namespace detail{
namespace view{
//-----------------------------------------------------------------------------


class CtrlPnl
	: public wxPanel
	, public ctrlWithResMgr

{
	// hold parent node 
	std::shared_ptr<model::Obj> mObj = std::make_shared<model::Obj>();

	wxAuiManager		mAuiMgr;
	wxAuiToolBar*		mToolBar = nullptr;
	//wxStaticText*		mPathSring = nullptr;
	wxPropertyGrid* mPropGrid;


public:
	CtrlPnl(wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxTAB_TRAVERSAL,
		const wxString& name = wxPanelNameStr);
	~CtrlPnl();
	void SetObject(const wxString& cls_id, const wxString& obj_id, const wxString& obj_pid);

	void UpdatePGColour();
	void UpdateTab();
protected:
	void OnCmdReload(wxCommandEvent& evt);
	
	void OnChangeMainDetail(const IModel* model, const model::Obj::T_Data* data);

	void OnClsPropAfterInsert(const IModel& vec
		, const std::vector<SptrIModel>& newItems
		, const SptrIModel& itemBefore);
	void OnClsPropRemove(const IModel&, const std::vector<unsigned int>&);
	void OnClsPropChange(const IModel&, const std::vector<unsigned int>&);
	void OnObjPropAfterInsert(const IModel& vec
		, const std::vector<SptrIModel>& newItems
		, const SptrIModel& itemBefore);
	void OnObjPropRemove(const IModel&, const std::vector<unsigned int>&);
	void OnObjPropChange(const IModel&, const std::vector<unsigned int>&);


	sig::scoped_connection		mChangeMainDetail;

	sig::scoped_connection		mConnClsPropAppend;
	sig::scoped_connection		mConnClsPropRemove;
	sig::scoped_connection		mConnClsPropChange;
	sig::scoped_connection		mConnObjPropAppend;
	sig::scoped_connection		mConnObjPropRemove;
	sig::scoped_connection		mConnObjPropChange;

};

//-----------------------------------------------------------------------------
}//namespace view {
}//namespace detail {
}//namespace wh{
#endif // __****_H

