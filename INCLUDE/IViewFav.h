#ifndef __IVIEW_FAV_H
#define __IVIEW_FAV_H

#include "IViewWindow.h"
#include "ModelBrowserData.h"
#include "IViewFilterList.h"

namespace wh {
//-----------------------------------------------------------------------------
class IViewFav: public IViewWindow
{
public:
	virtual void SetBeforeUpdate(const std::vector<const IIdent64*>&, const IIdent64*) = 0;
	virtual void SetAfterUpdate(const std::vector<const IIdent64*>&, const IIdent64*) = 0;

	sig::signal<void()> sigRefresh;

	sig::signal<void(int64_t, int64_t)> sigAddClsProp;
	sig::signal<void(int64_t, int64_t)> sigAddObjProp;
	sig::signal<void(int64_t, int64_t)> sigAddPrevios;
	sig::signal<void(int64_t, int64_t)> sigAddPeriod;
	sig::signal<void(int64_t, int64_t)> sigAddNext;
	sig::signal<void(int64_t, int64_t)> sigAddLeft;

	sig::signal<void(int64_t, int64_t)> sigRemoveClsProp;
	sig::signal<void(int64_t, int64_t)> sigRemoveObjProp;
	sig::signal<void(int64_t, int64_t)> sigRemovePrevios;
	sig::signal<void(int64_t, int64_t)> sigRemovePeriod;
	sig::signal<void(int64_t, int64_t)> sigRemoveNext;
	sig::signal<void(int64_t, int64_t)> sigRemoveLeft;

	sig::signal<void(const wxString&)>	sigShowHelp;
};






//-----------------------------------------------------------------------------
} //namespace wh{
#endif // __*_H