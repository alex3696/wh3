#include "_pch.h"
#include "ConnectionCfgDlg.h"

//---------------------------------------------------------------------------
ConnectionCfgDlg::ConnectionCfgDlg(wxWindow* parent, wxWindowID id, const wxString& title
	, const wxPoint& pos, const wxSize& size, long style)
	: wxDialog(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	this->SetTitle("Настройки подключения");

	wxBoxSizer* szrMain;
	szrMain = new wxBoxSizer(wxVERTICAL);

	mPG = new wxPropertyGrid(this);
	mPGPServer = mPG->Append(new wxStringProperty("Сервер"));
	mPGPPort = mPG->Append(new wxIntProperty("Порт"));
	mPGPBd = mPG->Append(new wxStringProperty("БД"));
	mPGPRole = mPG->Append(new wxStringProperty("Роль"));
	szrMain->Add(mPG, 1, wxEXPAND, 5);

	wxBoxSizer* szrButtons = new wxBoxSizer(wxHORIZONTAL);

	szrButtons->AddStretchSpacer();
	m_btnOK = new wxButton(this, wxID_OK, wxT("Сохранить"), wxDefaultPosition, wxDefaultSize, 0);
	szrButtons->Add(m_btnOK, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	m_btnCancel = new wxButton(this, wxID_CANCEL, wxT("Отмена"), wxDefaultPosition, wxDefaultSize, 0);
	szrButtons->Add(m_btnCancel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	szrMain->Add(szrButtons, 0, wxEXPAND, 5);

	this->SetSizer(szrMain);
	this->Layout();
	mPG->ResetColumnSizes(true);
	
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ConnectionCfgDlg::OnOk, this, wxID_OK);


	wh::Cfg::DbConnect& dbcfg = whDataMgr::GetInstance()->mCfg.mConnect;
	dbcfg.Load();
	mPGPServer->SetValueFromString(dbcfg.mServer);
	mPGPPort->SetValueFromInt(dbcfg.mPort);
	mPGPBd->SetValueFromString(dbcfg.mDB);
	mPGPRole->SetValueFromString(dbcfg.mRole);

}
//---------------------------------------------------------------------------
ConnectionCfgDlg::~ConnectionCfgDlg()
{
	Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ConnectionCfgDlg::OnOk, this, wxID_OK);
}
//---------------------------------------------------------------------------
void ConnectionCfgDlg::OnOk(wxCommandEvent& evt)
{
	wh::Cfg::DbConnect& dbcfg = whDataMgr::GetInstance()->mCfg.mConnect;
	dbcfg.Load();
	dbcfg.mServer = mPGPServer->GetValueAsString();
	dbcfg.mPort = mPGPPort->GetValue().GetInteger();
	dbcfg.mDB = mPGPBd->GetValueAsString();
	dbcfg.mRole = mPGPRole->GetValueAsString();
	dbcfg.Save();

	EndModal(wxID_OK);
}
