#ifndef __CTRL_BROWSER_H
#define __CTRL_BROWSER_H

#include "CtrlWindowBase.h"
#include "ModelBrowser.h"
#include "CtrlFilterList.h"
#include "IViewBrowser.h"

namespace wh{
//-----------------------------------------------------------------------------
class CtrlTableBrowser final : public CtrlWindowBase<IViewTableBrowser, ModelPageBrowser>
{
	sig::scoped_connection connViewCmd_Refresh;
	sig::scoped_connection connViewCmd_Up;
	sig::scoped_connection connViewCmd_Activate;
	

	sig::scoped_connection connModel_BeforeRefreshCls;
	sig::scoped_connection connModel_AfterRefreshCls;
	
	sig::scoped_connection connModel_ObjOperation;
	
	sig::scoped_connection connModel_GroupByType;
	



public:
	CtrlTableBrowser(const std::shared_ptr<IViewTableBrowser>& view
		, const  std::shared_ptr<ModelPageBrowser>& model);

	void Refresh();
	void Up();
	void Activate(const ICls64*);


};
//-----------------------------------------------------------------------------
class CtrlToolbarBrowser final : public CtrlWindowBase<IViewToolbarBrowser, ModelPageBrowser>
{
	sig::scoped_connection connModel_GroupByType;

	sig::scoped_connection connViewCmd_Refresh;
	sig::scoped_connection connViewCmd_Up;

	sig::scoped_connection connViewCmd_Act;
	sig::scoped_connection connViewCmd_Move;

	sig::scoped_connection connViewCmd_AddType;
	sig::scoped_connection connViewCmd_AddObject;
	sig::scoped_connection connViewCmd_DeleteSelected;
	sig::scoped_connection connViewCmd_UpdateSelected;

	sig::scoped_connection connViewCmd_GroupByType;
	sig::scoped_connection connViewCmd_CollapseGroupByType;

	//sig::scoped_connection connViewCmd_ShowDetail;
	//sig::scoped_connection connViewCmd_ShowFilters;
	//sig::scoped_connection connViewCmd_ShowHistory;
	//sig::scoped_connection connViewCmd_ShowProperties;
public:
	CtrlToolbarBrowser(const std::shared_ptr<IViewToolbarBrowser>& view
		, const  std::shared_ptr<ModelPageBrowser>& model);

	void Refresh();
	void Up();

	void Act();
	void Move();
	
	void AddType();
	void AddObject();
	void DeleteSelected();
	void UpdateSelected();
	
	void GroupByType(bool enable_group_by_type);
	

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
public:
	CtrlPageBrowser(const std::shared_ptr<IViewBrowserPage>& view
		, const  std::shared_ptr<ModelPageBrowser>& model);


	void Find(const wxString&);
};



//-----------------------------------------------------------------------------
} //namespace wh{
#endif // __*_H