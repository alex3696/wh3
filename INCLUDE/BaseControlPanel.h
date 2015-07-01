#ifndef __BASECONRTOLPANEL_H
#define __BASECONRTOLPANEL_H

#include "BaseTable.h"

namespace wh{
namespace view{


//---------------------------------------------------------------------------
/// Панлька с вьюшкой и контроллером добавить/удалить/редактировать 
class BaseControlPanel
	: public wxPanel, 
	  public ctrlWithResMgr
{
public:
	BaseControlPanel(	wxWindow* parent, 
						wxWindowID id = wxID_ANY, 
						const wxPoint& pos = wxDefaultPosition, 
						const wxSize& size = wxDefaultSize, 
						long style = wxTAB_TRAVERSAL,
						const wxString& name = wxPanelNameStr );
	~BaseControlPanel();

	// view interface
	void			GetSelections(wxDataViewItemArray& array);
	void			SetMultiselect(bool enable = true);
	//wxDataViewItem	GetItem (unsigned int row) const;
	//unsigned int	GetRow (const wxDataViewItem &item) const;

protected:
	virtual BaseTable* GetTableView()=0;

	virtual void DoLoad	 ( wxCommandEvent& evt ) = 0;
	virtual void DoAdd	 ( wxCommandEvent& evt ) = 0;
	virtual void DoEdit  ( wxCommandEvent& evt ) = 0;
	virtual void DoRemove( wxCommandEvent& evt ) = 0;
	virtual void DoActivate( wxCommandEvent& evt );

	void HideEditBtn(bool hide = true);
protected:
	wxAuiToolBar*		mToolBar;
	wxMenu				mContextMenu;

	void				OnSelectChange ( wxDataViewEvent &event );
	void				OnContextMenu( wxDataViewEvent &event );

	void SafeDoLoad	 ( wxCommandEvent& evt );
	void SafeDoAdd	 ( wxCommandEvent& evt );
	void SafeDoEdit  ( wxCommandEvent& evt );
	void SafeDoRemove( wxCommandEvent& evt );
	void SafeDoActivate( wxCommandEvent& evt );


};




}//namespace view
}//namespace wh
#endif //__BASETABLE_H