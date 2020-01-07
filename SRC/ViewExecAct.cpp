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
	mPanel->SetMinSize(mPanel->GetSize());
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
	mPropPG = std::make_shared<ViewPropPg>(mPropListPanel);
	wxSizer *propListSzr = new wxBoxSizer(wxVERTICAL);
	mPropListPanel->SetSizer(propListSzr);
	propListSzr->Add(mPropPG->GetWnd(), 1, wxEXPAND, 5);
	
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

	
	mPanel->Bind(wxEVT_DESTROY, [this](wxWindowDestroyEvent& evt) 
	{
		if (evt.GetWindow() != mPanel)
			return;
		mTimer.Stop();
		mObjBrowser.reset();
		mActBrowser.reset();
		mPropPG.reset();
		mPanel = nullptr;
	});

	mPanel->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ViewExecActWindow::OnCancel, this, wxID_CANCEL);
	mPanel->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ViewExecActWindow::OnOk, this, wxID_OK);
	mPanel->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ViewExecActWindow::OnBack, this, wxID_BACKWARD);
	mTimer.Bind(wxEVT_TIMER, &ViewExecActWindow::OnTimer, this);
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
void ViewExecActWindow::SetClose()//override;
{
	StopCountdown();
	mPanel->EndModal(wxID_CANCEL);
}
//-----------------------------------------------------------------------------
//virtual 
void ViewExecActWindow::SetShow() //override
{
	//mPanel->SetTitle(wxString::Format("Выполнение действия. Начато: %s",
	//	wxDateTime::Now().Format()));
	StartCountdown();
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
//virtual 
std::shared_ptr<ViewPropPg> ViewExecActWindow::GetViewPropPG()const
{
	return mPropPG;
}
//-----------------------------------------------------------------------------
void ViewExecActWindow::OnClose(wxCloseEvent& evt)
{
	this->sigUnlock();
	SetClose();
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
	if (mObjListPanel->IsShown())
	{
		SetSelectPage(1);
		return;
	}
	if (mActListPanel->IsShown())
	{
		sigSelectAct();
		return;
	}

	if (mPropListPanel->IsShown())
	{ 
		sigExecute();
		return;
	}
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
void ViewExecActWindow::OnTimer(wxTimerEvent &evt)
{
	StepCountdown();
}
//-----------------------------------------------------------------------------
void ViewExecActWindow::StartCountdown()
{
	mMillSecLeft = 1000 * 60 * 10;//10min
	mTimer.Start(1000 * 10);//10sec
	mPanel->SetTitle("Выполнение действия");
}
//-----------------------------------------------------------------------------
void ViewExecActWindow::StepCountdown()
{
	if (mMillSecLeft > 0)
	{
		mMillSecLeft -= 1000 * 10;

		mPanel->SetTitle(wxString::Format(
			"Выполнение действия."
			" Автосброс блокировки объекта через %d:%d"
			, mMillSecLeft / 1000 / 60
			, (mMillSecLeft/1000) % 60 ));
	}
	else
	{
		mTimer.Stop();
		mMillSecLeft = 0;
		mPanel->SetTitle("Выполнение действия невозможно. "
			"Перезапустите этот диалог ");
	}


}
//-----------------------------------------------------------------------------
void ViewExecActWindow::StopCountdown()
{
	mTimer.Stop();
}