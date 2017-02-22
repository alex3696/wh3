#include "_pch.h"
#include "ReportPresenter.h"

using namespace wh;

//---------------------------------------------------------------------------
ReportPresenter::ReportPresenter(std::shared_ptr<IReportView> view
								, std::shared_ptr<ReportModel> model)
	: CtrlWindowBase(view, model)
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


