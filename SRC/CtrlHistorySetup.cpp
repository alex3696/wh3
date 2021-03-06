#include "_pch.h"
#include "CtrlHistorySetup.h"

using namespace wh;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CtrlHistorySetup::CtrlHistorySetup(const std::shared_ptr<IViewHistorySetup>& view
	, const std::shared_ptr<ModelPageHistory>& model)
	: CtrlWindowBase(view, model)
{
	namespace ph = std::placeholders;
	connViewCmd_SetCfg = mView->sigSetCfg
		.connect(std::bind(&CtrlHistorySetup::SetCfg, this, ph::_1));
	connViewCmd_SetCfgDefault = mView->sigSetCfgDefault
		.connect(std::bind(&CtrlHistorySetup::SetCfgDefault, this, ph::_1));

	connModel_SetCfg = mModel->sigCfgUpdated.connect
		([this](const rec::PageHistory& cfg)
		{
			mView->SetCfg(cfg);
		});

	mView->SetCfg(mModel->GetGuiModel());
}

//---------------------------------------------------------------------------
void CtrlHistorySetup::SetCfg(const rec::PageHistory& cfg)
{
	mModel->SetGuiModel(cfg);
}
//---------------------------------------------------------------------------
void CtrlHistorySetup::SetCfgDefault(const rec::PageHistory& cfg)
{
	mModel->SetGuiCfgDefault(cfg);
}
//---------------------------------------------------------------------------
void CtrlHistorySetup::ShowCfgWindow()
{
	mView->ShowCfgWindow();
}
