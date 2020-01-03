#include "_pch.h"
#include "CtrlActBrowser.h"

using namespace wh;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CtrlActBrowser::CtrlActBrowser(
	const std::shared_ptr<ViewActBrowser>& view
	, const  std::shared_ptr<ModelActBrowserWindow>& model)
	:CtrlWindowBase(view, model)
{
	namespace ph = std::placeholders;

	connModel_BeforeRefresh = mModel->sigBeforeRefresh
		.connect(std::bind(&T_View::SetBeforeRefresh, mView.get(), ph::_1));
	connModel_AfterRefresh = mModel->sigAfterRefresh
		.connect(std::bind(&T_View::SetAfterRefresh, mView.get(), ph::_1));

	connViewCmd_Activate = mView->sigActivate
		.connect(std::bind(&CtrlActBrowser::Activate, this, ph::_1));

};
//---------------------------------------------------------------------------
void CtrlActBrowser::Activate(int64_t aid)
{
	mModel->DoActivate(aid);
}