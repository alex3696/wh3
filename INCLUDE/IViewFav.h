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
	virtual void SetUpdate(const std::vector<const IIdent64*>&, const IIdent64*, bool) = 0;

	sig::signal<void()> sigDoUpdate;
};






//-----------------------------------------------------------------------------
} //namespace wh{
#endif // __*_H