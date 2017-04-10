#ifndef __CTRL_HISTORY
#define __CTRL_HISTORY

#include "CtrlWindowBase.h"
#include "IViewHistory.h"
#include "ModelHistory.h"

namespace wh{
//-----------------------------------------------------------------------------
class CtrlPageHistory : public CtrlWindowBase<IViewHistory, ModelPageHistory>
{
	sig::scoped_connection connViewCmd_Update;
	sig::scoped_connection connViewCmd_Forward;
	sig::scoped_connection connViewCmd_Backward;
	sig::scoped_connection connViewCmd_Filter;

	sig::scoped_connection connModel_LoadedHistoryTable;
	sig::scoped_connection connModel_SetRowsOffset;
	sig::scoped_connection connModel_SetRowsLimit;
public:
	CtrlPageHistory(const std::shared_ptr<IViewHistory>& view, const  std::shared_ptr<ModelPageHistory>& model);

	void Update();
	void PageForward();
	void PageBackward();
	void ShowFilter();

};
//-----------------------------------------------------------------------------

} // namespace wh{
#endif // __INOTEBOOKVIEW_H