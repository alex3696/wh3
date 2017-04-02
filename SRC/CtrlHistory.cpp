#include "_pch.h"
#include "CtrlHistory.h"

using namespace wh;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CtrlPageHistory::CtrlPageHistory( std::shared_ptr<IViewHistory> view
								, std::shared_ptr<ModelPageHistory> model)
	: CtrlWindowBase(view, model)
{
	namespace ph = std::placeholders;
	connViewCmd_Update = mView->sigUpdate
		.connect(std::bind(&CtrlPageHistory::Update, this));



	connModel_LoadedHistoryTable = mModel->GetModelHistory().sigAfterLoad.connect
		([this](const std::shared_ptr<const ModelHistoryTableData>& rt)
	{
		mView->SetHistoryTable(rt);
	});


}

//---------------------------------------------------------------------------
void CtrlPageHistory::Update()
{
	mModel->Update();
}

