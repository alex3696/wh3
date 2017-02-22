#include "_pch.h"
#include "ReportListPresenter.h"
#include "ReportEditorPresenter.h"
#include "ReportPresenter.h"
#include "globaldata.h"

using namespace wh;

//---------------------------------------------------------------------------
ReportListPresenter::ReportListPresenter(std::shared_ptr<IReportListView> view, std::shared_ptr<ReportListModel> model)
	: CtrlWindowBase(view, model)
{
	namespace ph = std::placeholders;

	connViewUpdateList = mView->sigUpdateList
		.connect(std::bind(&ReportListPresenter::UpdateList, this));

	connViewExecReport = mView->sigExecReport
		.connect(std::bind(&ReportListPresenter::ExecReport, this, ph::_1));

	connViewMkReport = mView->sigMkReport
		.connect(std::bind(&ReportListPresenter::MkReport, this));
	connViewRmReport = mView->sigRmReport
		.connect(std::bind(&ReportListPresenter::RmReport, this, ph::_1));
	connViewChReport = mView->sigChReport
		.connect(std::bind(&ReportListPresenter::ChReport, this, ph::_1));

	
	connModelUpdate = model->sigListUpdated
		.connect(std::bind(&ReportListPresenter::OnListUpdated, this, ph::_1));

	UpdateList();
}
//---------------------------------------------------------------------------
std::shared_ptr<IViewWindow> ReportListPresenter::GetView()const
{ 
	return mView; 
};
//---------------------------------------------------------------------------
void ReportListPresenter::UpdateList()
{
	mModel->UpdateList();
}
//---------------------------------------------------------------------------
void ReportListPresenter::ExecReport(size_t idx)
{
	auto controller = whDataMgr::GetInstance()->mContainer;
	auto presenter = controller->GetObject<ReportPresenter>("FactoryReportPresenter");

	//rec::PageReport pr;
	//whDataMgr::GetInstance()->mNotebookPresenter->DoAddPage(pr);
}
//---------------------------------------------------------------------------
size_t ReportListPresenter::MkReport()
{
	auto controller = whDataMgr::GetInstance()->mContainer;
	auto peditor = controller->GetObject<ReportEditorPresenter>("FactoryReportEditorPresenter");
	if (peditor)
	{
		peditor->ShowView();
		mModel->UpdateList();
	}
	return 0;
}
//---------------------------------------------------------------------------
void ReportListPresenter::RmReport(size_t idx)
{
	mModel->Rm(idx);
	mModel->UpdateList();
}
//---------------------------------------------------------------------------
void ReportListPresenter::ChReport(size_t pos)
{
	auto controller = whDataMgr::GetInstance()->mContainer;
	auto peditor = controller->GetObject<ReportEditorPresenter>("FactoryReportEditorPresenter");
	if (peditor)
	{
		peditor->SetItemPosition(pos);
		peditor->ShowView();
		mModel->UpdateList();
	}
}
//---------------------------------------------------------------------------
void ReportListPresenter::OnListUpdated(const rec::ReportList& rl)
{
	if (mView)
		mView->SetReportList(rl);
}
//-----------------------------------------------------------------------------
void ReportListPresenter::ConnectView()
{
	if (!mView)
		return;
	
	CtrlWindowBase::ConnectView();

	namespace ph = std::placeholders;

	connViewUpdateList = mView->sigUpdateList
		.connect(std::bind(&ReportListPresenter::UpdateList, this));

	connViewExecReport = mView->sigExecReport
		.connect(std::bind(&ReportListPresenter::ExecReport, this, ph::_1));

	connViewMkReport = mView->sigMkReport
		.connect(std::bind(&ReportListPresenter::MkReport, this));
	connViewRmReport = mView->sigRmReport
		.connect(std::bind(&ReportListPresenter::RmReport, this, ph::_1));
	connViewChReport = mView->sigChReport
		.connect(std::bind(&ReportListPresenter::ChReport, this, ph::_1));

}
//-----------------------------------------------------------------------------
void ReportListPresenter::DisconnectView()
{
	CtrlWindowBase::DisconnectView();

	connViewUpdateList.disconnect();
	connViewExecReport.disconnect();
	connViewMkReport.disconnect();
	connViewRmReport.disconnect();
	connViewChReport.disconnect();

}
//-----------------------------------------------------------------------------
void ReportListPresenter::MkView()
{
	if (!mView)
	{
		mView = whDataMgr::GetInstance()->mContainer->GetObject<IReportListView>("ViewPageReportList");
		ConnectView();
		UpdateList();
	}
}
