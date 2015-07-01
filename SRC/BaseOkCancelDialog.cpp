#include "_pch.h"
#include "BaseOkCancelDialog.h"


using namespace wh;
using namespace wh::view;

//---------------------------------------------------------------------------
void DlgBaseOkCancel::Create()
{
	wxSizer* szrMain= new wxBoxSizer( wxVERTICAL );

	
	m_sdbSizer = new wxStdDialogButtonSizer();
	m_btnOK = new wxButton( this, wxID_OK);//,"Сохранить и закрыть" );
	m_sdbSizer->AddButton( m_btnOK );
	m_btnCancel = new wxButton( this, wxID_CANCEL);//," Закрыть" );
	m_sdbSizer->AddButton( m_btnCancel );
	m_sdbSizer->Realize();
	szrMain->Add( m_sdbSizer, 0, wxALL|wxEXPAND, 10 );

	this->SetSizer( szrMain );	
}
//---------------------------------------------------------------------------

DlgBaseOkCancel::DlgBaseOkCancel(std::function<wxWindow*(wxWindow*)>	mainPanelCreate, 
															wxWindow* parent, 
															wxWindowID id ,
															const wxString& title,
															const wxPoint& pos ,
															const wxSize& size,
															long style,
															const wxString& name )
	:wxDialog(parent,id,title,pos,size,style,name)
	, mMainPanel(nullptr)
{
	Create();
	if( !mainPanelCreate )
	{
		mMainPanel = mainPanelCreate(this);
		if (mMainPanel)
			GetSizer()->Insert(0, mMainPanel, 1, wxALL | wxEXPAND, 0);
		Layout();
	}
}
//---------------------------------------------------------------------------
DlgBaseOkCancel::DlgBaseOkCancel(	wxWindow* parent, 
									wxWindowID id ,
									const wxString& title,
									const wxPoint& pos ,
									const wxSize& size,
									long style,
									const wxString& name )
	:wxDialog(parent,id,title,pos,size,style,name)
	, mMainPanel(nullptr)
{
	Create();		
}
//---------------------------------------------------------------------------
void DlgBaseOkCancel::InsertMainWindow( wxWindow* panel )
{
	if (panel)
	{
		mMainPanel = panel;
		GetSizer()->Insert(0, mMainPanel, 1, wxALL | wxEXPAND, 0);
		Layout();
	}
}
//---------------------------------------------------------------------------
wxWindow* DlgBaseOkCancel::GetMainPanel()const
{
	return mMainPanel;
}
