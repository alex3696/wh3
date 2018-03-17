#include "_pch.h"
#include "whLogin.h"
#include "ConnectionCfgDlg.h"
#include "config.h"

//---------------------------------------------------------------------------
whLogin::whLogin(wxWindow* parent, wxWindowID id, const wxString& title
	, const wxPoint& pos, const wxSize& size, long style) 
	: wxDialog(parent, id, title, pos, size, style /*| wxFRAME_TOOL_WINDOW */)
{
	wxString title_str;
	title_str << "Авторизация " << "wh" << GetAppVersion();
	this->SetTitle(title_str);

	wxBoxSizer* szrMain;
	szrMain = new wxBoxSizer(wxVERTICAL);
	
	wxStaticBitmap* imgLogo;
	imgLogo = new wxStaticBitmap(this, wxID_ANY, wxBitmap(wxT("../../RESOURCES/login_caption.png"), wxBITMAP_TYPE_ANY), wxDefaultPosition, wxDefaultSize, 0);
	szrMain->Add(imgLogo, 0);

	wxFlexGridSizer* szrGrid;
	szrGrid = new wxFlexGridSizer(3, 2, 10, 10);
	szrGrid->AddGrowableCol(1);

	szrGrid->SetFlexibleDirection(wxBOTH);
	szrGrid->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxStaticText* lblName = new wxStaticText(this, wxID_ANY, wxT("Имя пользователя"));
	szrGrid->Add(lblName, 0, wxALL | wxALIGN_RIGHT, 5);

	m_cbxUserName = new wxComboBox(this,wxID_ANY, wxEmptyString);
	szrGrid->Add(m_cbxUserName, 0, wxALL | wxEXPAND, 5);

	wxStaticText* lblPass = new wxStaticText(this, wxID_ANY, wxT("Пароль"));
	szrGrid->Add(lblPass, 0, wxALL | wxALIGN_RIGHT, 5);

	m_txtPass = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
	szrGrid->Add(m_txtPass, 0, wxALL | wxEXPAND, 5);

	szrGrid->Add(0, 0, 0, wxALL, 5);
	m_chkStorePass = new wxCheckBox(this, wxID_ANY, wxT("Сохранить пароль"), wxDefaultPosition, wxDefaultSize, 0);
	szrGrid->Add(m_chkStorePass, 0, wxALL | wxEXPAND, 5);

	szrMain->Add(szrGrid, 6, wxEXPAND, 5);
	

	wxBoxSizer* szrButtons;
	szrButtons = new wxBoxSizer(wxHORIZONTAL);
	m_btnParam = new wxButton(this, wxID_PROPERTIES, wxT("Параметры подключения"), wxDefaultPosition, wxDefaultSize, 0);
	szrButtons->Add(m_btnParam, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	szrButtons->Add(0, 0, 1, wxEXPAND, 5);
	m_btnOK = new wxButton(this, wxID_OK, wxT("Войти"), wxDefaultPosition, wxDefaultSize, 0);
	m_btnOK->SetDefault();
	szrButtons->Add(m_btnOK, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	m_btnCancel = new wxButton(this, wxID_CANCEL, wxT("Отмена"), wxDefaultPosition, wxDefaultSize, 0);
	szrButtons->Add(m_btnCancel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	szrMain->Add(szrButtons, 2, wxEXPAND, 5);

	
	//this->SetAutoLayout(true);
	szrMain->SetSizeHints(this);
	this->SetSizerAndFit(szrMain);
	this->Layout();
	Centre(wxBOTH);


	// Connect Events
	m_btnParam->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(whLogin::OnParam), NULL, this);

	this->Bind(wxEVT_SHOW, &whLogin::OnShow, this);
	m_btnOK->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &whLogin::OnOk, this);

}
//---------------------------------------------------------------------------
whLogin::~whLogin()
{
	m_btnParam->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(whLogin::OnParam), NULL, this);
}
//---------------------------------------------------------------------------

void whLogin::OnShow(wxShowEvent& event)
{
	const auto& conn_cfg = whDataMgr::GetInstance()->mConnectCfg->GetData();
	m_cbxUserName->SetValue(conn_cfg.mUser);
	m_txtPass->SetValue(conn_cfg.mPass);
	m_chkStorePass->SetValue(conn_cfg.mStorePass);
}
//---------------------------------------------------------------------------

void whLogin::OnParam( wxCommandEvent& event )
{
	ConnectionCfgDlg conn_dlg(this);
	conn_dlg.ShowModal();
}
//---------------------------------------------------------------------------

void whLogin::OnOk(wxCommandEvent& evt)
{
	whDataMgr* mgr = whDataMgr::GetInstance();
	auto conn_cfg = whDataMgr::GetInstance()->mConnectCfg->GetData();

	conn_cfg.mUser = m_cbxUserName->GetValue();
	conn_cfg.mPass = m_txtPass->GetValue();
	conn_cfg.mStorePass = m_chkStorePass->GetValue();

	whDataMgr::GetInstance()->mConnectCfg->SetData(conn_cfg);
	whDataMgr::GetInstance()->mConnectCfg->Save();

	mgr->mDb.Open(conn_cfg.mServer
		, conn_cfg.mPort
		, conn_cfg.mDB
		, conn_cfg.mUser
		, conn_cfg.mPass);

	EndModal(wxID_OK);
}
