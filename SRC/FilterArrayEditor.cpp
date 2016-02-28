#include "_pch.h"
#include "FilterArrayEditor.h"
#include "PGPTypeId.h"

using namespace wh;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// PathPatternEditor
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
FilterArrayEditor::FilterArrayEditor(wxWindow *parent,
	wxWindowID winid,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name)
	: wxScrolledWindow(parent, winid, pos, size, style, name)
{
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	this->SetScrollRate(5, 5);
	
	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);
	mPropGrid = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_SPLITTER_AUTO_CENTER);
	szrMain->Add(mPropGrid, 1, wxALL | wxEXPAND, 0);
	
	wxButton* apply_btn = new wxButton(this, wxID_APPLY,"Применить");
	szrMain->Add(apply_btn, 0, wxEXPAND, 10);

	
	this->SetSizer(szrMain);
	this->Layout();

	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &FilterArrayEditor::OnApply, this, wxID_APPLY);

}

//---------------------------------------------------------------------------
void FilterArrayEditor::SetModel(std::shared_ptr<IFieldArray>& newModel)
{
	if (newModel == mModel)
		return;
	mModel = newModel;
	if (!mModel)
		return;

	namespace ph = std::placeholders;
	
	for (unsigned int i = 0; i < mModel->size(); i++)
	{
		auto fld = mModel->at(i);
		if (fld)
		{
			const auto& fldData = fld->GetData();
			wxPGProperty* pgp = nullptr;
			switch (fldData.mType)
			{
			case ftText:	mPropGrid->Append(new wxLongStringProperty(fldData.mTitle)); break;
			case ftName:	mPropGrid->Append(new wxStringProperty(fldData.mTitle)); break;
			case ftLong:	
				if (FieldEditor::Type == fldData.mEditor)
				{
					pgp = mPropGrid->Append(new wxPGPSmallType(fldData.mTitle));
					auto chs = pgp->GetChoices();
					chs.Insert(wxEmptyString, 0, -1);
					pgp->SetChoices(chs);
					pgp->SetChoiceSelection(-1);
				}
				else if (FieldEditor::Normal == fldData.mEditor)
				{
					pgp = mPropGrid->Append(new wxStringProperty(fldData.mTitle));
					pgp->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
				}
				break;
			case ftDouble:	mPropGrid->Append(new wxFloatProperty(fldData.mTitle));  break;
			case ftDate:	mPropGrid->Append(new wxDateProperty(fldData.mTitle));  break;
			case ftLink:	mPropGrid->Append(new wxStringProperty(fldData.mTitle));  break;
			case ftFile:	mPropGrid->Append(new wxStringProperty(fldData.mTitle));  break;
			case ftJSON:	mPropGrid->Append(new wxLongStringProperty(fldData.mTitle));  break;
			default:break;
			}
		}
	}
}
//---------------------------------------------------------------------------
void FilterArrayEditor::OnApply(wxCommandEvent& evt)
{
	bool hasEmpty = false;

	for (unsigned int i = 0; i < mModel->size(); ++i)
	{
		auto fld = mModel->at(i);
		if (fld)
		{
			auto fldData = fld->GetData();
			auto pgProp = mPropGrid->GetProperty(fldData.mTitle);
			if (pgProp)
			{
				wxString gui_value;
				fldData.mFilter.clear();
				
				if (ftLong == fldData.mType && fldData.mEditor == FieldEditor::Type)
				{
					int cs = pgProp->GetChoiceSelection();
					int ftype = pgProp->GetChoices().GetValue(cs);
					if (-1 != ftype)
						fldData.mFilter.emplace_back(wxString::Format("%d", ftype));
				}
				else if (ftName == fldData.mType || ftText == fldData.mType)
				{
					gui_value = pgProp->GetValueAsString().Trim().Trim(false);
					if (!gui_value.IsEmpty())
						fldData.mFilter.emplace_back("%"+gui_value+"%", foLike);
				}
				else
				{
					gui_value = pgProp->GetValueAsString().Trim().Trim(false);
					if (!gui_value.IsEmpty())
						fldData.mFilter.emplace_back(gui_value);
				}
				
				fld->SetData(fldData);
			}//if (pgProp)
		}//if (fld)
	}
	wxCommandEvent refresh_evt(wxEVT_COMMAND_MENU_SELECTED, wxID_REFRESH);
	this->GetParent()->ProcessWindowEvent(refresh_evt);
}