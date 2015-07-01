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
	wxStaticText*		mPathSring = nullptr;
	
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

	void OnCmdSetTypeDir(wxCommandEvent& evt);
	void OnCmdSetPathDir(wxCommandEvent& evt);
	void OnCmdReload(wxCommandEvent& evt);
	void OnCmdUp(wxCommandEvent& evt);
	void OnCmdMove(wxCommandEvent& evt);
	void OnCmdAct(wxCommandEvent& evt);
	void OnMkObj(wxCommandEvent& evt);
	void OnMkCls(wxCommandEvent& evt);
	void OnEdit(wxCommandEvent& evt);
	void OnDelete(wxCommandEvent& evt);


	void OnSelect(wxDataViewEvent& evt);

	std::shared_ptr<object_catalog::MObjItem> GetSelectedObj()const;

	void OnChangePath(const IModel&, const std::vector<unsigned int>&);
	scoped_connection		mConnPathChange;
public:

	VObjCatalogCtrl(wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxTAB_TRAVERSAL,
		const wxString& name = wxPanelNameStr);
	~VObjCatalogCtrl();

	void OnActivated(wxDataViewEvent &evt);

	void SetModel(std::shared_ptr<IModel> model);
	void GetSelected(std::vector<unsigned int>& selected);

	bool GetSelectedObjKey(rec::PathItem& objKey)const;

};








}//namespace view
}//namespace wh
#endif //__*_H