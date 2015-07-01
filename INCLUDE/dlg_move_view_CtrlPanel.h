#ifndef __DLG_MOVE_VIEW_CTRLPANEL_H
#define __DLG_MOVE_VIEW_CTRLPANEL_H

#include "dlg_move_view_Tree.h"

//-------------------------------------------------------------------------
namespace wh{
namespace dlg_move {
namespace view {
//-------------------------------------------------------------------------


class CtrlPanel
	: public wxPanel
	, public ctrlWithResMgr
	
{
	// hold parent node 
	std::shared_ptr<model::MovableObj > mMovable;

	wxAuiToolBar*		mToolBar = nullptr;
	wxMenu				mContextMenu;
	Tree*				mTree = nullptr;

public:

	CtrlPanel(wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxTAB_TRAVERSAL,
		const wxString& name = wxPanelNameStr);
	~CtrlPanel();

	void OnActivated(wxDataViewEvent &evt);

	void SetModel(std::shared_ptr<IModel>& model);
	void SetModel(std::shared_ptr<model::MovableObj>& model);

	void GetSelected(std::vector<unsigned int>& selected);
	std::shared_ptr<model::DstObj> GetSelected()const;
	
	void BuildLoadItem();

	void BuildCtrlItem(const int winid, const wxString& label,
		const std::function<void()>& itemFunc, 
		const wxBitmap&	ico16, const wxBitmap&	ico24);


};






//-------------------------------------------------------------------------
} // namespace view {
} // namespace dlg_move {
} // namespace wh{
#endif // __****_H