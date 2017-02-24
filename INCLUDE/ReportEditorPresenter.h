#ifndef __IREPORTEDITORPRESENTER_H
#define __REPORTEDITORPRESENTER_H

#include "IReportEditorView.h"
#include "ReportListModel.h"

namespace wh{
//-----------------------------------------------------------------------------
class ReportEditorPresenter 
{
	sig::scoped_connection connViewChangeItem;
	
	std::shared_ptr<IReportEditorView>	mView;
	std::shared_ptr<ReportItemModel>	mModel;

public:
	ReportEditorPresenter(std::shared_ptr<IReportEditorView> view
		, std::shared_ptr<ReportItemModel> model);


	void ChangeItem(const rec::ReportItem& ri);

	void ShowView();
	void CloseView();

	void SetRepId(const wxString& rep_id);



};
//-----------------------------------------------------------------------------

} // namespace wh{
#endif // __INOTEBOOKVIEW_H