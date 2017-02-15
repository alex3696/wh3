#include "_pch.h"
#include "ReportListPresenter.h"
#include "ReportEditorPresenter.h"
#include "ReportPresenter.h"

using namespace wh;

//---------------------------------------------------------------------------
ReportListPresenter::ReportListPresenter(std::shared_ptr<IReportListView> view, std::shared_ptr<ReportListModel> model)
	:mvp::PagePresenter(nullptr)
	, mView(view)
	, mModel(model)
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

}
//---------------------------------------------------------------------------
	
void ReportListPresenter::SetView(mvp::IView* view)
{
	
}
//---------------------------------------------------------------------------
void ReportListPresenter::SetModel(const std::shared_ptr<mvp::IModel>& model)
{
	mvp::PagePresenter::SetModel(model);
}
//---------------------------------------------------------------------------
std::shared_ptr<mvp::IModel> ReportListPresenter::GetModel() 
{
	return mvp::PagePresenter::GetModel();
}
//---------------------------------------------------------------------------
mvp::IView* ReportListPresenter::GetView() 
{ 
	return mView.get(); 
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
	auto p = controller->GetObject<ReportEditorPresenter>("FactoryReportEditorPresenter");
	p->ShowView();
	
	mModel->UpdateList();
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
	peditor->SetItemPosition(pos);
	peditor->ShowView();
	mModel->UpdateList();
}
//---------------------------------------------------------------------------
void ReportListPresenter::OnListUpdated(const rec::ReportList& rl)
{
	if (mView)
		mView->SetReportList(rl);
}