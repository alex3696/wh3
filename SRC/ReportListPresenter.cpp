#include "_pch.h"
#include "ReportListPresenter.h"
#include "ReportEditorPresenter.h"
#include "ReportPresenter.h"
#include "globaldata.h"
#include "CtrlNotebook.h"

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

	connModelMk = model->sigMkReport
		.connect(std::bind(&ReportListPresenter::OnMkReport, this, ph::_1));
	connModelRm = model->sigRmReport
		.connect(std::bind(&ReportListPresenter::OnRmReport, this, ph::_1));
	connModelCh = model->sigChReport
		.connect(std::bind(&ReportListPresenter::OnChReport, this, ph::_1, ph::_2));


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
void ReportListPresenter::OnMkReport(const std::shared_ptr<const rec::ReportItem>& ri)
{
	mView->OnMkReport(ri);
}
//---------------------------------------------------------------------------
void ReportListPresenter::OnRmReport(const std::shared_ptr<const rec::ReportItem>& ri)
{
	mView->OnRmReport(ri);
}
//---------------------------------------------------------------------------
void ReportListPresenter::OnChReport(const std::shared_ptr<const rec::ReportItem>& ri, const wxString& old_rep_id)
{
	mView->OnChReport(ri, old_rep_id);
}
//---------------------------------------------------------------------------
void ReportListPresenter::ExecReport(const wxString& rep_id)
{
	auto container = whDataMgr::GetInstance()->mContainer;
	auto nb2 = container->GetObject<CtrlNotebook>("CtrlNotebook");
	if (nb2)
	{
		auto rep_id_s = container->GetObject<wxString>("CurrReportId");
		(*rep_id_s) = rep_id;
		nb2->MkWindow("CtrlPageReport");
	}
		
}
//---------------------------------------------------------------------------
void ReportListPresenter::MkReport()
{
	auto controller = whDataMgr::GetInstance()->mContainer;
	auto peditor = controller->GetObject<ReportEditorPresenter>("FactoryReportEditorPresenter");
	if (peditor)
	{
		peditor->ShowView();
	}
}
//---------------------------------------------------------------------------
void ReportListPresenter::RmReport(const wxString& rep_id)
{
	mModel->Rm(rep_id);
}
//---------------------------------------------------------------------------
void ReportListPresenter::ChReport(const wxString& rep_id)
{
	auto controller = whDataMgr::GetInstance()->mContainer;
	auto peditor = controller->GetObject<ReportEditorPresenter>("FactoryReportEditorPresenter");
	if (peditor)
	{
		peditor->SetRepId(rep_id);
		peditor->ShowView();
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
