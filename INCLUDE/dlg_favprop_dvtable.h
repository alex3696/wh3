#ifndef __DLG_FAVPROPDVTABLE_H
#define __DLG_FAVPROPDVTABLE_H

#include "BaseTable.h"
#include "dlg_favprop_dvmodel.h"

//-----------------------------------------------------------------------------
namespace wh{
namespace dlg{
namespace favprop {
namespace view {
//-----------------------------------------------------------------------------

class DvTable
	: public wh::view::BaseDataView
{
	
public:
	DvTable(wxWindow*		parent,
		wxWindowID		id = wxID_ANY,
		const wxPoint&	pos = wxDefaultPosition,
		const wxSize &	size = wxDefaultSize,
		long			style = 0 | wxDV_VERT_RULES | wxDV_HORIZ_RULES);

	void SetModel(std::shared_ptr<IModel> model);
protected:
	DvModel*			mDvModel;
	model::FavPropArray mModel;
};

//-----------------------------------------------------------------------------
} //namespace wh{
} //namespace dlg{
} //namespace favprop {
} //namespace view {
#endif // __****_H