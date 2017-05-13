#ifndef __CTRL_HISTORY_SETUP_H
#define __CTRL_HISTORY_SETUP_H

#include "CtrlWindowBase.h"
#include "IViewHistorySetup.h"
#include "ModelHistory.h"

namespace wh{
//-----------------------------------------------------------------------------
class CtrlHistorySetup final : public CtrlWindowBase<IViewHistorySetup, ModelPageHistory>
{
	sig::scoped_connection connViewCmd_SetCfg;

	sig::scoped_connection connModel_SetCfg;
public:
	CtrlHistorySetup(const std::shared_ptr<IViewHistorySetup>& view
		, const  std::shared_ptr<ModelPageHistory>& model);

	void SetCfg(const rec::PageHistory&);

};
//-----------------------------------------------------------------------------

} // namespace wh{
#endif // __*_H