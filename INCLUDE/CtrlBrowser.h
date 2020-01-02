#ifndef __CTRL_BROWSER_H
#define __CTRL_BROWSER_H

#include "CtrlWindowBase.h"
#include "ModelBrowser.h"
#include "CtrlFilterList.h"
#include "IViewBrowser.h"

namespace wh{

class CtrlPageBrowser;
//-----------------------------------------------------------------------------
class CtrlTableBrowser final : public CtrlWindowBase<IViewTableBrowser, ModelBrowser>
{
	sig::scoped_connection connModel_SelectCurrent;
	sig::scoped_connection connModel_BeforeRefreshCls;
	sig::scoped_connection connModel_AfterRefreshCls;
	sig::scoped_connection connModel_ObjOperation;

	sig::scoped_connection connViewCmd_Activate;
	sig::scoped_connection connViewCmd_RefreshClsObjects;
	sig::scoped_connection connViewCmd_GotoCls;
	sig::scoped_connection connViewCmd_GotoObj;
	sig::scoped_connection connViewCmd_SelectCls;
	sig::scoped_connection connViewCmd_SelectObj;

	sig::scoped_connection connViewCmd_Refresh;
	sig::scoped_connection connViewCmd_Up;

	sig::scoped_connection connViewCmd_Act;
	sig::scoped_connection connViewCmd_Move;
	sig::scoped_connection connViewCmd_ShowObjDetail;

	sig::scoped_connection connViewCmd_ClsInsert;
	sig::scoped_connection connViewCmd_ClsDelete;
	sig::scoped_connection connViewCmd_ClsUpdate;
	
	sig::scoped_connection connViewCmd_ObjInsert;
	sig::scoped_connection connViewCmd_ObjDelete;
	sig::scoped_connection connViewCmd_ObjUpdate;

	sig::scoped_connection connViewCmd_ToggleGroupByType;
	sig::scoped_connection connViewCmd_ShowFav;
	sig::scoped_connection connViewCmd_ShowSettings;
	sig::scoped_connection connViewCmd_ShowHelp;

	

public:
	CtrlTableBrowser(const std::shared_ptr<IViewTableBrowser>& view
		, const  std::shared_ptr<ModelBrowser>& model);

	void Refresh();
	void Up();
	void Activate(int64_t cid);
	void RefreshClsObjects(int64_t cid);
	void GotoCls(int64_t cid);
	void GotoObj(int64_t oid);
	void SelectCls(int64_t cid, bool select);
	void SelectObj(int64_t oid, int64_t opid, bool select);
	void SetObjects(const std::set<ObjectKey>& obj);

	void Act();
	void Move();
	
	void SetShowDetail();// ask view about selected item 
	void ShowDetail(int64_t oid, int64_t parent_oid);// do action
	
	// ask view about selected item 
	void SetInsertType();
	void SetInsertObj();
	void SetDelete();
	void SetUpdate();
	// do action
	void ClsInsert(int64_t parent_cid);
	void ClsDelete(int64_t cid);
	void ClsUpdate(int64_t cid);

	void ObjInsert(int64_t cid);
	void ObjDelete(int64_t oid, int64_t parent_oid);
	void ObjUpdate(int64_t oid, int64_t parent_oid);

	void ToggleGroupByType();
	void SetShowFav();// ask view about selected item 
	void ShowFav(int64_t cid);// do action
	void ShowSettings();
	void ShowHelp(const wxString& index);

	
};
//-----------------------------------------------------------------------------
class CtrlToolbarBrowser final : public CtrlWindowBase<IViewToolbarBrowser, ModelPageBrowser>
{
	sig::scoped_connection connModel_AfterRefreshCls;

	sig::scoped_connection connViewCmd_Refresh;
	sig::scoped_connection connViewCmd_Up;

	sig::scoped_connection connViewCmd_Act;
	sig::scoped_connection connViewCmd_Move;
	sig::scoped_connection connViewCmd_ShowDetail;

	sig::scoped_connection connViewCmd_AddType;
	sig::scoped_connection connViewCmd_AddObject;
	sig::scoped_connection connViewCmd_DeleteSelected;
	sig::scoped_connection connViewCmd_UpdateSelected;

	sig::scoped_connection connViewCmd_GroupByType;
	sig::scoped_connection connViewCmd_ToggleGroupByType;
	sig::scoped_connection connViewCmd_ShowFav;
	sig::scoped_connection connViewCmd_ShowSettings;
	sig::scoped_connection connViewCmd_ShowHelp;

	CtrlTableBrowser* mTableCtrl;
public:
	CtrlToolbarBrowser(const std::shared_ptr<IViewToolbarBrowser>& view
		, const  std::shared_ptr<ModelPageBrowser>& model
		, CtrlTableBrowser* table_ctrl);

	void Refresh();
	void Up();

	void Act();
	void Move();
	void ShowDetail();
	
	void InsertType();
	void InsertObject();
	void Delete();
	void Update();
	
	void GroupByType(bool enable_group_by_type);
	void ToggleGroupByType();
	void ShowFav();
	void ShowSettings();
	void ShowHelp(const wxString& index);
	

};
//-----------------------------------------------------------------------------
class CtrlPathBrowser final : public CtrlWindowBase<IViewPathBrowser, ModelPageBrowser>
{
	sig::scoped_connection connModel_PathChanged;
	sig::scoped_connection connModel_ModeChanged;
public:
	CtrlPathBrowser(const std::shared_ptr<IViewPathBrowser>& view
		, const  std::shared_ptr<ModelPageBrowser>& model);

};
//-----------------------------------------------------------------------------
class CtrlPageBrowser final : public CtrlWindowBase<IViewBrowserPage, ModelPageBrowser>
{
	
	std::shared_ptr<CtrlToolbarBrowser>	mCtrlToolbarBrowser;
	std::shared_ptr<CtrlPathBrowser>	mCtrlPathBrowser;
	std::shared_ptr<CtrlTableBrowser>	mCtrlTableBrowser;

	sig::scoped_connection connViewCmd_Find;
	sig::scoped_connection connViewCmd_Mode;
	
	sig::scoped_connection connModel_AfterRefreshCls;

	sig::scoped_connection connViewCmd_Close;
public:
	CtrlPageBrowser(const std::shared_ptr<IViewBrowserPage>& view
		, const  std::shared_ptr<ModelPageBrowser>& model);


	void Find(const wxString&);
	void SetMode(int);
	void ClosePage();
};



//-----------------------------------------------------------------------------
} //namespace wh{
#endif // __*_H