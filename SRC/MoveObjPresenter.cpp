#include "_pch.h"
#include "MoveObjPresenter.h"

using namespace wh;

//-----------------------------------------------------------------------------
CtrlMoveExecWindow::CtrlMoveExecWindow(
	const std::shared_ptr<IMoveObjView>& view
	, const std::shared_ptr<ModelMoveExecWindow>& model)
	: CtrlWindowBase(view, model)
{
	mCtrlObjBrowser = std::make_shared<CtrlTableObjBrowser_RO>
		(view->GetViewObjBrowser(), model->mModelObjBrowser);

	if (!mView)
		return;

	namespace ph = std::placeholders;

	connViewCmd_Unlock = mView->sigUnlock
		.connect(std::bind(&CtrlMoveExecWindow::Unlock, this));
	connViewCmd_Execute = mView->sigExecute
		.connect(std::bind(&CtrlMoveExecWindow::Execute, this));

	connViewCmd_EnableRecent = mView->sigEnableRecent
		.connect(std::bind(&CtrlMoveExecWindow::OnViewEnableRecent, this, ph::_1));
	connViewCmd_FindObj = mView->sigFindObj
		.connect(std::bind(&CtrlMoveExecWindow::OnViewFindObj, this, ph::_1));

}
//-----------------------------------------------------------------------------
CtrlMoveExecWindow::~CtrlMoveExecWindow()
{

}
//-----------------------------------------------------------------------------
void CtrlMoveExecWindow::SetObjects(const std::set<ObjectKey>& obj)
{
	mModel->LockObjects(obj);
	//OnModelUpdate();
}
//-----------------------------------------------------------------------------
void CtrlMoveExecWindow::Execute()
{
	mModel->DoExecute();
}
//---------------------------------------------------------------------------
void CtrlMoveExecWindow::Unlock()
{
	mModel->UnlockObjects();
}
//-----------------------------------------------------------------------------
void CtrlMoveExecWindow::OnViewEnableRecent(bool enable)
{ 
	mModel->SetRecentEnable(enable);
	//OnModelUpdate();
}
//-----------------------------------------------------------------------------
void CtrlMoveExecWindow::OnViewFindObj(const wxString& ss)
{
	mModel->FindObj(ss);
	//OnModelUpdate();
}
