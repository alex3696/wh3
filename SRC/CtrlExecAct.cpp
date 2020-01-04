#include "_pch.h"
#include "CtrlExecAct.h"

using namespace wh;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CtrlActExecWindow::CtrlActExecWindow(
	const std::shared_ptr<ViewExecActWindow>& view
	, const std::shared_ptr<ModelActExecWindow>& model)
	: CtrlWindowBase(view, model)
{
	mCtrlObjBrowser = std::make_shared<CtrlTableObjBrowser_RO>
		(view->GetViewObjBrowser(), model->mModelObjBrowser );
	mCtrlActBrowser = std::make_shared<CtrlActBrowser>
		(view->GetViewActBrowser(), model->mModelActBrowser);
	
	namespace ph = std::placeholders;
	connViewCmd_Unlock = mView->sigUnlock
		.connect(std::bind(&CtrlActExecWindow::Unlock, this));

};
//---------------------------------------------------------------------------
CtrlActExecWindow::~CtrlActExecWindow()
{
}
//---------------------------------------------------------------------------
void CtrlActExecWindow::SetObjects(const std::set<ObjectKey>& obj)
{
	mModel->LockObjects(obj);
}
//---------------------------------------------------------------------------
void wh::CtrlActExecWindow::Unlock()
{
	mModel->UnlockObjects();
}
