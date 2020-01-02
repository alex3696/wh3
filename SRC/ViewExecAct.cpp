#include "_pch.h"
#include "ViewExecAct.h"

using namespace wh;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewExecActWindow::ViewExecActWindow(wxWindow* parent)
{
	mPanel = new wxDialog(parent, wxID_ANY, "Выполнение действия"
		, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	const wxIcon  ico("ICO_ACT_24", wxBITMAP_TYPE_ICO_RESOURCE, 24, 24);
	mPanel->SetIcon(ico);
	mPanel->SetSize(mPanel->GetSize()*1.8);
	wxSizer *mainSz = new wxBoxSizer(wxVERTICAL);
	// content	
	wxBoxSizer* szr_content = new wxBoxSizer(wxVERTICAL);
	mObjBrowser = std::make_shared<ViewTableBrowser>(mPanel);
	mActBrowser = std::make_shared<ViewActBrowser>(mPanel);
	szr_content->Add(mObjBrowser->GetWnd(), 1, wxALL | wxEXPAND, 0);
	szr_content->Add(mActBrowser->GetWnd(), 1, wxALL | wxEXPAND, 0);
	mainSz->Add(szr_content, 1, wxEXPAND, 0);
	// buttons
	wxBoxSizer* msdbSizer = new wxBoxSizer(wxHORIZONTAL);
	msdbSizer->Add(0, 0, 1, wxEXPAND, 5);
	auto mbtnOK = new wxButton(mPanel, wxID_OK, "Выполнить");
	auto mbtnCancel = new wxButton(mPanel, wxID_CANCEL, "Закрыть");
	msdbSizer->Add(mbtnOK, 0, wxALL, 5);
	msdbSizer->Add(mbtnCancel, 0, wxALL, 5);
	mainSz->Add(msdbSizer, 0, wxEXPAND, 10);

	//mPanel->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ViewUndoWindow::OnCmd_ExecuteUndo, this, wxID_OK);
	mPanel->SetSizer(mainSz);
	mPanel->Layout();
	mPanel->Centre(wxBOTH);

}
//-----------------------------------------------------------------------------
ViewExecActWindow::ViewExecActWindow(const std::shared_ptr<IViewWindow>& parent)
	: ViewExecActWindow(parent->GetWnd())
{

}
//-----------------------------------------------------------------------------
//virtual 
void ViewExecActWindow::SetShow() //override
{
	mPanel->ShowModal();
}
//-----------------------------------------------------------------------------
//virtual 
void ViewExecActWindow::SetUpdateTitle(const wxString& str, const wxIcon& ico)//override;
{
	mPanel->SetTitle(str);
	mPanel->SetIcon(ico);
}

//-----------------------------------------------------------------------------
//virtual 
std::shared_ptr<ViewTableBrowser> ViewExecActWindow::GetViewObjBrowser()const
{
	return mObjBrowser;
}
//-----------------------------------------------------------------------------
//virtual 
std::shared_ptr<ViewActBrowser> ViewExecActWindow::GetViewActBrowser()const
{
	return mActBrowser;
}
