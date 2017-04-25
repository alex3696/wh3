#include "_pch.h"
#include "FilterArrayEditor.h"
#include "PGPTypeId.h"

using namespace wh;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// FilterArrayEditor
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
	



	wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
	auto clearFilter = new wxBitmapButton(this, wxID_CLEAR
		, wxArtProvider::GetBitmap(wxART_CROSS_MARK, wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
	btnSizer->Add(clearFilter, 0, wxALL, 5);


	wxButton* apply_btn = new wxButton(this, wxID_APPLY,"Применить");
	btnSizer->Add(apply_btn, 1, wxALL, 5);


	szrMain->Add(btnSizer, 0, wxEXPAND, 5);
	
	this->SetSizer(szrMain);
	this->Layout();

	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &FilterArrayEditor::OnApply, this, wxID_APPLY);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &FilterArrayEditor::OnClear, this, wxID_CLEAR);

}

//---------------------------------------------------------------------------
void FilterArrayEditor::SetModel(std::shared_ptr<ITable> newModel)
{
	if (newModel == mMTable)
		return;
	namespace ph = std::placeholders;

	mConnFieldAI.disconnect();
	mConnFieldBR.disconnect();
	mConnFieldAC.disconnect();

	mMTable = newModel;
	if (!mMTable)
		return;

	auto fnFieldAI = std::bind(&FilterArrayEditor::OnFieldAfterInsert, this, ph::_1, ph::_2, ph::_3);
	auto fnFieldBR = std::bind(&FilterArrayEditor::OnFieldBeforeRemove, this, ph::_1, ph::_2);
	auto fnFieldAC = std::bind(&FilterArrayEditor::OnFieldInfoChange, this, ph::_1, ph::_2);
	mConnFieldAI = mMTable->mFieldVec->ConnAfterInsert(fnFieldAI);
	mConnFieldBR = mMTable->mFieldVec->ConnectBeforeRemove(fnFieldBR);
	mConnFieldAC = mMTable->mFieldVec->ConnectChangeSlot(fnFieldAC);

	std::vector<SptrIModel> newItems;
	for (unsigned int i = 0; i < mMTable->mFieldVec->size(); i++)
		newItems.emplace_back(mMTable->mFieldVec->at(i));
	OnFieldAfterInsert(*mMTable->mFieldVec, newItems, nullptr);

}
//---------------------------------------------------------------------------
void FilterArrayEditor::OnApply(wxCommandEvent& evt)
{
	//bool hasEmpty = false;

	for (unsigned int i = 0; i < mMTable->mFieldVec->size(); ++i)
	{
		auto fld = mMTable->mFieldVec->at(i);
		if (fld)
		{
			const wh::Field& oldFldData = fld->GetData();
			auto fldData = fld->GetData();

			//bool ok = GetFilterValue(fldData);
			if (fldData != oldFldData )
				fld->SetData(fldData);
		}//if (fld)
	}
	mMTable->mPageNo->SetData(0);
	mMTable->Load();
}
//---------------------------------------------------------------------------
void FilterArrayEditor::OnClear(wxCommandEvent& evt)
{
	wxPropertyGridIterator it;
	for (it = mPropGrid->GetIterator(); !it.AtEnd(); it++)
		(*it)->SetValueToUnspecified();
}
//-----------------------------------------------------------------------------
wxPGProperty* FilterArrayEditor::MakeProperty(const wh::Field& field)
{
	const  wxString first_filter_val
		= field.mFilter.size() ? field.mFilter.at(0).mVal : wxEmptyString;
	
	wxPGProperty* pgp = nullptr;
	switch (field.mType)
	{
	case ftText:	pgp = new wxLongStringProperty(field.mTitle, wxPG_LABEL, first_filter_val); break;
	case ftName:	pgp = new wxStringProperty(field.mTitle, wxPG_LABEL, first_filter_val); break;
	case ftLong:
		if (FieldEditor::Type == field.mEditor)
		{
			pgp = new wxPGPSmallType(field.mTitle);
			auto chs = pgp->GetChoices();
			chs.Insert(wxEmptyString, 0, -1);
			pgp->SetChoices(chs);
			unsigned long val=-1;
			if (first_filter_val.ToCULong(&val))
				pgp->SetChoiceSelection(val);
			else
				pgp->SetValueToUnspecified();
		}
		else if (FieldEditor::Normal == field.mEditor)
		{
			pgp = new wxStringProperty(field.mTitle, wxPG_LABEL, first_filter_val);
			pgp->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
		}
		break;
	case ftDouble:	
		pgp = new wxFloatProperty(field.mTitle, wxPG_LABEL);  
		pgp->SetValueFromString(first_filter_val);
		break;
	case ftDateTime:
	case ftTime:
	case ftDate:	pgp = new wxDateProperty(field.mTitle);
		pgp->SetAttribute(wxPG_DATE_PICKER_STYLE
			, (long)(wxDP_DROPDOWN | wxDP_SHOWCENTURY | 	wxDP_ALLOWNONE));
		pgp->SetAttribute(wxPG_DATE_FORMAT
			, wxLocale::GetOSInfo(wxLOCALE_SHORT_DATE_FMT, wxLOCALE_CAT_DATE));
		break;
	case ftLink:	pgp = new wxStringProperty(field.mTitle, wxPG_LABEL, first_filter_val);  break;
	case ftFile:	pgp = new wxStringProperty(field.mTitle, wxPG_LABEL, first_filter_val);  break;
	case ftJSON:	pgp = new wxLongStringProperty(field.mTitle, wxPG_LABEL, first_filter_val);  break;
	default:break;
	}
	//if (pgp)
	//	pgp->SetValueToUnspecified();
	return pgp;
}
//-----------------------------------------------------------------------------
bool FilterArrayEditor::GetFilterValue(wh::Field& field)
{
	field.mFilter.clear();
	auto pgp = mPropGrid->GetProperty(field.mTitle);
	
	if (!pgp || pgp->IsValueUnspecified())
		return false;

	if (ftLong == field.mType && field.mEditor == FieldEditor::Type)
	{
		int cs = pgp->GetChoiceSelection();
		int ftype = pgp->GetChoices().GetValue(cs);
		if (-1 != ftype)
			field.mFilter.emplace_back(wxString::Format("%d", ftype));
	}
	else
	{
		wxString gui_value = pgp->GetValueAsString().Trim().Trim(false);
		if(!gui_value.IsEmpty())
			if (ftName == field.mType || ftText == field.mType)
				field.mFilter.emplace_back(gui_value, foLike);
			else
				field.mFilter.emplace_back(gui_value);
	}
		
	return field.mFilter.size() && !field.mFilter[0].mVal.IsEmpty();
}
//-----------------------------------------------------------------------------
void FilterArrayEditor::OnFieldAfterInsert(const IModel& vec
	, const std::vector<SptrIModel>& newItems, const SptrIModel& itemBefore)
{
	auto field_before = std::dynamic_pointer_cast<wh::Field>(itemBefore);
	wxPGProperty* pgp_before = nullptr;
	if (field_before)
		pgp_before = mPropGrid->GetProperty(field_before->mTitle);
	
	for (const auto& new_ifield : newItems)
	{
		auto new_field = std::dynamic_pointer_cast<ITableField>(new_ifield);
		if (new_field)
		{
			wxPGProperty* new_pgp = MakeProperty(new_field->GetData());
			if (new_pgp)
			{
				if (pgp_before)
					pgp_before = mPropGrid->Insert(pgp_before, new_pgp);
				else
					mPropGrid->Append(new_pgp);
			}
				
		}
	}
}
//-----------------------------------------------------------------------------
void FilterArrayEditor::OnFieldBeforeRemove(const IModel& vec
	, const std::vector<SptrIModel>& remVec)
{
	for (const auto& del_ifield : remVec)
	{
		auto del_field = std::dynamic_pointer_cast<ITableField>(del_ifield);
		if (del_field)
			mPropGrid->DeleteProperty(del_field->GetData().mTitle);
	}
}
//-----------------------------------------------------------------------------
void FilterArrayEditor::OnFieldInfoChange(const IModel& newVec
	, const std::vector<unsigned int>& itemVec)
{
	for (const auto& pos : itemVec)
	{
		wxPGProperty* old_pgp = nullptr;
		auto old_it = mPropGrid->GetVIterator(wxPG_ITERATE_PROPERTIES);
		for (size_t i = 0; i < pos; i++)
			old_it.Next();
		old_pgp = old_it.GetProperty();
		
		auto inew_field = newVec.GetChild(pos);
		if (inew_field && old_pgp)
		{
			auto edit_field = std::dynamic_pointer_cast<ITableField>(inew_field);
			if (edit_field)
			{
				const auto& new_field_data = edit_field->GetData();
				
				bool name_change = new_field_data.mTitle != old_pgp->GetName();
				bool val_change = new_field_data.mFilter.size() ?
					new_field_data.mFilter[0] != old_pgp->GetValueAsString()
					: !old_pgp->GetValueAsString().IsEmpty();
				if (name_change || val_change)
				{
					wxPGProperty* new_pgp = MakeProperty(new_field_data);
					if (new_pgp)
					{
						mPropGrid->ReplaceProperty(old_pgp, new_pgp);
						if (new_field_data.mFilter.size())
							new_pgp->SetValueFromString(new_field_data.mFilter[0].mVal);
					}
						
				}
			
			}//if (edit_field)
		}
	}

}