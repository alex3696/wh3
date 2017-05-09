#include "_pch.h"
#include "CtrlHistory.h"

#include "ViewFilterList.h"


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
	connViewCmd_ShowFilterList = mView->sigShowFilterList
		.connect(std::bind(&CtrlPageHistory::ShowFilterList, this, ph::_1));
	connViewCmd_ShowObjPropList = mView->sigShowObjPropList
		.connect(std::bind(&CtrlPageHistory::ShowObjPropList, this, ph::_1));
	connViewCmd_SelectHistoryItem = mView->sigSelectHistoryItem
		.connect(std::bind(&CtrlPageHistory::SelectHistoryItem, this, ph::_1));


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
void CtrlPageHistory::ShowFilterList(bool show)
{
	if (show)
	{
		if (!mCtrlFilterList)
		{
			auto view_fl = mView->GetViewFilterList();//std::make_shared<IViewFilterList>(mView);
			auto model_fl = mModel->GetModelHistory().GetFilterList();//std::make_shared<ModelFilterList>();
			mCtrlFilterList = std::make_shared<CtrlFilterList>(view_fl, model_fl);
		}
		mCtrlFilterList->UpdateAll();
	}

	mView->ShowFilterList(show);

}
//---------------------------------------------------------------------------
void CtrlPageHistory::ShowObjPropList(bool show)
{
	if (show)
	{
		if (!mCtrlObjPropList)
		{
			auto view = mView->GetViewObjPropList();//std::make_shared<IViewFilterList>(mView);
			auto model = mModel->GetModelHistory().GetObjPropList();//std::make_shared<ModelFilterList>();
			mCtrlObjPropList = std::make_shared<CtrlObjPropList>(view, model);
		}
		mCtrlObjPropList->Update();
	}
	mView->ShowObjPropList(show);

}
//---------------------------------------------------------------------------
void CtrlPageHistory::SelectHistoryItem(const wxString& str)
{
	mModel->GetModelHistory().SelectHistoryItem(str);
}