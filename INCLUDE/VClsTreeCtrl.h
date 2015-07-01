#ifndef __TVCLSTREECTRL_H
#define __TVCLSTREECTRL_H
	

#include "MClsTree.h"
#include "TPresenter.h"
#include "TViewCtrlPanel.h"
#include "DClsEditor.h"

namespace wh{
namespace view{


class VClsTreeCtrl
	: public TViewCtrlPanel<CtrlTool::Load 
							| CtrlTool::Append
							| CtrlTool::Remove
							| CtrlTool::Edit
							, DClsEditor
							, true
							>
{

public:
	VClsTreeCtrl(wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxTAB_TRAVERSAL,
		const wxString& name = wxPanelNameStr);

	void OnActivated(wxDataViewEvent &evt);
	void OnBack(wxCommandEvent& event);

	void SetModel(std::shared_ptr<IModel> model);

	// hold paren node 
	std::shared_ptr<MClsNode> mClsNode;

	void BuildGoUpItem();
};







/*

class VClsTreeCtrl
	: public wxPanel
	, public ctrlWithResMgr
{
public:
	VClsTreeCtrl(wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxTAB_TRAVERSAL,
		const wxString& name = wxPanelNameStr);

	void SetModel(std::shared_ptr<MClsNode> model);

	void GetSelected(std::vector<unsigned int>& selected);

protected:
	wxAuiToolBar*		mToolBar = nullptr;
	wxMenu				mContextMenu;
	//VClsTreeTable*		mTableView = nullptr;
	TViewTable*			mTableView = nullptr;
	

	bool				mAutoSaveEdited = true;
	char				mTools = CtrlTool::Load
								| CtrlTool::Append
								| CtrlTool::Remove
								| CtrlTool::Edit;

	std::shared_ptr<MClsNode> mVecModel;

	void OnSelectChange(wxDataViewEvent &event);
	void OnContextMenu(wxDataViewEvent &event);

	void BuildCtrlItem(const int winid, const wxString& label,
		const std::function<void()>& itemFunc,
		const wxBitmap&	ico16, const wxBitmap&	ico24);

	void BuildLoadItem();
	void BuildAddItem();
	void BuildDelItem();
	void BuildEditItem();

	void OnActivated(wxDataViewEvent &evt);
	void OnBack(wxCommandEvent& event);



};
*/



}//namespace view
}//namespace wh
#endif //__*_H