#ifndef __MOVEOBJPRESENTER_H
#define __MOVEOBJPRESENTER_H
//-----------------------------------------------------------------------------
#include "_pch.h"
#include "IMoveObjView.h"
#include "MoveObjModel.h"
#include "CtrlBrowserRO.h"

namespace wh{
//-----------------------------------------------------------------------------
class CtrlMoveExecWindow final : public CtrlWindowBase<IMoveObjView, ModelMoveExecWindow>
{
	// Model connector
	sig::scoped_connection connModelUpdate;
	// View connector
	sig::scoped_connection connViewCmd_Unlock;
	sig::scoped_connection connViewCmd_Execute;
	sig::scoped_connection connViewCmd_EnableRecent;
	sig::scoped_connection connViewCmd_FindObj;

	std::shared_ptr<CtrlTableObjBrowser_RO>	mCtrlObjBrowser;
public:
	CtrlMoveExecWindow(const std::shared_ptr<IMoveObjView>& view
		,const std::shared_ptr<ModelMoveExecWindow>& model);
	~CtrlMoveExecWindow();
	
	void SetObjects(const std::set<ObjectKey>& obj);
	void Execute();
	void Unlock();

	void OnViewEnableRecent(bool enable);
	void OnViewFindObj(const wxString& ss);


};//CtrlMoveExecWindow




//-----------------------------------------------------------------------------
}//namespace wh{

#endif // __****_H