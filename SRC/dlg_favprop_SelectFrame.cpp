#include "_pch.h"
#include "dlg_favprop_SelectFrame.h"

using namespace wh;
using namespace dlg::favprop::view;

//---------------------------------------------------------------------------
SelectFrame::SelectFrame(wxWindow*		parent,
	wxWindowID		id,
	const wxString& title,
	const wxPoint&	pos,
	const wxSize&	size,
	long style,
	const wxString& name)
	: wxDialog(parent, id, title, pos, size, style, name)
{
	auto mgr = ResMgr::GetInstance();
	this->SetIcon(wxIcon(mgr->m_ico_favprop_select24));

	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);
	SetSizer(szrMain);

	mCtrlPnl = new CtrlPnl(this);
	szrMain->Add(mCtrlPnl, 1, wxALL | wxEXPAND, 1);

	wxBoxSizer* msdbSizer = new wxBoxSizer(wxHORIZONTAL);
	msdbSizer->Add(0, 0, 1, wxEXPAND, 5);
	mbtnOK = new wxButton(this, wxID_OK, "Сохранить и закрыть");
	mbtnCancel = new wxButton(this, wxID_CANCEL, "Отмена");
	msdbSizer->Add(mbtnCancel, 0, wxALL, 5);
	msdbSizer->Add(mbtnOK, 0, wxALL, 5);
	szrMain->Add(msdbSizer, 0, wxEXPAND, 10);

	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &SelectFrame::OnOk, this, wxID_OK);

	this->SetSizer(szrMain);
	this->Layout();


}

//---------------------------------------------------------------------------
void SelectFrame::SetModel(std::shared_ptr<IModel>& model)
{
	mModel = std::dynamic_pointer_cast<model::FavPropArray>(model);
	if (!mModel)
		return;
	mCtrlPnl->SetModel(model);
}
//---------------------------------------------------------------------------
void SelectFrame::OnOk(wxCommandEvent& evt)
{
	if (!mModel)
		return;
	mModel->Save();
	EndModal(wxID_OK);
}
