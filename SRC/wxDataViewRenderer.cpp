#include "_pch.h"
#include "wxDataViewRenderer.h"


// -------------------------------------
// wxDataViewChoiceRenderer
// -------------------------------------
IMPLEMENT_CLASS(wxDataViewChoiceRenderer2, wxDataViewCustomRenderer)

wxDataViewChoiceRenderer2::wxDataViewChoiceRenderer2( const wxArrayString& choices, wxDataViewCellMode mode, int alignment ) 
:wxDataViewCustomRenderer(wxT("string"), mode, alignment )
{
    m_choices = choices;
}

wxControl* wxDataViewChoiceRenderer2::CreateEditorCtrl( wxWindow *parent, wxRect labelRect, const wxVariant &value )
{
	wxPoint pt=labelRect.GetTopLeft();
	wxSize sz=labelRect.GetSize();
	wxSize szfont=parent->GetFont().GetPixelSize();

	if( (sz.y-szfont.y)<8 )
	{
		pt.y -= (szfont.y+8 - sz.y)/2;
		sz.y=szfont.y+8+1;
	}
	wxChoice* c = new wxChoice(parent, wxID_ANY,pt ,sz , m_choices );
    c->SetStringSelection( value.GetString() );
    return c;
}

bool wxDataViewChoiceRenderer2::GetValueFromEditorCtrl( wxControl* editor, wxVariant &value )
{
    wxChoice *c = (wxChoice*) editor;
    wxString s = c->GetStringSelection();
    value = s;
    return true;
}

bool wxDataViewChoiceRenderer2::Render( wxRect rect, wxDC *dc, int state )
{
    RenderText( m_data, 0, rect, dc, state );
    return true;
}

wxSize wxDataViewChoiceRenderer2::GetSize() const
{
    return wxSize(80,16);
}

bool wxDataViewChoiceRenderer2::SetValue( const wxVariant &value )
{
    m_data = value.GetString();
    return true;
}

bool wxDataViewChoiceRenderer2::GetValue( wxVariant &value ) const
{
    value = m_data;
    return false;
}


// -------------------------------------
// wxDataViewComboRenderer
// -------------------------------------
IMPLEMENT_CLASS(wxDataViewComboRenderer, wxDataViewTextRenderer)


wxDataViewComboRenderer::wxDataViewComboRenderer( wxDataViewCellMode mode, int alignment ) 
//:wxDataViewCustomRenderer(wxT("string"), mode, alignment )
:wxDataViewTextRenderer(wxT("string"), mode, alignment),m_Dlg(NULL)
{

}
wxControl* wxDataViewComboRenderer::CreateEditorCtrl( wxWindow *parent, wxRect labelRect, const wxVariant &value )
{
	wxPoint pt=labelRect.GetTopLeft();
	wxSize sz=labelRect.GetSize();
	wxSize szfont=parent->GetFont().GetPixelSize();

	if( (sz.y-szfont.y)<8 )
	{
		pt.y -= (szfont.y+8 - sz.y)/2;
		sz.y=szfont.y+8+1;
	}

	//wxTextCtrl* c = new wxTextCtrl( parent, wxID_ANY, value,pt ,sz,wxTE_PROCESS_ENTER);
	wxTextCtrlWithBtn* c = new wxTextCtrlWithBtn( parent, wxID_ANY, value, pt ,sz,wxTE_PROCESS_ENTER );


	wxPoint  pos=parent->GetScreenPosition () ;

	pos.x+= labelRect.x;
	pos.y+= labelRect.height* wxPtrToUInt(m_item);
	if(m_Dlg)
		m_Dlg->SetPosition(pos);

	c->SetDialogPtr(m_Dlg);

	c->SetInsertionPointEnd();
    c->SelectAll();

    return c;
}
