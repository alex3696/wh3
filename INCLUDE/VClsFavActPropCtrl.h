#ifndef __TVCLSFAVACTPROPCTRL_H
#define __TVCLSFAVACTPROPCTRL_H
	

#include "MClsFavoriteActProp.h"
#include "TPresenter.h"
#include "TViewCtrlPanel.h"
#include "DClsFavoriteProp.h"

namespace wh{
namespace view{


class VClsFavActPropCtrl
	: public TViewCtrlPanel<CtrlTool::Load 
							| CtrlTool::Edit
							, DClsFavoriteProp
							, true
							>
{

public:
	VClsFavActPropCtrl(wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxTAB_TRAVERSAL,
		const wxString& name = wxPanelNameStr);

	void OnActivated(wxDataViewEvent &evt);
	void OnBack(wxCommandEvent& event);

	void SetModel(std::shared_ptr<IModel> model);

	// hold paren node 
	std::shared_ptr<MClsFavorite> mClsNode;

	void BuildGoUpItem();
};







}//namespace view
}//namespace wh
#endif //__*_H