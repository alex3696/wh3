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

	connModel_SelectPage = mModel->sigSelectPage
		.connect(std::bind(&T_View::SetSelectPage, mView.get(), ph::_1));

	connModel_EnableRecent = mModel->sigEnableRecent
		.connect(std::bind(&T_View::EnableRecent , mView.get(), ph::_1));
	connModel_UpdateRecent = mModel->sigUpdateRecent
		.connect(std::bind(&T_View::UpdateRecent, mView.get(), ph::_1));
	connModel_UpdateDst = mModel->sigUpdateDst
		.connect(std::bind(&T_View::UpdateDst, mView.get(), ph::_1));
	connModel_GetSelect = mModel->sigGetSelection
		.connect(std::bind(&T_View::GetSelection, mView.get(), ph::_1));

	connViewCmd_Unlock = mView->sigUnlock
		.connect(std::bind(&CtrlMoveExecWindow::Unlock, this));
	connViewCmd_Execute = mView->sigExecute
		.connect(std::bind(&CtrlMoveExecWindow::Execute, this));

	connViewCmd_EnableRecent = mView->sigEnableRecent
		.connect(std::bind(&CtrlMoveExecWindow::OnViewEnableRecent, this, ph::_1));

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
