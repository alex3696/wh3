#ifndef __IVIEW_OBJPROPERLIST_H
#define __IVIEW_OBJPROPERLIST_H
//-----------------------------------------------------------------------------

#include "IViewWindow.h"
#include "ModelHistoryData.h"
#include "IViewFilterList.h"

namespace wh{
//-----------------------------------------------------------------------------
class IViewObjPropList : public IViewWindow
{
public:
	virtual void SetPropList(const PropValTable& rt, const IAct* ) = 0;
	
	sig::signal<void()> sigUpdate;
};

//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H