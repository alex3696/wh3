#ifndef __IVIEW_BROWSER_H
#define __IVIEW_BROWSER_H
//-----------------------------------------------------------------------------

#include "IViewWindow.h"
#include "ModelBrowserData.h"
#include "IViewFilterList.h"


namespace wh{
//-----------------------------------------------------------------------------
class IViewTableBrowser : public IViewWindow
{
public:
	virtual void SetBeforeRefreshCls(const std::vector<const IIdent64*>&, const IIdent64*, const wxString&, bool, int)=0;
	virtual void SetAfterRefreshCls(const std::vector<const IIdent64*>&, const IIdent64*, const wxString&, bool, int) = 0;
	virtual void SetShowDetail() = 0;
	virtual void SetShowFav() = 0;
	virtual void SetInsertType()const = 0;
	virtual void SetInsertObj()const = 0;
	virtual void SetDeleteSelected()const = 0;
	virtual void SetUpdateSelected()const = 0;
	virtual void SetSelectCurrent()const = 0;

	virtual void SetObjOperation(Operation, const std::vector<const IIdent64*>&) = 0;

	sig::signal<void(int64_t)> sigActivate;
	sig::signal<void(int64_t)> sigRefreshClsObjects;
	sig::signal<void(int64_t)> sigGotoCls;
	sig::signal<void(int64_t)> sigGotoObj;
	sig::signal<void(int64_t, bool)> sigSelectCls;
	sig::signal<void(int64_t, int64_t, bool)> sigSelectObj;

	sig::signal<void()> sigRefresh;
	sig::signal<void()> sigUp;
	
	sig::signal<void()> sigMove;
	sig::signal<void()> sigAct;
	sig::signal<void(int64_t, int64_t)> sigShowDetail;

	sig::signal<void(int64_t)> sigClsInsert;
	sig::signal<void(int64_t)> sigClsDelete;
	sig::signal<void(int64_t)> sigClsUpdate;

	sig::signal<void(int64_t)>			sigObjInsert;
	sig::signal<void(int64_t, int64_t)> sigObjDelete;
	sig::signal<void(int64_t, int64_t)> sigObjUpdate;

	sig::signal<void()>					sigToggleGroupByType;
	sig::signal<void(int64_t)>			sigShowFav;
	sig::signal<void()>					sigShowSettings;
	sig::signal<void(const wxString&)>	sigShowHelp;

};
//-----------------------------------------------------------------------------
class IViewToolbarBrowser : public IViewWindow
{
public:
	virtual void SetVisibleFilters(bool enable) = 0;
	virtual void SetAfterRefreshCls(const std::vector<const IIdent64*>&, const IIdent64*, const wxString&, bool, int) = 0;
	
	sig::signal<void()> sigRefresh;
	sig::signal<void()> sigUp;
	
	sig::signal<void()> sigAct;
	sig::signal<void()> sigMove;
	sig::signal<void()> sigShowDetail;

	sig::signal<void()> sigInsertType;
	sig::signal<void()> sigInsertObject;
	sig::signal<void()> sigDelete;
	sig::signal<void()> sigUpdate;
	
	sig::signal<void(bool)>				sigGroupByType;
	sig::signal<void()>					sigToggleGroupByType;
	sig::signal<void()>					sigShowFav;
	sig::signal<void()>					sigShowSettings;
	sig::signal<void(const wxString&)>	sigShowHelp;

};
//-----------------------------------------------------------------------------
class IViewPathBrowser : public IViewWindow
{
public:
	virtual void SetPathMode(const int mode) = 0;
	virtual void SetPathString(const wxString&) = 0;
};
//-----------------------------------------------------------------------------
class IViewBrowserPage : public IViewWindow
{
public:
	virtual std::shared_ptr<IViewPathBrowser>		GetViewPathBrowser()const = 0;
	virtual std::shared_ptr<IViewTableBrowser>		GetViewTableBrowser()const = 0;
	virtual std::shared_ptr<IViewToolbarBrowser>	GetViewToolbarBrowser()const = 0;

	virtual void SetAfterRefreshCls(const std::vector<const IIdent64*>&, const IIdent64*, const wxString&, bool, int) = 0;

	sig::signal<void(const wxString&)>	sigFind;
	sig::signal<void(int)>				sigMode;
	sig::signal<void()>					sigClosePage;
};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H