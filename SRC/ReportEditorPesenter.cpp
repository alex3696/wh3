#include "_pch.h"
#include "ReportEditorPresenter.h"

using namespace wh;

//-----------------------------------------------------------------------------
ReportEditorPresenter::ReportEditorPresenter(std::shared_ptr<IReportEditorView> view
											, std::shared_ptr<ReportItemModel> model)
	:mView(view), mModel(model)
{
	namespace ph = std::placeholders;

	connViewChangeItem = mView->sigChItem
		.connect(std::bind(&ReportEditorPresenter::ChangeItem, this, ph::_1));
}
//-----------------------------------------------------------------------------
void ReportEditorPresenter::ChangeItem(const rec::ReportItem& ri)
{
	mModel->SetValue(ri);
}
//-----------------------------------------------------------------------------
void ReportEditorPresenter::ShowView()
{
	const auto& item = (mModel->IsOk()) ? mModel->GetValue() : rec::ReportItem();
	
	mView->SetReportItem(item);
	mView->Show();
	
}
//-----------------------------------------------------------------------------
void ReportEditorPresenter::CloseView()
{
	mView->Close();

}
//-----------------------------------------------------------------------------
void ReportEditorPresenter::SetItemPosition(const size_t pos)
{
	mModel->SetPosition(pos);
}