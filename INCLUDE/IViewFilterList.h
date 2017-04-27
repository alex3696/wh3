#ifndef __IVIEW_FILTERLIST_H
#define __IVIEW_FILTERLIST_H
//-----------------------------------------------------------------------------

#include "IViewWindow.h"
#include "ModelFilterList.h"

namespace wh{
//-----------------------------------------------------------------------------
class IViewFilterList : public IViewWindow
{
public:
	using NotyfyItem = ModelFilterList::NotyfyItem;

	virtual void Update(const std::vector<NotyfyItem>& data) = 0;

	sig::signal<void(const std::vector<NotyfyItem>& data)> sigUpdate;
	sig::signal<void()> sigUpdateAll;

};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H