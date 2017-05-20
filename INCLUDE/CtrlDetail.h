#ifndef __CTRL_DETAIL_H
#define __CTRL_DETAIL_H

#include "CtrlHistory.h"
#include "IViewDetail.h"
#include "ModelDetail.h"

namespace wh{
//-----------------------------------------------------------------------------
/*
class CtrlToolbarAct final : public CtrlWindowBase<IViewToolbarHistory, ModelPageHistory>
{
	sig::scoped_connection connViewCmd_DoMove;
	sig::scoped_connection connViewCmd_DoAct;
	sig::scoped_connection connViewCmd_DoRollback;
public:
	CtrlToolbarAct(const std::shared_ptr<IViewToolbarHistory>& view
		, const  std::shared_ptr<ModelPageHistory>& model);

	void DoMove();
	void DoAct();
	void DoRollback();

};
*/
//-----------------------------------------------------------------------------
class CtrlPageDetail final : public CtrlWindowBase<IViewPageDetail, ModelPageDetail>
{
	std::shared_ptr<CtrlToolbarHistory>	mCtrlToolbarHistory;
	std::shared_ptr<CtrlTableHistory>	mCtrlTableHistory;
	std::shared_ptr<CtrlFilterList>		mCtrlFilterList;
	std::shared_ptr<CtrlObjPropList>	mCtrlObjPropList;

	sig::scoped_connection connModel_SetCfg;

public:
	CtrlPageDetail(const std::shared_ptr<IViewPageDetail>& view
		, const  std::shared_ptr<ModelPageDetail>& model);


};
//-----------------------------------------------------------------------------

} // namespace wh{
#endif // __*_H