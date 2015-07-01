#ifndef __DLG_ACT_VIEW_ACTLIST_H
#define __DLG_ACT_VIEW_ACTLIST_H

#include "dlg_act_view_ActListDvModel.h"

//-------------------------------------------------------------------------
namespace wh{
namespace dlg_act {
namespace view {
//-------------------------------------------------------------------------


//---------------------------------------------------------------------------
/// 
class ActList
	: public wxDataViewCtrl
{

public:

	ActList(wxWindow*		parent,
		wxWindowID		id = wxID_ANY,
		const wxPoint&	pos = wxDefaultPosition,
		const wxSize &	size = wxDefaultSize,
		long			style = 0 | wxDV_MULTIPLE);

	void SetModel(std::shared_ptr<model::ActArray> model);
	void GetSelected(std::vector<unsigned int>& vec);


	void EnableAutosizeColumn(unsigned int columnNo);
	void DisableAutosizeColumn(unsigned int columnNo);
	void SetMultiselect(bool enabled = true);

	std::shared_ptr<model::Act> GetSelected()const;
protected:
	std::shared_ptr<model::ActArray > mActArray;
	ActListDvModel* mDvModel = nullptr;

	//sig::scoped_connection mCatalogUpdate;
	
	std::set<unsigned int>	mAutosizeColumn;
	void OnResize(wxSizeEvent& evt);
	
};





//-------------------------------------------------------------------------
} // namespace view {
} // namespace dlg_move {
} // namespace wh{
#endif // __****_H