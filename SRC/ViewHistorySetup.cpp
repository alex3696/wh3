#include "_pch.h"
#include "ViewHistorySetup.h"
#include "globaldata.h"

using namespace wh;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewHistorySetup::ViewHistorySetup(std::shared_ptr<IViewWindow> parent)
{
	mPanel = new wxDialog(parent->GetWnd(), wxID_ANY, "Настройки истории"
		, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	mPanel->Centre(wxBOTH);

	wxSizer *mainSz = new wxBoxSizer(wxVERTICAL);

	int style = wxPG_SPLITTER_AUTO_CENTER | wxPG_BOLD_MODIFIED | wxPG_DESCRIPTION;
	int extraStyle = wxPG_EX_HELP_AS_TOOLTIPS;
	mPG = new wxPropertyGrid(mPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	mPG->CenterSplitter(true);
	mPG->SetExtraStyle(extraStyle);
	mainSz->Add(mPG, 1, wxALL | wxEXPAND, 0);

	mPGPRowsLimit = new wxIntProperty("Записей на странице", wxPG_LABEL, 30);
	mPGPRowsLimit->SetEditor(wxPGEditor_SpinCtrl);
	mPGPRowsLimit->SetAttribute(wxPG_ATTR_MIN, (long)1);
	mPGPRowsLimit->SetAttribute(wxPG_ATTR_MAX, (long)10000);
	//mPGPRowsLimit->SetAttribute(wxPG_ATTR_UNITS, wxT("записей"));
	//pgp->SetValueToUnspecified();// Set value to unspecified so that Hint attribute will be demonstrated
	//mPGPRowsLimit->SetAttribute(wxPG_ATTR_HINT, wxT("Задайте количество записей в таблице истории 1-10000"));
	mPGPRowsLimit->SetHelpString("Задайте количество записей в таблице истории 1-10000");
	mPG->Append(mPGPRowsLimit);
	

	mPGPRowsOffset = new wxIntProperty("Пропуск записей", wxPG_LABEL, 0);
	mPGPRowsOffset->SetAttribute(wxPG_ATTR_MIN, (long)0);
	mPGPRowsOffset->SetAttribute(wxPG_ATTR_MAX, (long)MAXINT32);
	mPGPRowsOffset->SetEditor(wxPGEditor_SpinCtrl);
	mPG->Append(mPGPRowsOffset);

	mPGPStrPerRow = new wxIntProperty("Строк в записи", wxPG_LABEL, 4);
	mPGPStrPerRow->SetAttribute(wxPG_ATTR_MIN, (long)2);
	mPGPStrPerRow->SetAttribute(wxPG_ATTR_MAX, (long)20);
	mPGPStrPerRow->SetEditor(wxPGEditor_SpinCtrl);
	//mPGPStrPerRow->SetAttribute(wxPG_ATTR_HINT, wxT("Задайте количество строк для отображения одной записи"));
	mPGPStrPerRow->SetHelpString("Задайте количество строк для отображения одной записи");
	mPG->Append(mPGPStrPerRow);
	
	mPGPPathInProperties = new wxBoolProperty("Показывать путь в действиях");
	mPGPPathInProperties->SetAttribute(wxPG_BOOL_USE_CHECKBOX, true);
	mPG->Append(mPGPPathInProperties);
	
	mPGPColAutosize = new wxBoolProperty("Авторазмер столбцов");
	mPGPColAutosize->SetAttribute(wxPG_BOOL_USE_CHECKBOX, true);
	mPG->Append(mPGPColAutosize);

	mPGPShowPropertyList = new wxBoolProperty("Отображать панель свойств");
	mPGPShowPropertyList->SetAttribute(wxPG_BOOL_USE_CHECKBOX, true);
	mPG->Append(mPGPShowPropertyList);

	mPGPShowFilterList = new wxBoolProperty("Отображать панель фильтров");
	mPGPShowFilterList->SetAttribute(wxPG_BOOL_USE_CHECKBOX, true);
	mPG->Append(mPGPShowFilterList);
	
	wxBoxSizer* msdbSizer = new wxBoxSizer(wxHORIZONTAL);
	msdbSizer->Add(0, 0, 1, wxEXPAND, 5);
	auto mbtnOK = new wxButton(mPanel, wxID_OK, "Применить");
	auto mbtnCancel = new wxButton(mPanel, wxID_CANCEL, "Закрыть");
	msdbSizer->Add(mbtnOK, 0, wxALL, 5);
	msdbSizer->Add(mbtnCancel, 0, wxALL, 5);
	mainSz->Add(msdbSizer, 0, wxEXPAND, 10);



	mPanel->SetSizer(mainSz);
	mPanel->Layout();

	mPanel->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ViewHistorySetup::OnCmd_SetCfgToPage, this, wxID_OK);
}
//-----------------------------------------------------------------------------
//virtual 
wxWindow* ViewHistorySetup::GetWnd()const //override;
{
	return mPanel;
}
//-----------------------------------------------------------------------------
//virtual 
void ViewHistorySetup::ShowCfgWindow() //override
{
	mPanel->ShowModal();
}
//-----------------------------------------------------------------------------
//virtual 
void ViewHistorySetup::SetCfg(const rec::PageHistory& cfg) //override;
{
	mCurrentCfg = cfg;

	mPGPRowsLimit->SetValueFromInt(cfg.mRowsLimit);
	mPGPRowsOffset->SetValueFromInt(cfg.mRowsOffset);
	mPGPStrPerRow->SetValueFromInt(cfg.mStringPerRow);
	mPGPPathInProperties->SetValue(cfg.mPathInProperties);
	mPGPColAutosize->SetValue(cfg.mColAutosize);
	mPGPShowPropertyList->SetValue(cfg.mShowPropertyList);
	mPGPShowFilterList->SetValue(cfg.mShowFilterList);
}
//-----------------------------------------------------------------------------
void ViewHistorySetup::OnCmd_SetCfgToPage(wxCommandEvent& evt)
{
	rec::PageHistory cfg = mCurrentCfg;
	cfg.mRowsLimit = mPGPRowsLimit->GetValue().GetInteger();
	cfg.mRowsOffset = mPGPRowsOffset->GetValue().GetInteger();
	cfg.mStringPerRow = mPGPStrPerRow->GetValue().GetInteger();

	cfg.mPathInProperties = mPGPPathInProperties->GetValue().GetBool();
	cfg.mColAutosize = mPGPColAutosize->GetValue().GetBool();
	cfg.mShowPropertyList = mPGPShowPropertyList->GetValue().GetBool();
	cfg.mShowFilterList = mPGPShowFilterList->GetValue().GetBool();
	sigSetCfg(cfg);
}
//-----------------------------------------------------------------------------
void ViewHistorySetup::OnCmd_SetCfgToGlobal(wxCommandEvent& evt)
{

}

