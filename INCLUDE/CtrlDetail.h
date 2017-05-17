#ifndef __CTRL_DETAIL_H
#define __CTRL_DETAIL_H

#include "CtrlWindowBase.h"
#include "IViewDetail.h"
#include "ModelDetail.h"

namespace wh{
//-----------------------------------------------------------------------------
	class CtrlDetailPage final : public CtrlWindowBase<IViewDetailPage, ModelDetailPage>
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

public:
	CtrlDetailPage(const std::shared_ptr<IViewDetailPage>& view
		, const  std::shared_ptr<ModelDetailPage>& model);

	void Update();
	void PageForward();
	void PageBackward();

	void ShowFilterList(bool show = true);
	void ShowObjPropList(bool show = true);
	void SelectHistoryItem(const wxString& str);

};
//-----------------------------------------------------------------------------

} // namespace wh{
#endif // __*_H