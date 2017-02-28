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

	connViewExecute = mView->sigExecute
		.connect(std::bind(&ReportPresenter::Execute, this, ph::_1));


	connViewExport = mView->sigExport
		.connect(std::bind(&ReportPresenter::Export, this));


	connModelExecuted = mModel->sigExecuted.connect
		([this](const rec::ReportTable& rt)
		{ 
			mView->SetReportTable(rt);
		});

	connModelShowFilterTable = mModel->sigSetFilterTable.connect
		([this](const rec::ReportFilterTable& ft)
	{
		mView->SetFilterTable(ft);
	});
		
	connModelShowNote = mModel->sigSetNote.connect
		([this](const wxString& note)
	{
		mView->SetNote(note);
	});


}
//---------------------------------------------------------------------------
void ReportPresenter::Update()
{
	mModel->Update();
}
//---------------------------------------------------------------------------
void ReportPresenter::Execute(const std::vector<wxString>& filter_vec)
{
	mModel->Execute(filter_vec);
}
//---------------------------------------------------------------------------
void ReportPresenter::Export()
{
	mModel->Export();
}

