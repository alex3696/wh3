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
class CtrlTableHistory final : public CtrlWindowBase<IViewTableHistory, ModelPageHistory>
{
	sig::scoped_connection connViewCmd_Update;
	sig::scoped_connection connViewCmd_Forward;
	sig::scoped_connection connViewCmd_Backward;
	sig::scoped_connection connViewCmd_SelectHistoryItem;

	sig::scoped_connection connModel_LoadedHistoryTable;
	sig::scoped_connection connModel_SetCfg;

public:
	CtrlTableHistory(const std::shared_ptr<IViewTableHistory>& view
		, const  std::shared_ptr<ModelPageHistory>& model);

	void Update();
	void PageForward();
	void PageBackward();
	void SelectHistoryItem(const wxString& str);
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CtrlToolbarHistory final : public CtrlWindowBase<IViewToolbarHistory, ModelPageHistory>
{
	sig::scoped_connection connViewCmd_Update;
	sig::scoped_connection connViewCmd_Forward;
	sig::scoped_connection connViewCmd_Backward;
	sig::scoped_connection connViewCmd_ShowFilterList;
	sig::scoped_connection connViewCmd_ShowObjPropList;
	sig::scoped_connection connViewCmd_ShowCfgWindow;

	sig::scoped_connection connModel_LoadedHistoryTable;
	sig::scoped_connection connModel_SetRowsOffset;
	sig::scoped_connection connModel_SetRowsLimit;
	sig::scoped_connection connModel_SetCfg;

	std::shared_ptr<CtrlHistorySetup>	mCtrlHistorySetup;
public:
	CtrlToolbarHistory(const std::shared_ptr<IViewToolbarHistory>& view
		, const  std::shared_ptr<ModelPageHistory>& model);

	void Update();
	void PageForward();
	void PageBackward();
	void ShowFilterList(bool show = true);
	void ShowObjPropList(bool show = true);
	void ShowSetup();

};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CtrlPageHistory final: public CtrlWindowBase<IViewHistory, ModelPageHistory>
{
	std::shared_ptr<CtrlToolbarHistory>	mCtrlToolbarHistory;
	std::shared_ptr<CtrlTableHistory>	mCtrlTableHistory;
	std::shared_ptr<CtrlFilterList>		mCtrlFilterList;
	std::shared_ptr<CtrlObjPropList>	mCtrlObjPropList;
	
	sig::scoped_connection connModel_SetCfg;
public:
	CtrlPageHistory(const std::shared_ptr<IViewHistory>& view, const  std::shared_ptr<ModelPageHistory>& model);


};
//-----------------------------------------------------------------------------

} // namespace wh{
#endif // __INOTEBOOKVIEW_H