#ifndef __WXDATAVIEWRENDERER_H
#define __WXDATAVIEWRENDERER_H

#include "_pch.h"
//#include "ResManager.h" 

//---------------------------------------------------------------------------
class wxDataViewChoiceRenderer2: public wxDataViewCustomRenderer
{
public:
    wxDataViewChoiceRenderer2( const wxArrayString &choices,
                            wxDataViewCellMode mode = wxDATAVIEW_CELL_EDITABLE,
                            int alignment = wxDVR_DEFAULT_ALIGNMENT );
    virtual bool HasEditorCtrl() const { return true; }
    virtual wxControl* CreateEditorCtrl( wxWindow *parent, wxRect labelRect, const wxVariant &value );
    virtual bool GetValueFromEditorCtrl( wxControl* editor, wxVariant &value );
    virtual bool Render( wxRect rect, wxDC *dc, int state );
    virtual wxSize GetSize() const;
    virtual bool SetValue( const wxVariant &value );
    virtual bool GetValue( wxVariant &value ) const;

    wxString GetChoice(size_t index) const { return m_choices[index]; }
    const wxArrayString& GetChoices() const { return m_choices; }

private:
    wxArrayString m_choices;
    wxString      m_data;
protected:
	DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewChoiceRenderer2)
};

//---------------------------------------------------------------------------
class whSelectTextDialog: public wxDialog
{
public:
	whSelectTextDialog (wxWindow *parent, wxWindowID id=wxID_ANY, const wxString &title=wxEmptyString,
		const wxPoint &pos=wxDefaultPosition,const wxSize &size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE, const wxString &name=wxDialogNameStr) 
		:wxDialog(parent,id,title,pos,size,style,name)
	{}

	virtual wxString GetValue()const	{	return wxEmptyString;	}
};







class whListBoxSelectTextDialog: public whSelectTextDialog
{
protected:	
	wxListBox* m_ListBox;
	wxStdDialogButtonSizer* m_sdbSizer;
	wxButton* m_sdbSizerOK;
	wxButton* m_sdbSizerCancel;
public:
	whListBoxSelectTextDialog (wxWindow *parent, wxWindowID id=wxID_ANY, const wxString &title=wxEmptyString,
		const wxPoint &pos=wxDefaultPosition,const wxSize &size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE, const wxString &name=wxDialogNameStr) 
		:whSelectTextDialog(parent,id,title,pos,size,style,name)
	{
		wxBoxSizer* bSizer2 = new wxBoxSizer( wxVERTICAL );
	
		m_ListBox = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
		bSizer2->Add( m_ListBox, 1, wxALL|wxEXPAND);
		
		m_sdbSizer = new wxStdDialogButtonSizer();
		m_sdbSizerOK = new wxButton( this, wxID_OK );
		m_sdbSizer->AddButton( m_sdbSizerOK );
		m_sdbSizerCancel = new wxButton( this, wxID_CANCEL );
		m_sdbSizer->AddButton( m_sdbSizerCancel );
		m_sdbSizer->Realize();
		bSizer2->Add( m_sdbSizer, 0, wxEXPAND, 5 );
		
		this->SetSizer( bSizer2 );
		this->Layout();
		m_ListBox->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( whListBoxSelectTextDialog::OnDclickListBox ), NULL, this );

	
	}

	~whListBoxSelectTextDialog()
	{
		m_ListBox->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( whListBoxSelectTextDialog::OnDclickListBox ), NULL, this );
	}

	virtual void OnDclickListBox( wxCommandEvent& event )
	{ 
		if ( !IsModal() )
		{
			SetReturnCode(wxID_OK);
			this->Show(false);
		}
		else
			EndModal(wxID_OK);
	}

	void AddChoice(const wxString& str)
	{
		m_ListBox->Append( str );
	}

	void ClearChoice()
	{
		m_ListBox->Clear();
	}

	virtual wxString GetValue()const	
	{	
		int row = m_ListBox->GetSelection();
		return m_ListBox->GetString(row);
	}

};
//---------------------------------------------------------------------------
class wxDataViewComboRenderer;

class wxTextCtrlWithBtn : public wxTextCtrl
{
	wxStaticText*				m_lblBtn;
	whSelectTextDialog*			m_Dlg;
public:
    wxTextCtrlWithBtn(wxWindow *parent,
                        wxWindowID id = wxID_ANY,
                        const wxString& value = wxEmptyString,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxValidator& validator = wxDefaultValidator,
                        const wxString& name = wxComboBoxNameStr)
        : wxTextCtrl(parent,id,value,pos,size,style,validator,name),m_Dlg(NULL)//,validator,name)
    {
		m_lblBtn = new wxStaticText( this, wxID_ANY, "...", wxPoint(size.x-(20+4),0),wxSize(20,size.y-4), 0|wxSIMPLE_BORDER|wxALIGN_CENTRE );
		m_lblBtn->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxTextCtrlWithBtn::OnLDown ), NULL, this );

    }

	~wxTextCtrlWithBtn()
	{
		m_lblBtn->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxTextCtrlWithBtn::OnLDown ), NULL, this );
		delete m_lblBtn;
	}


	
	void SetDialogPtr(whSelectTextDialog* ptr){m_Dlg=ptr;}


	void OnLDown( wxMouseEvent& event )
	{ 
		wxEvtHandler* h= this->PopEventHandler();
		if(m_Dlg &&	m_Dlg->ShowModal()==wxID_OK)
		{
			this->SetValue(m_Dlg->GetValue());
		}
		this->PushEventHandler(h);
	}


};


//---------------------------------------------------------------------------
class wxDataViewComboRenderer: public wxDataViewTextRenderer//wxDataViewCustomRenderer
{
public:
	whSelectTextDialog*		m_Dlg;
	
	wxDataViewComboRenderer(wxDataViewCellMode mode = wxDATAVIEW_CELL_EDITABLE,int alignment = wxDVR_DEFAULT_ALIGNMENT );
	
	virtual wxControl* CreateEditorCtrl( wxWindow *parent, wxRect labelRect, const wxVariant &value );

protected:
	DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewComboRenderer)
};

#endif // __WXDATAVIEWRENDERER_H