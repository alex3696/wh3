#ifndef __CTRL_PROPPG_H
#define __CTRL_PROPPG_H

#include "ModelHistoryData.h"
#include "IModelWindow.h"
#include "ViewPropPg.h"
#include "ModelPropPg.h"

namespace wh {
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CtrlPropPg : public CtrlWindowBase<ViewPropPg, ModelPropPg>
{
	sig::scoped_connection connModel_BeforeRefresh;
	sig::scoped_connection connModel_AfterRefresh;

public:
	CtrlPropPg(const std::shared_ptr<ViewPropPg>& view
		, const  std::shared_ptr<ModelPropPg>& model);

	void Update();

};




} //namespace mvp{
#endif // __IMVP_H