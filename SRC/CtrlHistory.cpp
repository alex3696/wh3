#include "_pch.h"
#include "CtrlHistory.h"

using namespace wh;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CtrlPageHistory::CtrlPageHistory(const std::shared_ptr<IViewHistory>& view
	, const std::shared_ptr<ModelPageHistory>& model)
	: CtrlWindowBase(view, model)
{
	namespace ph = std::placeholders;
	connViewCmd_Update = mView->sigUpdate
		.connect(std::bind(&CtrlPageHistory::Update, this));

	connViewCmd_Forward = mView->sigPageForward
		.connect(std::bind(&CtrlPageHistory::PageForward , this));
	connViewCmd_Backward = mView->sigPageBackward
		.connect(std::bind(&CtrlPageHistory::PageBackward, this));
	connViewCmd_Filter = mView->sigFilter
		.connect(std::bind(&CtrlPageHistory::ShowFilter, this));


	connModel_LoadedHistoryTable = mModel->GetModelHistory().sigAfterLoad.connect
		([this](const std::shared_ptr<const ModelHistoryTableData>& rt)
	{
		mView->SetHistoryTable(rt);
	});

	connModel_SetRowsOffset = mModel->GetModelHistory().sigRowsOffset.connect
		([this](const size_t& offset)
	{
		mView->SetRowsOffset(offset);
	});

	connModel_SetRowsLimit = mModel->GetModelHistory().sigRowsLimit.connect
		([this](const size_t& limit)
	{
		mView->SetRowsLimit(limit);
	});

	mModel->GetModelHistory().SetRowsOffset(0);
	mModel->GetModelHistory().SetRowsLimit(50);

}

//---------------------------------------------------------------------------
void CtrlPageHistory::Update()
{
	mModel->Update();
}
//---------------------------------------------------------------------------
void CtrlPageHistory::PageForward()
{
	mModel->PageForward();
}
//---------------------------------------------------------------------------

void CtrlPageHistory::PageBackward()
{
	mModel->PageBackward();
}
//---------------------------------------------------------------------------
void CtrlPageHistory::ShowFilter()
{

}
