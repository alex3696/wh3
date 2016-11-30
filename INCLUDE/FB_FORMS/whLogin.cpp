#include "_pch.h"
#include "whLogin.h"
#include "ConnectionCfgDlg.h"
#include "config.h"

wxString GetAppVersion()
{
	wxStandardPathsBase& stdp = wxStandardPaths::Get();
	auto wxFileName = stdp.GetExecutablePath();
	
	const wchar_t *pszFilePath = wxFileName.wc_str();

	DWORD               dwSize = 0;
	BYTE                *pbVersionInfo = NULL;
	VS_FIXEDFILEINFO    *pFileInfo = NULL;
	UINT                puLenFileInfo = 0;

	// get the version info for the file requested
	dwSize = GetFileVersionInfoSize(pszFilePath, NULL);
	if (dwSize == 0)
	{
		printf("Error in GetFileVersionInfoSize: %d\n", GetLastError());
		return "version error";
	}

	pbVersionInfo = new BYTE[dwSize];

	if (!GetFileVersionInfo(pszFilePath, 0, dwSize, pbVersionInfo))
	{
		printf("Error in GetFileVersionInfo: %d\n", GetLastError());
		delete[] pbVersionInfo;
		return "version error";
	}

	if (!VerQueryValue(pbVersionInfo, TEXT("\\"), (LPVOID*)&pFileInfo, &puLenFileInfo))
	{
		printf("Error in VerQueryValue: %d\n", GetLastError());
		delete[] pbVersionInfo;
		return "version error";
	}


	DWORD v1 = (pFileInfo->dwFileVersionMS >> 16) & 0xFFFF; 
	DWORD v2 = (pFileInfo->dwFileVersionMS >> 0) & 0xFFFF; 
	DWORD v3 = (pFileInfo->dwFileVersionLS >> 16) & 0xFFFF; 
	DWORD v4 = (pFileInfo->dwFileVersionLS >> 0) & 0xFFFF; 

	delete[] pbVersionInfo;

	// pFileInfo->dwFileVersionMS is usually zero. However, you should check
	// this if your version numbers seem to be wrong

	//printf("File Version: %d.%d.%d.%d\n",
	//	(pFileInfo->dwFileVersionLS >> 24) & 0xff,
	//	(pFileInfo->dwFileVersionLS >> 16) & 0xff,
	//	(pFileInfo->dwFileVersionLS >> 8) & 0xff,
	//	(pFileInfo->dwFileVersionLS >> 0) & 0xff
	//	);

	//// pFileInfo->dwProductVersionMS is usually zero. However, you should check
	//// this if your version numbers seem to be wrong

	//printf("Product Version: %d.%d.%d.%d\n",
	//	(pFileInfo->dwProductVersionLS >> 24) & 0xff,
	//	(pFileInfo->dwProductVersionLS >> 16) & 0xff,
	//	(pFileInfo->dwProductVersionLS >> 8) & 0xff,
	//	(pFileInfo->dwProductVersionLS >> 0) & 0xff
	//	);
	return	wxString::Format("%d.%d.%d.%d", v1, v2, v3, v4);

}

//---------------------------------------------------------------------------
whLogin::whLogin(wxWindow* parent, wxWindowID id, const wxString& title
	, const wxPoint& pos, const wxSize& size, long style) 
	: wxDialog(parent, id, title, pos, size, style | wxFRAME_TOOL_WINDOW)
{
	Centre(wxBOTH);
	this->SetExtraStyle(wxDIALOG_EX_METAL);

	wxString title_str;
	title_str << "Авторизация " << "wh" << GetAppVersion();
	this->SetTitle(title_str);

	wxBoxSizer* szrMain;
	szrMain = new wxBoxSizer(wxVERTICAL);

	wxStaticBitmap* imgLogo;
	imgLogo = new wxStaticBitmap(this, wxID_ANY, wxBitmap(wxT("../../RESOURCES/login_caption.png"), wxBITMAP_TYPE_ANY), wxDefaultPosition, wxDefaultSize, 0);
	szrMain->Add(imgLogo, 1, 0, 5);
	//szrMain->Add(0, 0, 3, wxEXPAND, 5);

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

	m_txtPass = new wxTextCtrl(this, wxID_ANY);
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

	this->SetSizer(szrMain);
	this->Layout();

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

	mgr->mDb.Open(conn_cfg.mServer
		, conn_cfg.mPort
		, conn_cfg.mDB
		, conn_cfg.mUser
		, conn_cfg.mPass);



	EndModal(wxID_OK);
}
