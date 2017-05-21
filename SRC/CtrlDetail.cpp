#include "_pch.h"
#include "CtrlDetail.h"

#include "MoveObjPresenter.h"
#include "dlg_act_view_Frame.h"
#include "CtrlUndo.h"
#include "ViewUndo.h"

using namespace wh;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CtrlToolbarAct::CtrlToolbarAct(
	const std::shared_ptr<IViewToolbarAct>& view
	, const std::shared_ptr<ModelPageDetail>& model)
	: CtrlWindowBase(view, model)
{
	namespace ph = std::placeholders;

	connViewCmd_DoMove = mView->sigMove
		.connect(std::bind(&CtrlToolbarAct::DoMove, this));
	connViewCmd_DoAct = mView->sigAct
		.connect(std::bind(&CtrlToolbarAct::DoAct, this));
	connViewCmd_DoRollback = mView->sigRedo
		.connect(std::bind(&CtrlToolbarAct::DoRollback, this));

	//connModel_SetObj = mModel->sigObjDetailUpdated
	//	([this](const rec::ObjInfo& rt)
	//{
	//	mView->SetObj(rt);
	//});
	//mView->SetObj(mModel->GetGuiModel());
}

//---------------------------------------------------------------------------
void CtrlToolbarAct::DoMove()
{
	auto p0 = GetTickCount();

	const rec::PathItem& data = mModel->mModelObjDetail->Get();
	auto ctrl = whDataMgr::GetInstance()->mContainer;

	try
	{
		auto moveable_sp = ctrl->GetObject<rec::PathItem>("MoveableObj");
		if (!moveable_sp)
			return;

		*moveable_sp = data;

		auto presenter = ctrl->GetObject<MoveObjPresenter>("MoveObjPresenter");
		if (!presenter)
			return;

		wxLogMessage(wxString::Format("%d \t MoveObj \t init", GetTickCount() - p0));

		auto busyCursor = std::make_unique<wxBusyCursor>();
		presenter->OnViewUpdate();
		busyCursor.reset();
		wxLogMessage(wxString::Format("%d \t MoveObj \t TOTAL start time", GetTickCount() - p0));

		presenter->ShowDialog();
	}
	catch (...)
	{
		// Transaction already rollbacked, dialog was destroyed, so nothinh to do
		wxLogError("Объект занят другим пользователем (см.подробности)");
	}
	mModel->Init();
}
//---------------------------------------------------------------------------
void CtrlToolbarAct::DoAct()
{
	const rec::PathItem& data = mModel->mModelObjDetail->Get();
	try
	{
		auto subj = std::make_shared<dlg_act::model::Obj >();
		subj->SetData(data, true);
		dlg_act::view::Frame dlg;
		dlg.SetModel(subj);
		dlg.ShowModal();
	}
	catch (...)
	{
		// Transaction already rollbacked, dialog was destroyed, so nothinh to do
		wxLogError("Объект занят другим пользователем (см.подробности)");
	}
	mModel->Init();

}
//---------------------------------------------------------------------------
void CtrlToolbarAct::DoRollback()
{
	const rec::PathItem& data = mModel->mModelObjDetail->Get();
	try
	{

		std::shared_ptr<IViewUndoWindow> view 
			= std::make_shared<ViewUndoWindow>(mView->GetWnd());

		std::shared_ptr<ModelUndoWindow> model
			= std::make_shared<ModelUndoWindow>(data);

		auto ctrl_undo = std::make_shared<CtrlUndoWindow>(view, model);

		ctrl_undo->Init();
		ctrl_undo->Show();

	}
	catch (...)
	{
		// Transaction already rollbacked, dialog was destroyed, so nothinh to do
		wxLogError("Невозможно откатить (см.подробности)");
	}
	mModel->Init();

}



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CtrlPageDetail::CtrlPageDetail(const std::shared_ptr<IViewPageDetail>& view
	, const std::shared_ptr<ModelPageDetail>& model)
	: CtrlWindowBase(view, model)
{
	namespace ph = std::placeholders;

	mCtrlToolbarAct
		= std::make_shared<CtrlToolbarAct>(view->GetViewToolbarAct(), model);




	auto& mh = mModel->mModelPageHistory;

	mCtrlToolbarHistory
		= std::make_shared<CtrlToolbarHistory>(view->GetViewToolbarHistory(), mh);

	mCtrlTableHistory
		= std::make_shared<CtrlTableHistory>(view->GetViewTableHistory(), mh);

	mCtrlFilterList = std::make_shared<CtrlFilterList>(
		mView->GetViewFilterList()
		, mh->GetModelHistory().GetFilterList());

	mCtrlObjPropList = std::make_shared<CtrlObjPropList>(
		mView->GetViewObjPropList()
		, mh->GetModelHistory().GetObjPropList());


	connModel_SetCfg = mh->sigCfgUpdated.connect
		(std::bind(&IViewPageDetail::SetCfg, mView.get(), ph::_1));

	mView->SetCfg(mh->GetGuiModel());
}
