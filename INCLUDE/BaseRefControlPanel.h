#ifndef __BASEREFCONRTOLPANEL_H
#define __BASEREFCONRTOLPANEL_H

#include "BaseControlPanel.h"

namespace wh{
namespace view{


//---------------------------------------------------------------------------
/// Панлька с вьюшкой и контроллером добавить/удалить 
class BaseRefControlPanel
	: public wxPanel, 
	  public ctrlWithResMgr
{
public:
	BaseRefControlPanel(	wxWindow* parent, 
						wxWindowID id = wxID_ANY, 
						const wxPoint& pos = wxDefaultPosition, 
						const wxSize& size = wxDefaultSize, 
						long style = wxTAB_TRAVERSAL,
						const wxString& name = wxPanelNameStr );
	~BaseRefControlPanel();

	void	GetSelections(wxDataViewItemArray& array);
protected:
	virtual BaseTable* GetTableView()=0;

	virtual void DoLoad	 ( wxCommandEvent& WXUNUSED(evt) ) = 0;
	virtual void DoAdd	 ( wxCommandEvent& WXUNUSED(evt) ) = 0;
	//virtual void DoEdit  ( wxCommandEvent& evt ) = 0;
	virtual void DoRemove( wxCommandEvent& evt ) = 0;

private:
	wxAuiToolBar*		mToolBar;
	wxMenu				mContextMenu;

	void				OnSelectChange ( wxDataViewEvent &event );
	void				OnContextMenu( wxDataViewEvent &event );

	virtual void SafeDoLoad	 ( wxCommandEvent& WXUNUSED(evt) );
	virtual void SafeDoAdd	 ( wxCommandEvent& WXUNUSED(evt) );
	virtual void SafeDoRemove( wxCommandEvent& evt );


};





}//namespace view
}//namespace wh
#endif //__BASEREFCONRTOLPANEL_H