#include "_pch.h"
#include "CtrlDetail.h"

#include "MoveObjPresenter.h"
#include "CtrlExecAct.h"
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
	try
	{
		wxBusyCursor busyCursor;
		auto p0 = GetTickCount();

		wxLogMessage(wxString::Format("MoveObj:\t%d\t start", GetTickCount() - p0));
		p0 = GetTickCount();
		const rec::PathItem& data = mModel->mModelObjDetail->Get();
		auto ctrl = whDataMgr::GetInstance()->mContainer;
		
		wxLogMessage(wxString::Format("MoveObj:\t%d\t init mov_obj", GetTickCount() - p0));
		p0 = GetTickCount();
		std::shared_ptr<rec::PathItem> mov_obj = std::make_shared<rec::PathItem>(data);
		ctrl->RegInstance("MoveableObj", mov_obj);
		//*moveable_sp = data;
		
		wxLogMessage(wxString::Format("MoveObj:\t%d\t get presenter", GetTickCount() - p0));
		p0 = GetTickCount();
		auto presenter = ctrl->GetObject<MoveObjPresenter>("MoveObjPresenter");
		if (!presenter)
			return;

		wxLogMessage(wxString::Format("MoveObj:\t%d\t init", GetTickCount() - p0));
		p0 = GetTickCount();
		presenter->OnViewUpdate();
		wxLogMessage(wxString::Format("MoveObj:\t%d\t TOTAL start time", GetTickCount() - p0));

		presenter->ShowDialog();
	}
	catch (...)
	{
		// Transaction already rollbacked, dialog was destroyed, so nothinh to do
		wxLogError("������ ����� ������ ������������� (��.�����������)");
	}
	mModel->Init();
}
//---------------------------------------------------------------------------
void CtrlToolbarAct::DoAct()
{
	auto container = whDataMgr::GetInstance()->mContainer;
	std::set<ObjectKey> obj;

	const rec::PathItem& data = mModel->mModelObjDetail->Get();

	obj.emplace(ObjectKey(data.mObj.mId, data.mObj.mParent.mId));

	try
	{
		auto ctrlActExecWindow = container->GetObject<CtrlActExecWindow>("CtrlActExecWindow");
		if (ctrlActExecWindow)
		{
			ctrlActExecWindow->SetObjects(obj);
			ctrlActExecWindow->Show();
		}
	}
	catch (...)
	{
		// Transaction already rollbacked, dialog was destroyed, so nothinh to do
		wxLogError("������ ����� ������ ������������� (��.�����������)");
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
		wxLogError("���������� �������� (��.�����������)");
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
