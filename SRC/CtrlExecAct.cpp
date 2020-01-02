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
	
};
//---------------------------------------------------------------------------
void CtrlActExecWindow::SetObjects(const std::set<ObjectKey>& obj)
{
	mCtrlObjBrowser->SetObjects(obj);
}