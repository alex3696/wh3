#ifndef __CTRL_HISTORY
#define __CTRL_HISTORY

#include "CtrlWindowBase.h"
#include "IViewHistory.h"
#include "ModelHistory.h"
#include "CtrlFilterList.h"
#include "CtrlObjPropList.h"
#include "CtrlHistorySetup.h"

namespace wh{
//-----------------------------------------------------------------------------
class CtrlPageHistory final: public CtrlWindowBase<IViewHistory, ModelPageHistory>
{
	sig::scoped_connection connViewCmd_Update;
	sig::scoped_connection connViewCmd_Forward;
	sig::scoped_connection connViewCmd_Backward;
	sig::scoped_connection connViewCmd_ShowFilterList;
	sig::scoped_connection connViewCmd_ShowObjPropList;
	sig::scoped_connection connViewCmd_ShowCfgWindow;
	sig::scoped_connection connViewCmd_SelectHistoryItem;

	sig::scoped_connection connModel_LoadedHistoryTable;
	sig::scoped_connection connModel_SetRowsOffset;
	sig::scoped_connection connModel_SetRowsLimit;
	sig::scoped_connection connModel_SetCfg;

	std::shared_ptr<CtrlFilterList>		mCtrlFilterList;
	std::shared_ptr<CtrlObjPropList>	mCtrlObjPropList;
	std::shared_ptr<CtrlHistorySetup>	mCtrlHistorySetup;
public:
	CtrlPageHistory(const std::shared_ptr<IViewHistory>& view, const  std::shared_ptr<ModelPageHistory>& model);

	void Update();
	void PageForward();
	void PageBackward();

	void ShowFilterList(bool show = true);
	void ShowObjPropList(bool show = true);
	void SelectHistoryItem(const wxString& str);

};
//-----------------------------------------------------------------------------

} // namespace wh{
#endif // __INOTEBOOKVIEW_H