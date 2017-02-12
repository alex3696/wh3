#include "_pch.h"
#include "ReportEditorView.h"

using namespace wh;

//-----------------------------------------------------------------------------
ReportEditorView::ReportEditorView(std::shared_ptr<wxWindow*> wnd)
{
	mFrame = new wxDialog(*wnd,wxID_ANY, "Редактирование отчёта"
		,wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	wxBoxSizer* mMainSizer;
	mMainSizer = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* mGridSizer;
	mGridSizer = new wxFlexGridSizer(2, 2, 0, 0);
	mGridSizer->AddGrowableCol(1);
	mGridSizer->AddGrowableRow(1);
	mGridSizer->SetFlexibleDirection(wxBOTH);
	mGridSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_ALL);

	wxStaticText* labelTitle = new wxStaticText(mFrame, wxID_ANY, wxT("Имя"), wxDefaultPosition, wxDefaultSize, 0);
	labelTitle->Wrap(-1);
	mGridSizer->Add(labelTitle, 0, wxALL, 5);

	mTitleText = new wxTextCtrl(mFrame, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mGridSizer->Add(mTitleText, 1, wxALL | wxEXPAND, 5);

	wxStaticText* labelNote = new wxStaticText(mFrame, wxID_ANY, wxT("Описание"), wxDefaultPosition, wxDefaultSize, 0);
	labelNote->Wrap(-1);
	mGridSizer->Add(labelNote, 0, wxALL, 5);

	mNoteText = new wxTextCtrl(mFrame, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	mGridSizer->Add(mNoteText, 1, wxALL | wxEXPAND, 5);


	mMainSizer->Add(mGridSizer, 1, wxEXPAND, 5);

	mScriptText = new wxTextCtrl(mFrame, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	mMainSizer->Add(mScriptText, 3, wxEXPAND | wxALL, 5);

	mBtnSizer = new wxStdDialogButtonSizer();
	mBtnSizerOK = new wxButton(mFrame, wxID_OK);
	mBtnSizer->AddButton(mBtnSizerOK);
	mBtnSizerCancel = new wxButton(mFrame, wxID_CANCEL);
	mBtnSizer->AddButton(mBtnSizerCancel);
	mBtnSizer->Realize();

	mMainSizer->Add(mBtnSizer, 0, wxEXPAND, 5);


	wxSize sz = mFrame->GetSize();
	sz = sz * 2;
	mFrame->SetSize(sz);

	mFrame->SetSizer(mMainSizer);
	mFrame->Layout();

	mFrame->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ReportEditorView::OnCancel, this, wxID_CANCEL);
	mFrame->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ReportEditorView::OnOk, this, wxID_OK);

}
//-----------------------------------------------------------------------------
void ReportEditorView::SetReportItem(const rec::ReportItem& item)
{
	mId = item.mId;
	mTitleText->SetValue(item.mTitle);
	mNoteText->SetValue(item.mNote);
	mScriptText->SetValue(item.mScript);

	if (mId.IsEmpty())
		mFrame->SetTitle("Создание отчёта");
	else
		mFrame->SetTitle("Редактирование отчёта id=" + mId);
}
//-----------------------------------------------------------------------------
void ReportEditorView::Show()
{
	mFrame->ShowModal();
}
//-----------------------------------------------------------------------------
void ReportEditorView::Close()
{
	mFrame->EndModal(wxID_CANCEL);
}
//-----------------------------------------------------------------------------

void ReportEditorView::OnCancel(wxCommandEvent& evt)
{
	mFrame->EndModal(wxID_CANCEL);
}
//-----------------------------------------------------------------------------
void ReportEditorView::OnOk(wxCommandEvent& evt)
{
	rec::ReportItem item;
	item.mId = mId;
	item.mTitle = mTitleText->GetValue();
	item.mNote = mNoteText->GetValue();
	item.mScript = mScriptText->GetValue();
	sigChItem(item);

	mFrame->EndModal(wxID_OK);
}