#ifndef __CTRL_OBJPROPLIST_H
#define __CTRL_OBJPROPLIST_H

#include "CtrlWindowBase.h"
#include "IViewObjPropList.h"
#include "ModelObjPropList.h"

namespace wh{
//-----------------------------------------------------------------------------
class CtrlObjPropList : public CtrlWindowBase<IViewObjPropList, ModelObjPropList>
{
	sig::scoped_connection connViewCmd_Update;

	sig::scoped_connection connModel_SetPropList;

public:
	CtrlObjPropList(const std::shared_ptr<IViewObjPropList>& view
		, const  std::shared_ptr<ModelObjPropList>& model);

	void Update();

};
//-----------------------------------------------------------------------------

} // namespace wh{
#endif // __INOTEBOOKVIEW_H