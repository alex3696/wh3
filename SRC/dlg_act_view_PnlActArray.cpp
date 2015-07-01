#include "_pch.h"
#include "dlg_act_view_PnlActArray.h"
#include "TViewCtrlPanel.h"


using namespace wh;
using namespace wh::dlg_act::view;

PnlActArray::PnlActArray(wxWindow* parent,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name)
	: wxPanel(parent, id, pos, size, style, name)
{

	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);
	SetSizer(szrMain);

	mActList = new ActList(this);

	szrMain->Add(mActList, 1, wxALL | wxEXPAND, 0);


	//Bind(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, &CtrlPanel::OnActivated, this);

	//Layout();
}//
//-----------------------------------------------------------------------------
PnlActArray::~PnlActArray()
{

}
//-----------------------------------------------------------------------------
void PnlActArray::SetModel(std::shared_ptr<dlg_act::model::ActArray>& model)
{
	mActArray = model;
	mActList->SetModel(model);
}

void PnlActArray::GetSelected(std::vector<unsigned int>& vec)
{
	mActList->GetSelected(vec);
}