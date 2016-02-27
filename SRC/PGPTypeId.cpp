#include "_pch.h"
#include "PGPTypeId.h"
#include "dbFieldType.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPGPSmallType, wxEnumProperty)
WX_PG_IMPLEMENT_PROPERTY_CLASS_PLAIN(wxPGPSmallType, long, Choice)

wxPGPSmallType::wxPGPSmallType(const wxString&	label, const wxString& name, int value)
:wxEnumProperty(label, name)
{
	wxPGChoices soc;
	for (const auto& ft : wh::gFieldTypeArray)
		soc.Add(ft.mTitle, ft.mType);
	this->SetChoices(soc);
	if (soc.GetCount())
		this->SetChoiceSelection(value);
}