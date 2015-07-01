#ifndef __EVIEWTEMPLATES_H
#define __EVIEWTEMPLATES_H

#include "BaseOkCancelDialog.h"

namespace wh{



//---------------------------------------------------------------------------
template < typename t_EditorPanel > 
class TEditorDlg
	: public DlgBaseOkCancel
	, public EMultiView
{
public:
	TEditorDlg(	wxWindow* parent, 
				wxWindowID id = wxID_ANY,
				const wxString& title=wxEmptyString,
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxSize( 400,300 ),//wxDefaultSize,
				long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER,
				const wxString& name = wxDialogNameStr)
	:DlgBaseOkCancel(parent,id,title,pos,wxSize(600,400),style,name)
	{
		SetTitle("Выбор свойства");
		mEditorPanel = new t_EditorPanel(this);
		GetSizer()->Insert(0, mEditorPanel, 1, wxALL|wxEXPAND, 0 );
		this->Layout(  );

		EMultiView::AppendView (mEditorPanel);
	}
protected:
	t_EditorPanel*	mEditorPanel;
};//TEditorDlg












}//namespace wh
#endif // __ACTREFPROPEDITORPANEL_H