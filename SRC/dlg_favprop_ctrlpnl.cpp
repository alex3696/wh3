#include "_pch.h"
#include "dlg_favprop_ctrlpnl.h"

using namespace wh;
using namespace dlg::favprop::view ;


//-----------------------------------------------------------------------------
CtrlPnl::CtrlPnl(wxWindow* parent,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name)
	: wxPanel(parent, id, pos, size, style, name)
{
	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);
	SetSizer(szrMain);

	mToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_TEXT);
	GetSizer()->Add(mToolBar, 0, wxALL | wxEXPAND, 0);

	mTableView = new DvTable(this);
	GetSizer()->Add(mTableView, 1, wxALL | wxEXPAND, 0);

	mToolBar->AddTool(wxID_REFRESH, "Обновить", m_ResMgr->m_ico_refresh24);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &CtrlPnl::OnCmdReload, this, wxID_REFRESH);
	
	mToolBar->Realize();
	Layout();
}
//-----------------------------------------------------------------------------
CtrlPnl::~CtrlPnl()
{
}
//-----------------------------------------------------------------------------
void CtrlPnl::SetModel(std::shared_ptr<IModel> model)
{
	mModel = std::dynamic_pointer_cast<model::FavPropArray>(model);
	if (!mModel)
		return;

	mTableView->SetModel(model);
}
//-----------------------------------------------------------------------------
void CtrlPnl::OnCmdReload(wxCommandEvent& evt)
{
	if (mModel)
		mModel->Load();
}
