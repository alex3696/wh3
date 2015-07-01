#ifndef __DLG_MOVE_VIEW_TREE_H
#define __DLG_MOVE_VIEW_TREE_H

#include "dlg_move_view_DvModel.h"

//-------------------------------------------------------------------------
namespace wh{
namespace dlg_move {
namespace view {
//-------------------------------------------------------------------------


//---------------------------------------------------------------------------
/// 
class Tree
	: public wxDataViewCtrl
{

public:

	Tree(wxWindow*		parent,
		wxWindowID		id = wxID_ANY,
		const wxPoint&	pos = wxDefaultPosition,
		const wxSize &	size = wxDefaultSize,
		long			style = 0 | wxDV_MULTIPLE);

	//virtual void SetModel(std::shared_ptr<IModel> model)override;
	//virtual void GetSelected(std::vector<unsigned int>& vec)override;
	void SetModel(std::shared_ptr<IModel> model);
	void GetSelected(std::vector<unsigned int>& vec);

	void ExpandAll();
	

	void EnableAutosizeColumn(unsigned int columnNo);
	void DisableAutosizeColumn(unsigned int columnNo);
	void SetMultiselect(bool enabled = true);

	std::shared_ptr<model::DstObj> GetSelected()const;
protected:
	std::shared_ptr<model::MovableObj > mMovable;
	DvModel* mDvModel = nullptr;

	sig::scoped_connection mCatalogUpdate;
	
	std::set<unsigned int>	mAutosizeColumn;
	void OnResize(wxSizeEvent& evt);
	
};





//-------------------------------------------------------------------------
} // namespace view {
} // namespace dlg_move {
} // namespace wh{
#endif // __****_H