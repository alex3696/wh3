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
	virtual void ViewToModel() = 0;

	sig::signal<void(const std::vector<NotyfyItem>& data)> sigUpdate;
	sig::signal<void()> sigUpdateAll;
	sig::signal<void()> sigApply;

	sig::signal<void(const wxString& title, const wxString& sys_title
		, FilterOp op, FieldType type
		, const std::vector<wxString>& val)> sigUpdateFilter;


};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H