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
	virtual void SetGroupByType(bool enable) = 0;
	virtual void SetBeforePathChange(const ICls64& node) = 0;
	virtual void SetAfterPathChange(const ICls64& node) = 0;
	
	virtual void SetObjOperation(Operation, const std::vector<const IObj64*>&) = 0;


	sig::signal<void()> sigRefresh;
	sig::signal<void()> sigUp;
	sig::signal<void(const ICls64*)> sigActivate;
};
//-----------------------------------------------------------------------------
class IViewToolbarBrowser : public IViewWindow
{
public:
	virtual void SetVisibleFilters(bool enable) = 0;
	virtual void SetGroupByType(bool enable) = 0;

	sig::signal<void()> sigRefresh;
	sig::signal<void()> sigUp;
	
	sig::signal<void()> sigAct;
	sig::signal<void()> sigMove;
	sig::signal<void(const wxString&)> sigFind;

	sig::signal<void()> sigAddType;
	sig::signal<void()> sigAddObject;
	sig::signal<void()> sigDeleteSelected;
	sig::signal<void()> sigUpdateSelected;

	sig::signal<void(bool)>	 sigGroupByType;

	sig::signal<void(bool)>	 sigShowDetail;
	sig::signal<void(bool)>	 sigShowFilters;
	sig::signal<void(bool)>	 sigShowHistory;
	sig::signal<void(bool)>	 sigShowProperties;


};
//-----------------------------------------------------------------------------
class IViewPathBrowser : public IViewWindow
{
public:
	virtual void SetPathMode(const int mode) = 0;
	virtual void SetPathString(const ICls64& node) = 0;
};
//-----------------------------------------------------------------------------
class IViewBrowserPage : public IViewWindow
{
public:
	virtual std::shared_ptr<IViewPathBrowser>		GetViewPathBrowser()const = 0;
	virtual std::shared_ptr<IViewTableBrowser>		GetViewTableBrowser()const = 0;
	virtual std::shared_ptr<IViewToolbarBrowser>	GetViewToolbarBrowser()const = 0;

	virtual void SetPathMode(const int mode) = 0;
	virtual void SetPathString(const ICls64& node) = 0;
};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H