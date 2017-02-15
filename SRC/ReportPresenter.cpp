#include "_pch.h"
#include "ReportPresenter.h"

using namespace wh;

//---------------------------------------------------------------------------
ReportPresenter::ReportPresenter(std::shared_ptr<IReportView> view
								, std::shared_ptr<ReportModel> model)
	:mvp::PagePresenter(nullptr)
	, mView(view)
	, mModel(model)

{
	namespace ph = std::placeholders;

	connViewUpdate = mView->sigUpdate
		.connect(std::bind(&ReportPresenter::Update, this));

	connViewExport = mView->sigExport
		.connect(std::bind(&ReportPresenter::Export, this));

	connViewSetParam = mView->sigSetParam
		.connect(std::bind(&ReportPresenter::SetParam, this));

}
//---------------------------------------------------------------------------
void ReportPresenter::SetView(mvp::IView* view)
{

}
//---------------------------------------------------------------------------
void ReportPresenter::SetModel(const std::shared_ptr<mvp::IModel>& model)
{
	mvp::PagePresenter::SetModel(model);
}
//---------------------------------------------------------------------------
std::shared_ptr<mvp::IModel> ReportPresenter::GetModel()
{
	return mvp::PagePresenter::GetModel();
}
//---------------------------------------------------------------------------
mvp::IView* ReportPresenter::GetView()
{
	return mView.get();
};
//---------------------------------------------------------------------------
void ReportPresenter::Update()
{
	mModel->Update();
}
//---------------------------------------------------------------------------
void ReportPresenter::Export()
{

}
//---------------------------------------------------------------------------
void ReportPresenter::SetParam()
{

}


