#ifndef __CTRL_FAV_H
#define __CTRL_FAV_H

#include "CtrlWindowBase.h"
#include "ModelFav.h"
#include "IViewFav.h"

namespace wh {
//-----------------------------------------------------------------------------
class CtrlFav final : public CtrlWindowBase<IViewFav, ModelFav>
{
	sig::scoped_connection connModel_BeforeRefreshCls;
	sig::scoped_connection connModel_AfterRefreshCls;
	
	sig::scoped_connection connViewCmd_Refresh;

	sig::scoped_connection connViewCmd_AddClsProp;
	sig::scoped_connection connViewCmd_AddObjProp;
	sig::scoped_connection connViewCmd_AddActProp;
	
	sig::scoped_connection connViewCmd_RemoveClsProp;
	sig::scoped_connection connViewCmd_RemoveObjProp;
	sig::scoped_connection connViewCmd_RemoveActProp;
	
	sig::scoped_connection connViewCmd_ShowHelp;

	
public:
	CtrlFav(const std::shared_ptr<IViewFav>& view
		, const  std::shared_ptr<ModelFav>& model);

	void EditFav(int64_t cid);

	void Refresh();
	
	void AddClsProp(int64_t clsId);
	void AddObjProp(int64_t clsId);
	void AddActProp(int64_t clsId, FavAPropInfo info);

	void RemoveClsProp(int64_t clsId, int64_t propId);
	void RemoveObjProp(int64_t clsId, int64_t propId);
	void RemoveActProp(int64_t clsId, int64_t actId, FavAPropInfo);

	void ShowHelp(const wxString& index);

};





//-----------------------------------------------------------------------------
} //namespace wh{
#endif // __*_H