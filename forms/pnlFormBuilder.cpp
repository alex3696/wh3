///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "pnlFormBuilder.h"

///////////////////////////////////////////////////////////////////////////

pnlSlotCommon::pnlSlotCommon( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 400,300 ) );
	
	wxFlexGridSizer* szrMain;
	szrMain = new wxFlexGridSizer( 3, 2, 0, 0 );
	szrMain->AddGrowableCol( 1 );
	szrMain->SetFlexibleDirection( wxBOTH );
	szrMain->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );
	
	wxStaticText* lblSlotClass;
	lblSlotClass = new wxStaticText( this, wxID_ANY, wxT("Класс слота(контейнера)"), wxDefaultPosition, wxDefaultSize, 0 );
	lblSlotClass->Wrap( -1 );
	szrMain->Add( lblSlotClass, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_txtSlotClass = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	bSizer4->Add( m_txtSlotClass, 1, wxALL|wxEXPAND, 5 );
	
	m_btnSlotClass = new wxButton( this, wxID_ANY, wxT("..."), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	bSizer4->Add( m_btnSlotClass, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	szrMain->Add( bSizer4, 1, wxEXPAND|wxALIGN_CENTER_VERTICAL, 0 );
	
	lblMaxQty = new wxStaticText( this, wxID_ANY, wxT("Максимум"), wxDefaultPosition, wxDefaultSize, 0 );
	lblMaxQty->Wrap( -1 );
	szrMain->Add( lblMaxQty, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_txtMaxQty = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	szrMain->Add( m_txtMaxQty, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	lblMinQty = new wxStaticText( this, wxID_ANY, wxT("Минимум"), wxDefaultPosition, wxDefaultSize, 0 );
	lblMinQty->Wrap( -1 );
	szrMain->Add( lblMinQty, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_txtMinQty = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	szrMain->Add( m_txtMinQty, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	this->SetSizer( szrMain );
	this->Layout();
}

pnlSlotCommon::~pnlSlotCommon()
{
}

pnlSlotPerm::pnlSlotPerm( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* szrMain;
	szrMain = new wxFlexGridSizer( 5, 2, 0, 0 );
	szrMain->AddGrowableCol( 1 );
	szrMain->AddGrowableRow( 2 );
	szrMain->AddGrowableRow( 4 );
	szrMain->SetFlexibleDirection( wxBOTH );
	szrMain->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );
	
	wxStaticText* lblRolname;
	lblRolname = new wxStaticText( this, wxID_ANY, wxT("Пользователям группы "), wxDefaultPosition, wxDefaultSize, 0 );
	lblRolname->Wrap( -1 );
	szrMain->Add( lblRolname, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* bSizer41;
	bSizer41 = new wxBoxSizer( wxHORIZONTAL );
	
	m_txtRolname1 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	bSizer41->Add( m_txtRolname1, 1, wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_btnRolname1 = new wxButton( this, wxID_ANY, wxT("..."), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	bSizer41->Add( m_btnRolname1, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	szrMain->Add( bSizer41, 1, wxEXPAND, 5 );
	
	m_staticText7 = new wxStaticText( this, wxID_ANY, wxT("разрешено вкладывать объект"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	szrMain->Add( m_staticText7, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* bSizer411;
	bSizer411 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText181 = new wxStaticText( this, wxID_ANY, wxT("тип"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText181->Wrap( -1 );
	bSizer411->Add( m_staticText181, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_textCtrl10 = new wxTextCtrl( this, wxID_ANY, wxT("ФЭУ"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	bSizer411->Add( m_textCtrl10, 1, wxEXPAND|wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_staticText10 = new wxStaticText( this, wxID_ANY, wxT("№"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	bSizer411->Add( m_staticText10, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_txtRolname11 = new wxTextCtrl( this, wxID_ANY, wxT("002"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer411->Add( m_txtRolname11, 1, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	
	m_btnRolname11 = new wxButton( this, wxID_ANY, wxT("..."), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	bSizer411->Add( m_btnRolname11, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	szrMain->Add( bSizer411, 1, wxEXPAND, 5 );
	
	m_staticText16 = new wxStaticText( this, wxID_ANY, wxT("Находящийся в "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	szrMain->Add( m_staticText16, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listCtrl5 = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	bSizer18->Add( m_listCtrl5, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer71;
	bSizer71 = new wxBoxSizer( wxVERTICAL );
	
	m_bpButton1 = new wxBitmapButton( this, wxID_ANY, wxBitmap( wxT("../RESOURCES/newsheet.ico"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer71->Add( m_bpButton1, 0, wxALL, 5 );
	
	m_bpButton2 = new wxBitmapButton( this, wxID_ANY, wxBitmap( wxT("../RESOURCES/delete.ico"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer71->Add( m_bpButton2, 0, wxALL, 5 );
	
	bSizer18->Add( bSizer71, 0, wxEXPAND, 5 );
	
	szrMain->Add( bSizer18, 1, wxEXPAND, 5 );
	
	m_staticText12 = new wxStaticText( this, wxID_ANY, wxT("в объект"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	szrMain->Add( m_staticText12, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* bSizer4111;
	bSizer4111 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText18 = new wxStaticText( this, wxID_ANY, wxT("тип"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18->Wrap( -1 );
	bSizer4111->Add( m_staticText18, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_textCtrl101 = new wxTextCtrl( this, wxID_ANY, wxT("СРК2М"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	bSizer4111->Add( m_textCtrl101, 1, wxEXPAND|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_staticText101 = new wxStaticText( this, wxID_ANY, wxT("№"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText101->Wrap( -1 );
	bSizer4111->Add( m_staticText101, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_txtRolname111 = new wxTextCtrl( this, wxID_ANY, wxT("98"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4111->Add( m_txtRolname111, 1, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	
	m_btnRolname111 = new wxButton( this, wxID_ANY, wxT("..."), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	bSizer4111->Add( m_btnRolname111, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	szrMain->Add( bSizer4111, 1, wxEXPAND, 5 );
	
	m_staticText17 = new wxStaticText( this, wxID_ANY, wxT("который накодится в"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	szrMain->Add( m_staticText17, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* bSizer181;
	bSizer181 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listCtrl51 = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	bSizer181->Add( m_listCtrl51, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer711;
	bSizer711 = new wxBoxSizer( wxVERTICAL );
	
	m_bpButton11 = new wxBitmapButton( this, wxID_ANY, wxBitmap( wxT("../RESOURCES/newsheet.ico"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer711->Add( m_bpButton11, 0, wxALL, 5 );
	
	m_bpButton21 = new wxBitmapButton( this, wxID_ANY, wxBitmap( wxT("../RESOURCES/delete.ico"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer711->Add( m_bpButton21, 0, wxALL, 5 );
	
	bSizer181->Add( bSizer711, 0, wxEXPAND, 5 );
	
	szrMain->Add( bSizer181, 1, wxEXPAND, 5 );
	
	bSizer7->Add( szrMain, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer7 );
	this->Layout();
}

pnlSlotPerm::~pnlSlotPerm()
{
}
