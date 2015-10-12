#include "_pch.h"
#include "dlgselectcls_ctrlpnl.h"

using namespace wh;
using namespace wh::select;

//---------------------------------------------------------------------------
ClsDlg::ClsDlg(wxWindow* parent,
	wxWindowID id,
	const wxString& title,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name)
	:wxDialog(parent, id, title, pos, size, style, name)
	, mMainPanel(nullptr)
{
	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);


	m_sdbSizer = new wxStdDialogButtonSizer();
	m_btnOK = new wxButton(this, wxID_OK);//,"Сохранить и закрыть" );
	m_sdbSizer->AddButton(m_btnOK);
	m_btnCancel = new wxButton(this, wxID_CANCEL);//," Закрыть" );
	m_sdbSizer->AddButton(m_btnCancel);
	
	m_sdbSizer->Realize();

	mMainPanel = new view::VObjCatalogCtrl(this);
	szrMain->Add(mMainPanel, 1, wxALL | wxEXPAND, 0);
	szrMain->Add(m_sdbSizer, 0, wxALL | wxEXPAND, 10);

	this->SetSizer(szrMain);
}
//---------------------------------------------------------------------------
void ClsDlg::SetModel(std::shared_ptr<IModel> model)
{
	mMainPanel->SetModel(model);
}
//---------------------------------------------------------------------------
bool ClsDlg::GetSelectedCls(wh::rec::Cls& cls)
{
	return false;
}
//---------------------------------------------------------------------------
bool ClsDlg::GetSelectedObj(wh::rec::ObjTitle& obj)
{
	return false;
}

