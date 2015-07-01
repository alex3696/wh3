#include "_pch.h"
#include "BaseRefControlPanel.h"


using namespace wh;
using namespace wh::view;

BaseRefControlPanel::BaseRefControlPanel(	wxWindow* parent, 
									wxWindowID id, 
									const wxPoint& pos, 
									const wxSize& size, 
									long style,
									const wxString& name)
	:wxPanel( parent, id, pos, size, style,name) 
{
	wxSizer* szrMain= new wxBoxSizer( wxVERTICAL );
	SetSizer( szrMain );

	mToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,wxAUI_TB_DEFAULT_STYLE );
	szrMain->Add( mToolBar, 0, wxEXPAND, 5 );
	
	mToolBar->AddTool( wxID_REFRESH,	"RELOAD",GetResMgr()->m_ico_refresh24);
	mToolBar->AddSeparator();	
	mToolBar->AddTool( wxID_ADD,		"ADD",GetResMgr()->m_ico_plus24);
	//mToolBar->AddTool( wxID_EDIT,		"EDIT",GetResMgr()->m_ico_edit24);
	mToolBar->AddTool( wxID_REMOVE,		"REMOVE",GetResMgr()->m_ico_minus24);
	mToolBar->Realize(); 
	AppendBitmapMenu(&mContextMenu,wxID_REFRESH,"Обновить",	m_ResMgr->m_ico_refresh16);
	mContextMenu.AppendSeparator();
	AppendBitmapMenu(&mContextMenu,wxID_ADD,	"Добавить",m_ResMgr->m_ico_plus16);
	//AppendBitmapMenu(&mContextMenu,wxID_EDIT,	"Редактировать",m_ResMgr->m_ico_edit16);
	AppendBitmapMenu(&mContextMenu,wxID_REMOVE,	"Удалить",m_ResMgr->m_ico_minus16);

	Bind(wxEVT_COMMAND_MENU_SELECTED,&BaseRefControlPanel::SafeDoLoad,this,wxID_REFRESH);
	Bind(wxEVT_COMMAND_MENU_SELECTED,&BaseRefControlPanel::SafeDoAdd,this,wxID_ADD);
	//Bind(wxEVT_COMMAND_MENU_SELECTED,&BaseRefControlPanel::DoEdit,this,wxID_EDIT);
	Bind(wxEVT_COMMAND_MENU_SELECTED,&BaseRefControlPanel::SafeDoRemove,this,wxID_REMOVE);
	Bind(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU,&BaseRefControlPanel::OnContextMenu,this);
	Bind(wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED,&BaseRefControlPanel::OnSelectChange,this);
	//Bind(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,&BaseRefControlPanel::DoEdit,this);
	
}//TTableViewPanel
//-----------------------------------------------------------------------------

BaseRefControlPanel::~BaseRefControlPanel() 
{
}
//-----------------------------------------------------------------------------
void BaseRefControlPanel::GetSelections(wxDataViewItemArray& array)
{
	auto view = GetTableView();
	if(view)
		view->GetSelections(array);
}
//-----------------------------------------------------------------------------

void BaseRefControlPanel::OnSelectChange ( wxDataViewEvent &event ) 
{
	wxDataViewItem item =  event.GetItem();
	if( item.IsOk() ) {
		//mContextMenu.FindChildItem(wxID_EDIT)->Enable();
		mContextMenu.FindChildItem(wxID_REMOVE)->Enable();//mRemoveMenu->Enable();
		mToolBar->EnableTool(wxID_EDIT,true );
		mToolBar->EnableTool(wxID_REMOVE,true );
		
		auto view = GetTableView();
		if(view)
			view->Select(item);
	} 
	else 
	{
		//mContextMenu.FindChildItem(wxID_EDIT)->Enable(false);
		mContextMenu.FindChildItem(wxID_REMOVE)->Enable(false);//mRemoveMenu->Enable(false);	
		mToolBar->EnableTool(wxID_EDIT,false );
		mToolBar->EnableTool(wxID_REMOVE,false );
		auto view = GetTableView();
		if(view)
			view->UnselectAll();
	}
	mToolBar->Refresh();	
}//void OnSelectChange
//-----------------------------------------------------------------------------

void BaseRefControlPanel::OnContextMenu( wxDataViewEvent &event ) 
{
	OnSelectChange( event );
	PopupMenu( &mContextMenu, event.GetPosition() );		
}//void OnContextMenu( wxDataViewEvent &event )
//-----------------------------------------------------------------------------

void BaseRefControlPanel::SafeDoLoad	 ( wxCommandEvent& evt )
{
	try
	{
		DoLoad(evt);	
	}//try
	catch(boost::exception & e)
	{
		whDataMgr::GetDB().RollBack();
		wxLogWarning(wxString(diagnostic_information(e)) );
	}///catch(boost::exception & e)
	catch(...)
	{
		wxLogWarning(wxString("Unhandled exception") );
	}//catch(...)
}
//-----------------------------------------------------------------------------

void BaseRefControlPanel::SafeDoAdd	 ( wxCommandEvent& evt )
{
	try
	{
		DoAdd(evt);
	}//try
	catch(boost::exception & e)
	{
		whDataMgr::GetDB().RollBack();
		wxLogWarning(wxString(diagnostic_information(e)) );
	}///catch(boost::exception & e)
	catch(...)
	{
		wxLogWarning(wxString("Unhandled exception") );
	}//catch(...)
}
//-----------------------------------------------------------------------------

void BaseRefControlPanel::SafeDoRemove( wxCommandEvent& evt )
{
	try
	{
		DoRemove(evt);
	}//try
	catch(boost::exception & e)
	{
		whDataMgr::GetDB().RollBack();
		wxLogWarning(wxString(diagnostic_information(e)) );
	}///catch(boost::exception & e)
	catch(...)
	{
		wxLogWarning(wxString("Unhandled exception") );
	}//catch(...)
}

















