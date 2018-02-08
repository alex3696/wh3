#ifndef __CTRL_FAV_H
#define __CTRL_FAV_H

#include "CtrlWindowBase.h"
#include "ModelFav.h"
#include "IViewFav.h"

namespace wh {
//-----------------------------------------------------------------------------
class CtrlFav final : public CtrlWindowBase<IViewFav, ModelFav>
{
	sig::scoped_connection connViewCmd_Find;
	sig::scoped_connection connModel_AfterRefreshCls;
public:
	CtrlFav(const std::shared_ptr<IViewFav>& view
		, const  std::shared_ptr<ModelFav>& model);

	void EditFav(int64_t cid);
};





//-----------------------------------------------------------------------------
} //namespace wh{
#endif // __*_H