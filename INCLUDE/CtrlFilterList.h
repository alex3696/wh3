#ifndef __CTRL_FILTERLIST_H
#define __CTRL_FILTERLIST_H

#include "CtrlWindowBase.h"
#include "IViewFilterList.h"
#include "ModelFilterList.h"

namespace wh{
//-----------------------------------------------------------------------------
class CtrlFilterList : public CtrlWindowBase<IViewFilterList, ModelFilterList>
{
	sig::scoped_connection connViewCmd_Update;
	sig::scoped_connection connViewCmd_UpdateAll;
	sig::scoped_connection connViewCmd_Apply;
	sig::scoped_connection connViewCmd_ViewToModel;

	sig::scoped_connection connModel_Update;
	
public:
	using NotyfyItem = ModelFilterList::NotyfyItem;

	CtrlFilterList(const std::shared_ptr<IViewFilterList>& view
		, const  std::shared_ptr<ModelFilterList>& model);

	void Update(const std::vector<NotyfyItem>& data);
	void UpdateAll();

	void UpdateFilter(const wxString& title, const wxString& sys_title
		, FilterOp op, FieldType type
		, const std::vector<wxString>& val);
	void Apply();

};
//-----------------------------------------------------------------------------

} // namespace wh{
#endif // __***_H