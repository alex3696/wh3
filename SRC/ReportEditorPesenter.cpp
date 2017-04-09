#include "_pch.h"
#include "ReportEditorPresenter.h"

using namespace wh;

//-----------------------------------------------------------------------------
ReportEditorPresenter::ReportEditorPresenter(
	const std::shared_ptr<IReportEditorView>& view
	, const  std::shared_ptr<ReportItemModel>& model)
	:mView(view), mModel(model)
{
	namespace ph = std::placeholders;

	connViewChangeItem = mView->sigChItem
		.connect(std::bind(&ReportEditorPresenter::ChangeItem, this, ph::_1));
}
//-----------------------------------------------------------------------------
void ReportEditorPresenter::ChangeItem(const rec::ReportItem& ri)
{
	mModel->SetValue(std::make_shared<rec::ReportItem>(ri));
}
//-----------------------------------------------------------------------------
void ReportEditorPresenter::ShowView()
{
	auto item = mModel->GetValue();
	if (item)
		mView->SetReportItem(*item);
	else
		mView->SetReportItem(rec::ReportItem());

	mView->Show();
	
}
//-----------------------------------------------------------------------------
void ReportEditorPresenter::CloseView()
{
	mView->Close();

}
//-----------------------------------------------------------------------------
void ReportEditorPresenter::SetRepId(const wxString& rep_id)
{
	mModel->SetRepId(rep_id);
} 