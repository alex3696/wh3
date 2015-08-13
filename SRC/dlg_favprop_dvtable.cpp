#include "_pch.h"
#include "dlg_favprop_dvtable.h"

using namespace wh;
using namespace dlg::favprop::view;

//---------------------------------------------------------------------------
DvTable::DvTable(wxWindow*		parent,
	wxWindowID		id,
	const wxPoint&	pos,
	const wxSize &	size,
	long			style)
	: BaseDataView(parent, id, pos, size, style)
{
	this->SetRowHeight(26);

	mDvModel = new DvModel();
	wxDataViewCtrl::AssociateModel(mDvModel);
	mDvModel->DecRef();

	AppendToggleColumn("Выбрать", 1, 
		 wxDATAVIEW_CELL_ACTIVATABLE, wxCOL_WIDTH_AUTOSIZE);

	AppendIconTextColumn("Имя", 2, wxDATAVIEW_CELL_INERT, 200,
		wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn("Тип", 3, wxDATAVIEW_CELL_INERT, 100,
		wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn("ID", 4, wxDATAVIEW_CELL_INERT, 50,
		wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);

}

//---------------------------------------------------------------------------
void DvTable::SetModel(std::shared_ptr<IModel> model)
{
	if (model )
	{
		namespace sph = std::placeholders;
		mDvModel->SetModel(model);
	}
}