#include "_pch.h"
#include "BaseControlPanel.h"


using namespace wh;
using namespace wh::view;

BaseControlPanel::BaseControlPanel(	wxWindow* parent, 
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
	mToolBar->AddTool( wxID_ADD,		"ADD",GetResMgr()->m_ico_create24);
	mToolBar->AddTool( wxID_EDIT,		"EDIT",GetResMgr()->m_ico_edit24);
	mToolBar->AddTool( wxID_REMOVE,		"REMOVE",GetResMgr()->m_ico_delete24);
	mToolBar->Realize(); 
	AppendBitmapMenu(&mContextMenu,wxID_REFRESH,"Обновить",	m_ResMgr->m_ico_refresh16);
	mContextMenu.AppendSeparator();
	AppendBitmapMenu(&mContextMenu,wxID_ADD,	"Добавить",m_ResMgr->m_ico_create16);
	AppendBitmapMenu(&mContextMenu,wxID_EDIT,	"Редактировать",m_ResMgr->m_ico_edit16);
	AppendBitmapMenu(&mContextMenu,wxID_REMOVE,	"Удалить",m_ResMgr->m_ico_delete16);

	Bind(wxEVT_COMMAND_MENU_SELECTED,&BaseControlPanel::SafeDoLoad,this,wxID_REFRESH);
	Bind(wxEVT_COMMAND_MENU_SELECTED,&BaseControlPanel::SafeDoAdd,this,wxID_ADD);
	Bind(wxEVT_COMMAND_MENU_SELECTED,&BaseControlPanel::SafeDoEdit,this,wxID_EDIT);
	Bind(wxEVT_COMMAND_MENU_SELECTED,&BaseControlPanel::SafeDoRemove,this,wxID_REMOVE);
	Bind(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,&BaseControlPanel::SafeDoActivate,this);

	Bind(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU,&BaseControlPanel::OnContextMenu,this);
	Bind(wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED,&BaseControlPanel::OnSelectChange,this);
	
	
}//TTableViewPanel
//-----------------------------------------------------------------------------

BaseControlPanel::~BaseControlPanel() 
{
}
//-----------------------------------------------------------------------------
void BaseControlPanel::GetSelections(wxDataViewItemArray& array)
{
	auto view = GetTableView();
	if(view)
		view->GetSelections(array);
}
//-----------------------------------------------------------------------------
void BaseControlPanel::SetMultiselect(bool enable)
{
	auto view = GetTableView();
	if(view)
		view->SetMultiselect(enable);
}
//-----------------------------------------------------------------------------
/*
wxDataViewItem	BaseControlPanel::GetItem (unsigned int row) const
{
	BaseTable* view = GetTableView();
	if(view)
		return view->GetItem(row);
}
//-----------------------------------------------------------------------------
unsigned int BaseControlPanel::GetRow (const wxDataViewItem &item) const
{
	auto view = GetTableView();
	if(view)
		return view->GetRow(item);
}
*/
//-----------------------------------------------------------------------------

void BaseControlPanel::OnSelectChange ( wxDataViewEvent &event ) 
{
	wxDataViewItem item =  event.GetItem();
	if( item.IsOk() ) {
		mContextMenu.FindChildItem(wxID_EDIT)->Enable();
		mContextMenu.FindChildItem(wxID_REMOVE)->Enable();//mRemoveMenu->Enable();
		mToolBar->EnableTool(wxID_EDIT,true );
		mToolBar->EnableTool(wxID_REMOVE,true );
		
		auto view = GetTableView();
		if(view)
			view->Select(item);
	} 
	else 
	{
		mContextMenu.FindChildItem(wxID_EDIT)->Enable(false);
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

void BaseControlPanel::OnContextMenu( wxDataViewEvent &event ) 
{
	OnSelectChange( event );
	PopupMenu( &mContextMenu, event.GetPosition() );		
}//void OnContextMenu( wxDataViewEvent &event )
//-----------------------------------------------------------------------------

void BaseControlPanel::SafeDoLoad	 ( wxCommandEvent& evt )
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

void BaseControlPanel::SafeDoAdd	 ( wxCommandEvent& evt )
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

void BaseControlPanel::SafeDoEdit  ( wxCommandEvent& evt )
{
	try
	{
		DoEdit(evt);
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

void BaseControlPanel::SafeDoRemove( wxCommandEvent& evt )
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
//-----------------------------------------------------------------------------

void BaseControlPanel::SafeDoActivate( wxCommandEvent& evt )
{
	try
	{
		DoActivate(evt);
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
void BaseControlPanel::DoActivate( wxCommandEvent& evt )
{
	DoEdit(evt);
}
//-----------------------------------------------------------------------------
void BaseControlPanel::HideEditBtn(bool hide)
{
	if (hide)
		mToolBar->DeleteTool(wxID_EDIT);
	else
	{
		if (0 == mToolBar->GetToolIndex(wxID_EDIT))
			mToolBar->AddTool(wxID_EDIT, "EDIT", GetResMgr()->m_ico_edit24);
	}
	mToolBar->Realize();
}




