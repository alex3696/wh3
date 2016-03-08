#include "_pch.h"
#include "whLogin.h"

//---------------------------------------------------------------------------
whLogin::whLogin(wxWindow* parent, wxWindowID id, const wxString& title
	, const wxPoint& pos, const wxSize& size, long style) 
	: wxDialog(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* szrMain;
	szrMain = new wxBoxSizer(wxVERTICAL);

	wxStaticBitmap* imgLogo;
	imgLogo = new wxStaticBitmap(this, wxID_ANY, wxBitmap(wxT("../../RESOURCES/login_caption.png"), wxBITMAP_TYPE_ANY), wxDefaultPosition, wxDefaultSize, 0);
	szrMain->Add(imgLogo, 0, 0, 5);

	wxStaticText* lblInfo;
	lblInfo = new wxStaticText(this, wxID_ANY, wxT("Подключение к базе данных"), wxDefaultPosition, wxDefaultSize, 0);
	lblInfo->Wrap(-1);
	szrMain->Add(lblInfo, 0, wxALL | wxALIGN_CENTER_VERTICAL, 10);

	wxFlexGridSizer* szrGrid;
	szrGrid = new wxFlexGridSizer(4, 2, 0, 50);
	szrGrid->AddGrowableCol(1);
	szrGrid->AddGrowableRow(0);
	szrGrid->AddGrowableRow(3);
	szrGrid->SetFlexibleDirection(wxBOTH);
	szrGrid->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxStaticText* lblName;
	lblName = new wxStaticText(this, wxID_ANY, wxT("Имя пользователя"), wxDefaultPosition, wxDefaultSize, 0);
	lblName->Wrap(-1);
	szrGrid->Add(lblName, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 5);

	m_cbxUserName = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
	szrGrid->Add(m_cbxUserName, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 5);

	wxStaticText* lblPass;
	lblPass = new wxStaticText(this, wxID_ANY, wxT("Пароль"), wxDefaultPosition, wxDefaultSize, 0);
	lblPass->Wrap(-1);
	szrGrid->Add(lblPass, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 5);

	m_txtPass = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
	szrGrid->Add(m_txtPass, 0, wxEXPAND | wxALL | wxALIGN_CENTER_VERTICAL, 5);


	szrGrid->Add(0, 0, 0, 0, 5);

	m_chkStorePass = new wxCheckBox(this, wxID_ANY, wxT("Сохранить пароль"), wxDefaultPosition, wxDefaultSize, 0);

	szrGrid->Add(m_chkStorePass, 0, wxALL | wxEXPAND, 5);

	szrGrid->Add(0, 0, 0, 0, 5);


	mPnlExt = new wxPanel(this, wxID_ANY);
	{
		wxFlexGridSizer* szrExtGrid;
		szrExtGrid = new wxFlexGridSizer(4, 2, 0, 50);
		szrExtGrid->AddGrowableCol(1);
		szrExtGrid->AddGrowableRow(0);
		szrExtGrid->AddGrowableRow(3);
		szrExtGrid->SetFlexibleDirection(wxBOTH);
		szrExtGrid->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

		wxStaticText* lblRole = new wxStaticText(mPnlExt, wxID_ANY, "Роль");
		szrExtGrid->Add(lblRole, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 5);
		mTxtRole = new wxTextCtrl(mPnlExt, wxID_ANY);
		szrExtGrid->Add(mTxtRole, 0, wxEXPAND | wxALL | wxALIGN_CENTER_VERTICAL, 5);

		wxStaticText* lblServer = new wxStaticText(mPnlExt, wxID_ANY, "Сервер");
		szrExtGrid->Add(lblServer, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 5);
		mTxtServer = new wxTextCtrl(mPnlExt, wxID_ANY);
		szrExtGrid->Add(mTxtServer, 0, wxEXPAND | wxALL | wxALIGN_CENTER_VERTICAL, 5);

		wxStaticText* lblPort = new wxStaticText(mPnlExt, wxID_ANY, "Порт");
		szrExtGrid->Add(lblPort, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 5);
		mTxtPort = new wxTextCtrl(mPnlExt, wxID_ANY);
		szrExtGrid->Add(mTxtPort, 0, wxEXPAND | wxALL | wxALIGN_CENTER_VERTICAL, 5);

		wxStaticText* lblDb = new wxStaticText(mPnlExt, wxID_ANY, "БД");
		szrExtGrid->Add(lblDb, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 5);
		mTxtDB = new wxTextCtrl(mPnlExt, wxID_ANY);
		szrExtGrid->Add(mTxtDB, 0, wxEXPAND | wxALL | wxALIGN_CENTER_VERTICAL, 5);

		mPnlExt->SetSizer(szrExtGrid);
	}
	mPnlExt->Hide();


	szrMain->Add(szrGrid, 0, wxEXPAND, 5);
	szrMain->Add(mPnlExt, 0, wxEXPAND, 5);

	wxBoxSizer* szrButtons;
	szrButtons = new wxBoxSizer(wxHORIZONTAL);

	m_btnParam = new wxButton(this, wxID_PROPERTIES, wxT("Параметры подключения"), wxDefaultPosition, wxDefaultSize, 0);
	szrButtons->Add(m_btnParam, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);


	szrButtons->Add(50, 0, 1, wxEXPAND | wxALIGN_CENTER_VERTICAL, 5);

	m_btnOK = new wxButton(this, wxID_OK, wxT("Войти"), wxDefaultPosition, wxDefaultSize, 0);
	m_btnOK->SetDefault();
	szrButtons->Add(m_btnOK, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	m_btnCancel = new wxButton(this, wxID_CANCEL, wxT("Отмена"), wxDefaultPosition, wxDefaultSize, 0);
	szrButtons->Add(m_btnCancel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	szrMain->Add(szrButtons, 1, wxEXPAND, 5);

	this->SetSizer(szrMain);
	this->Layout();

	// Connect Events
	m_btnParam->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(whLogin::OnParam), NULL, this);
}
//---------------------------------------------------------------------------
whLogin::~whLogin()
{
	m_btnParam->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(whLogin::OnParam), NULL, this);
}

//---------------------------------------------------------------------------
void whLogin::OnParam( wxCommandEvent& event )
{
	auto xsize = GetSize();
	
	if (mPnlExt->IsShown())
	{
		mPnlExt->Hide();
		xsize.SetHeight(270);
	}
	else
	{
		mPnlExt->Show();
		xsize.SetHeight(400);
	}

	SetSize(xsize);
}
//---------------------------------------------------------------------------
void whLogin::OnOk( wxCommandEvent& event )
{
	// TODO: Implement OnOk
}
//---------------------------------------------------------------------------
void whLogin::OnCancel( wxCommandEvent& event )
{
	// TODO: Implement OnCancel
}
//---------------------------------------------------------------------------
wh::Cfg::DbConnect whLogin::GetCfg()const
{
	wh::Cfg::DbConnect cfg;

	cfg.mServer =	mTxtServer->GetValue();
	cfg.mDB =		mTxtDB->GetValue();
	mTxtPort->GetValue().ToCLong(&cfg.mPort);
	cfg.mUser =		m_cbxUserName->GetValue();
	cfg.mPass =		m_txtPass->GetValue();
	cfg.mRole =		mTxtRole->GetValue();
	cfg.mStorePass =m_chkStorePass->GetValue();

	return cfg;
}
//---------------------------------------------------------------------------
void whLogin::SetCfg(const wh::Cfg::DbConnect& cfg)
{
	mTxtServer->SetValue(cfg.mServer);
	mTxtDB->SetValue(cfg.mDB);
	mTxtPort->SetValue(wxString::Format("%d",cfg.mPort));
	m_cbxUserName->SetValue(cfg.mUser);
	m_txtPass->SetValue(cfg.mPass);
	mTxtRole->SetValue(cfg.mRole);
	m_chkStorePass->SetValue(cfg.mStorePass);
}

