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

	sig::scoped_connection connModel_LoadedHistoryTable;
public:
	CtrlPageHistory(std::shared_ptr<IViewHistory> view, std::shared_ptr<ModelPageHistory> model);

	void Update();

};
//-----------------------------------------------------------------------------

} // namespace wh{
#endif // __INOTEBOOKVIEW_H