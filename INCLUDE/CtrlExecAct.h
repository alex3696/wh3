#ifndef __CTRLEXECACT_H
#define __CTRLEXECACT_H

#include "ViewExecAct.h"
#include "ModelExecAct.h"
#include "CtrlBrowser.h"
#include "CtrlActBrowser.h"
#include "CtrlPropPg.h"
#include "CtrlBrowserRO.h"

namespace wh {
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class CtrlActExecWindow final : public CtrlWindowBase<ViewExecActWindow, ModelActExecWindow>
{
	sig::scoped_connection connModel_SelectPage;
	sig::scoped_connection connViewCmd_Unlock;
	sig::scoped_connection connViewCmd_SelectAct;
	sig::scoped_connection connViewCmd_Execute;

	std::shared_ptr<CtrlActBrowser>			mCtrlActBrowser;
	std::shared_ptr<CtrlTableObjBrowser_RO>	mCtrlObjBrowser;
	std::shared_ptr<CtrlPropPg>				mCtrlPropPG;
public:
	CtrlActExecWindow(	const std::shared_ptr<ViewExecActWindow>& view
		, const std::shared_ptr<ModelActExecWindow>& model );
	~CtrlActExecWindow();

	void SetObjects(const std::set<ObjectKey>& obj);
	void SelectAct();
	void Execute();
	void Unlock();
};



//-----------------------------------------------------------------------------



}//namespace wh{
#endif // __****_H