#include "_pch.h"
#include "CtrlHistory.h"

#include "ViewFilterList.h"
#include "ViewHistorySetup.h"


using namespace wh;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CtrlTableHistory::CtrlTableHistory(
	  const std::shared_ptr<IViewTableHistory>& view
	, const std::shared_ptr<ModelPageHistory>& model)
	: CtrlWindowBase(view, model)
{
	namespace ph = std::placeholders;
	connViewCmd_Update = mView->sigUpdate
		.connect(std::bind(&CtrlTableHistory::Update, this));
	connViewCmd_Forward = mView->sigPageForward
		.connect(std::bind(&CtrlTableHistory::PageForward, this));
	connViewCmd_Backward = mView->sigPageBackward
		.connect(std::bind(&CtrlTableHistory::PageBackward, this));
	connViewCmd_SelectHistoryItem = mView->sigSelectHistoryItem
		.connect(std::bind(&CtrlTableHistory::SelectHistoryItem, this, ph::_1));

	connModel_LoadedHistoryTable = mModel->GetModelHistory().sigAfterLoad.connect
		([this](const std::shared_ptr<const ModelHistoryTableData>& rt)
	{
		mView->SetHistoryTable(rt);
	});
	connModel_SetCfg = mModel->sigCfgUpdated.connect
		(std::bind(&IViewTableHistory::SetCfg, mView.get(), ph::_1));

	mView->SetCfg(mModel->GetGuiModel());
}
//---------------------------------------------------------------------------
void CtrlTableHistory::Update()
{
	mModel->Update();
}
//---------------------------------------------------------------------------
void CtrlTableHistory::PageForward()
{
	mModel->PageForward();
}
//---------------------------------------------------------------------------

void CtrlTableHistory::PageBackward()
{
	mModel->PageBackward();
}
//---------------------------------------------------------------------------
void CtrlTableHistory::SelectHistoryItem(const wxString& str)
{
	mModel->GetModelHistory().SelectHistoryItem(str);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CtrlToolbarHistory::CtrlToolbarHistory(
	  const std::shared_ptr<IViewToolbarHistory>& view
	, const std::shared_ptr<ModelPageHistory>& model)
	: CtrlWindowBase(view, model)
{
	namespace ph = std::placeholders;

	if (!mCtrlHistorySetup)
	{
		auto view = std::make_shared<ViewHistorySetup>(mView);
		mCtrlHistorySetup = std::make_shared<CtrlHistorySetup>(view, mModel);
	}


	connViewCmd_Update = mView->sigUpdate
		.connect(std::bind(&CtrlToolbarHistory::Update, this));
	connViewCmd_Forward = mView->sigPageForward
		.connect(std::bind(&CtrlToolbarHistory::PageForward, this));
	connViewCmd_Backward = mView->sigPageBackward
		.connect(std::bind(&CtrlToolbarHistory::PageBackward, this));

	connViewCmd_ShowFilterList = mView->sigShowFilterList
		.connect(std::bind(&CtrlToolbarHistory::ShowFilterList, this, ph::_1));
	connViewCmd_ShowObjPropList = mView->sigShowObjPropList
		.connect(std::bind(&CtrlToolbarHistory::ShowObjPropList, this, ph::_1));
	connViewCmd_ShowCfgWindow = mView->sigShowCfgWindow
		.connect(std::bind(&CtrlToolbarHistory::ShowSetup, this));

	connViewCmd_ConvertToExcel = mView->sigConvertToExcel
		.connect(std::bind(&CtrlToolbarHistory::ConvertToExcel, this));
	
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

	connModel_SetCfg = mModel->sigCfgUpdated.connect
		//(std::bind(&IViewHistory::SetCfg, mView.get(), ph::_1));
		([this](const rec::PageHistory& cfg)
	{
		mView->SetCfg(cfg);
	});

	//mModel->GetModelHistory().SetRowsOffset(0);
	//mModel->GetModelHistory().SetRowsLimit(50);

	mView->SetCfg(mModel->GetGuiModel());
}

//---------------------------------------------------------------------------
void CtrlToolbarHistory::Update()
{
	mModel->Update();
}
//---------------------------------------------------------------------------
void CtrlToolbarHistory::PageForward()
{
	mModel->PageForward();
}
//---------------------------------------------------------------------------
void CtrlToolbarHistory::PageBackward()
{
	mModel->PageBackward();
}
//---------------------------------------------------------------------------
void CtrlToolbarHistory::ShowFilterList(bool show)
{
	auto cfg = mModel->GetGuiModel();
	cfg.mShowFilterList = show;
	mModel->SetGuiModel(std::move(cfg));
}
//---------------------------------------------------------------------------
void CtrlToolbarHistory::ShowObjPropList(bool show)
{
	auto cfg = mModel->GetGuiModel();
	cfg.mShowPropertyList = show;
	mModel->SetGuiModel(std::move(cfg));
}
//---------------------------------------------------------------------------
void CtrlToolbarHistory::ShowSetup()
{
	mCtrlHistorySetup->ShowCfgWindow();
}
//---------------------------------------------------------------------------
void wh::CtrlToolbarHistory::ConvertToExcel()
{
	mModel->ConvertToExcel();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CtrlPageHistory::CtrlPageHistory(const std::shared_ptr<IViewHistory>& view
	, const std::shared_ptr<ModelPageHistory>& model)
	: CtrlWindowBase(view, model)
{
	namespace ph = std::placeholders;

	mCtrlToolbarHistory
		= std::make_shared<CtrlToolbarHistory>(view->GetViewToolbarHistory(), model);

	mCtrlTableHistory 
		= std::make_shared<CtrlTableHistory>(view->GetViewTableHistory(), model);

	mCtrlFilterList = std::make_shared<CtrlFilterList>(
		mView->GetViewFilterList()
		, mModel->GetModelHistory().GetFilterList() );

	mCtrlObjPropList = std::make_shared<CtrlObjPropList>(
		mView->GetViewObjPropList()
		, mModel->GetModelHistory().GetObjPropList() );


	connModel_SetCfg = mModel->sigCfgUpdated.connect
		(std::bind(&IViewHistory::SetCfg, mView.get(), ph::_1));

	mView->SetCfg(mModel->GetGuiModel());
}
