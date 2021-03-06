#ifndef __VOBJCATALOGCTRL_H
#define __VOBJCATALOGCTRL_H
	

#include "MObjCatalog.h"
#include "TPresenter.h"
#include "TViewCtrlPanel.h"
#include "VObjCatalogTable.h"

namespace wh{
namespace view{



class VObjCatalogCtrl
	: public wxPanel
	, public ctrlWithResMgr
	
{
	// hold parent node 
	std::shared_ptr<wh::object_catalog::MObjCatalog> mCatalogModel;
	
	wxAuiToolBar*		mToolBar = nullptr;
	wxTextCtrl*			mPathSring = nullptr;

	wxComboBtn*			mFindCtrl;
	
	VObjCatalogTable*	mTableView = nullptr;

	wxMenu				mCatalogToolMenu;
	wxMenu				mContextMenu;

	wxAuiToolBarItem* mCatalogTool;
	wxAuiToolBarItem* mReloadTool;
	wxAuiToolBarItem* mUpTool;
	wxAuiToolBarItem* mMoveTool;
	wxAuiToolBarItem* mActTool;

	wxAuiToolBarItem* mMkObjTool;
	wxAuiToolBarItem* mMkClsTool;
	wxAuiToolBarItem* mEditTool;
	wxAuiToolBarItem* mDeleteTool;

	void OnCmdReload(wxCommandEvent& evt);
	void OnCmdUp(wxCommandEvent& evt);
	void OnCmdDetail(wxCommandEvent& evt);
	void OnCmdMove(wxCommandEvent& evt);
	void OnMkObj(wxCommandEvent& evt);
	void OnMkCls(wxCommandEvent& evt);
	void OnEdit(wxCommandEvent& evt);
	void OnDelete(wxCommandEvent& evt);
	
	void OnMiddleUpTable(wxMouseEvent& event);
	void UpdateToolsStates();



	void OnSelect(wxDataViewEvent& evt);
	std::map<int, int> mToolDisable;

	std::shared_ptr<object_catalog::MObjItem> GetSelectedObj()const;

	void OnChangePath(const IModel& vec
		, const std::vector<SptrIModel>& newItems, const SptrIModel& itemBefore);
	sig::scoped_connection		mConnPathChange;
public:

	VObjCatalogCtrl(wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxTAB_TRAVERSAL,
		const wxString& name = wxPanelNameStr);
	~VObjCatalogCtrl();


	void BuildToolbar(bool is_dlg = false);

	void OnActivated(wxDataViewEvent &evt);
	void OnExpanding(wxDataViewEvent &evt);
	

	void SetModel(std::shared_ptr<IModel> model);
	void GetSelected(std::vector<unsigned int>& selected);
	const IModel* GetSelectedItem()const;

	bool GetSelectedObjKey(rec::PathItem& objKey)const; 
	bool GetCurrentParent(rec::PathItem& root)const;

	void HideCatalogSelect(bool hide = false);
	void OnCmdSetTypeDir(wxCommandEvent& evt = wxCommandEvent());
	void OnCmdSetPathDir(wxCommandEvent& evt = wxCommandEvent());

};








}//namespace view
}//namespace wh
#endif //__*_H