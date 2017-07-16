#include "_pch.h"
#include "ConnectionCfgDlg.h"
#include "config.h"

//---------------------------------------------------------------------------
ConnectionCfgDlg::ConnectionCfgDlg(wxWindow* parent, wxWindowID id, const wxString& title
	, const wxPoint& pos, const wxSize& size, long style)
	: wxDialog(parent, id, title, pos, size, style)
{
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
	
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ConnectionCfgDlg::OnOk, this, wxID_OK);

	const auto& conn_cfg = whDataMgr::GetInstance()->mConnectCfg->GetData();
	mPGPServer->SetValueFromString(conn_cfg.mServer);
	mPGPPort->SetValueFromInt(conn_cfg.mPort);
	mPGPBd->SetValueFromString(conn_cfg.mDB);
	mPGPRole->SetValueFromString(conn_cfg.mRole);

	//mPG->ResetColumnSizes(true);
	mPG->SetMinSize(wxSize(200, mPG->GetRowHeight() * 5));
	mPG->FitColumns();

	szrMain->SetSizeHints(this);
	this->SetSizerAndFit(szrMain);
	this->Layout();
	Centre(wxBOTH);
}
//---------------------------------------------------------------------------
ConnectionCfgDlg::~ConnectionCfgDlg()
{
	Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ConnectionCfgDlg::OnOk, this, wxID_OK);
}
//---------------------------------------------------------------------------
void ConnectionCfgDlg::OnOk(wxCommandEvent& evt)
{
	auto conn_cfg = whDataMgr::GetInstance()->mConnectCfg->GetData();
	conn_cfg.mServer = mPGPServer->GetValueAsString();
	conn_cfg.mPort = mPGPPort->GetValue().GetInteger();
	conn_cfg.mDB = mPGPBd->GetValueAsString();
	conn_cfg.mRole = mPGPRole->GetValueAsString();

	whDataMgr::GetInstance()->mConnectCfg->SetData(conn_cfg);
	whDataMgr::GetInstance()->mConnectCfg->Save();

	EndModal(wxID_OK);
}
