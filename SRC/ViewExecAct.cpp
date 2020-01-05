#include "_pch.h"
#include "ViewExecAct.h"

using namespace wh;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewExecActWindow::ViewExecActWindow(wxWindow* parent)
{
	mPanel = new wxDialog(parent, wxID_ANY, wxEmptyString
		, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	const wxIcon  ico("ICO_ACT_24", wxBITMAP_TYPE_ICO_RESOURCE, 24, 24);
	mPanel->SetIcon(ico);
	mPanel->SetSize(mPanel->GetSize()*1.8);
	wxSizer *mainSz = new wxBoxSizer(wxVERTICAL);
	
	mInfo = new wxStaticText(mPanel, wxID_ANY, wxEmptyString);
	mInfo->Wrap(-1);
	mainSz->Add(mInfo, 0, wxALL | wxEXPAND, 5);
	
	//Panel Object list
	mObjListPanel = new wxPanel(mPanel);
	wxSizer *objListSzr = new wxBoxSizer(wxVERTICAL);
	mObjListPanel->SetSizer(objListSzr);
	mObjBrowser = std::make_shared<ViewTableBrowser>(mObjListPanel);
	objListSzr->Add(mObjBrowser->GetWnd(), 1, wxEXPAND, 5);

	//Panel Act list
	mActListPanel = new wxPanel(mPanel);
	mActBrowser = std::make_shared<ViewActBrowser>(mActListPanel);
	wxSizer *actListSzr = new wxBoxSizer(wxVERTICAL);
	mActListPanel->SetSizer(actListSzr);
	actListSzr->Add(mActBrowser->GetWnd(), 1, wxEXPAND, 5);

	//Panel Property list
	mPropListPanel = new wxPanel(mPanel);
	auto mPg = new wxPropertyGrid(mPropListPanel);
	wxSizer *propListSzr = new wxBoxSizer(wxVERTICAL);
	mPropListPanel->SetSizer(propListSzr);
	propListSzr->Add(mPg, 1, wxEXPAND, 5);
	
	mActListPanel->Hide();
	mPropListPanel->Hide();

	mainSz->Add(mObjListPanel, 1, wxEXPAND, 5);
	mainSz->Add(mActListPanel, 1, wxEXPAND, 5);
	mainSz->Add(mPropListPanel, 1, wxEXPAND, 5);

	// buttons
	wxBoxSizer* msdbSizer = new wxBoxSizer(wxHORIZONTAL);
	msdbSizer->Add(0, 0, 1, wxEXPAND, 5);
	msdbSizer->Add(0, 0, 1, wxEXPAND, 5);
	mBtnBack = new wxButton(mPanel, wxID_BACKWARD, "< Назад");
	mBtnForward = new wxButton(mPanel, wxID_OK, "Далее >");
	auto mbtnCancel = new wxButton(mPanel, wxID_CANCEL, "Закрыть");
	msdbSizer->Add(mBtnBack, 0, wxALL, 5);
	msdbSizer->Add(mBtnForward, 0, wxALL, 5);
	msdbSizer->Add(mbtnCancel, 0, wxALL, 5);
	mainSz->Add(msdbSizer, 0, wxEXPAND, 10);

	mPanel->SetSizer(mainSz);
	mPanel->Layout();
	mPanel->Centre(wxBOTH);

	//mPanel->Bind(wxEVT_CLOSE_WINDOW, &ViewExecActWindow::OnClose, this);

	
	mPanel->Bind(wxEVT_DESTROY, [this](wxWindowDestroyEvent&) 
	{
		mObjBrowser.reset();
		mActBrowser.reset();
		mPanel = nullptr;
	});

	mPanel->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ViewExecActWindow::OnCancel, this, wxID_CANCEL);
	mPanel->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ViewExecActWindow::OnOk, this, wxID_OK);
	mPanel->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ViewExecActWindow::OnBack, this, wxID_BACKWARD);

}
//-----------------------------------------------------------------------------
ViewExecActWindow::~ViewExecActWindow()
{
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
	mPanel->SetTitle(wxString::Format("Выполнение действия. Начато: %s",
		wxDateTime::Now().Format()));
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
//-----------------------------------------------------------------------------
void ViewExecActWindow::OnClose(wxCloseEvent& evt)
{
	this->sigUnlock();
	mPanel->EndModal(wxID_CANCEL);
	//mPanel->Destroy();
	//mPanel = nullptr;
}
//-----------------------------------------------------------------------------
void ViewExecActWindow::OnCancel(wxCommandEvent& evt)
{
	OnClose(wxCloseEvent());
}
//-----------------------------------------------------------------------------
void ViewExecActWindow::SetSelectPage(int page)
{
	switch (page)
	{
	default:
	case 0:
		mObjListPanel->Show();
		mActListPanel->Hide();
		mPropListPanel->Hide();
		mBtnBack->Hide();
		mBtnForward->SetLabel("Вперёд >");
		mInfo->SetLabel("Внимание! "
			"Выбранные объекты заблокированы на 10 минут для выполнения действия."
			"\nДля перехода к выбору действий нажмите кнопку 'Далее >'"	);
		mPanel->Layout();
		break;
	case 1:
		mObjListPanel->Hide();
		mActListPanel->Show();
		mPropListPanel->Hide();
		mBtnBack->Show();
		mBtnForward->SetLabel("Далее >");
		mInfo->SetLabel(
			"Для перехода к заполнению свойств"
			" двойной клик левой кнопкой мыши на необходимом дествии или"
			"\nвыберите действие нажатием левой кнопкой мыши и нажмите кнопку 'Далее >'");
		mPanel->Layout();
	break;
	case 2:
		mObjListPanel->Hide();
		mActListPanel->Hide();
		mPropListPanel->Show();
		mBtnBack->Show();
		mBtnForward->SetLabel("Выполнить");
		mInfo->SetLabel(
			"Заполните свойства для выбранного ранее действия"
			"\nДля сохранения действия нажмите кнопку 'Выполнить'");

		mPanel->Layout();
		break;
	}
}
//-----------------------------------------------------------------------------
void ViewExecActWindow::OnOk(wxCommandEvent& evt)
{
	if (mPropListPanel->IsShown())
	{ 
		sigExecute();
		return;
	}
	
	if (mActListPanel->IsShown())
	{
		sigSelectAct();
		return;
	}
	SetSelectPage(1);
}
//-----------------------------------------------------------------------------
void ViewExecActWindow::OnBack(wxCommandEvent& evt)
{
	int page = 0;

	if (mActListPanel->IsShown())
		page = 0;
	else if (mPropListPanel->IsShown())
		page = 1;
	else
		page = 1;
	
	SetSelectPage(page);
}
//-----------------------------------------------------------------------------
void ViewExecActWindow::OnActivated(wxDataViewEvent &evt)
{

}
