#include "_pch.h"
#include "wxDateTimeProperty.h"


WX_PG_IMPLEMENT_PROPERTY_CLASS(wxDateTimeProperty, wxPGProperty,
	wxDateTime, const wxDateTime&, TextCtrl)
	//-----------------------------------------------------------------------------
	wxDateTimeProperty::wxDateTimeProperty(const wxString& label,
	const wxString& name, const wxDateTime& value)
	:wxPGProperty(label, name)
{
	if (value.IsValid())
		SetValue(WXVARIANT(value));
	else
		SetValue(WXVARIANT(wxDateTime::Now()));

	wxDateTime dt = m_value.GetDateTime();

	auto pgp_date = new wxDateProperty("Дата", wxPG_LABEL, dt);
	auto pgp_time = new wxStringProperty("Время", wxPG_LABEL, dt.FormatTime());
	pgp_date->SetAttribute(wxPG_DATE_PICKER_STYLE, (long)(wxDP_DROPDOWN | wxDP_SHOWCENTURY));
	const wxString format = wxLocale::GetOSInfo(wxLOCALE_SHORT_DATE_FMT, wxLOCALE_CAT_DATE);
	pgp_date->SetAttribute(wxPG_DATE_FORMAT, format);

	AddPrivateChild(pgp_date);
	AddPrivateChild(pgp_time);

}
//-----------------------------------------------------------------------------
wxDateTimeProperty::~wxDateTimeProperty() { }
//-----------------------------------------------------------------------------
void wxDateTimeProperty::RefreshChildren()
{
	if (!GetChildCount() || "datetime" != m_value.GetType())
		return;
	wxDateTime dt = m_value.GetDateTime();
	wxString format_dt = wxLocale::GetInfo(wxLOCALE_DATE_TIME_FMT);
	wxString ret = dt.Format(format_dt);
	//dt.ResetTime();
	wxString format_t = wxLocale::GetInfo(wxLOCALE_TIME_FMT);
	//dt.FormatTime();

	Item(0)->SetValue(WXVARIANT(dt));
	Item(1)->SetValue(WXVARIANT(dt.Format(format_t)));
}
//-----------------------------------------------------------------------------
wxVariant wxDateTimeProperty::ChildChanged(wxVariant& thisValue,
	int childIndex,
	wxVariant& childValue) const
{
	wxDateTime dt = thisValue.GetDateTime();
	if (!dt.IsValid())
		dt = wxDateTime::Now();

	switch (childIndex)
	{
	case 0:
	{
		wxDateTime dt_date = childValue.GetDateTime();
		if (!dt_date.IsValid())
			dt_date = wxDateTime::Now();

		dt.SetYear(dt_date.GetYear());
		dt.SetMonth(dt_date.GetMonth());
		dt.SetDay(dt_date.GetDay());
	}
	break;
	case 1:
	{
		wxDateTime dt_time = childValue.GetDateTime();
		if (!dt_time.IsValid())
			dt_time.ResetTime();

		dt.SetHour(dt_time.GetHour());
		dt.SetMinute(dt_time.GetMinute());
		dt.SetSecond(dt_time.GetSecond());
		dt.SetMillisecond(dt_time.GetMillisecond());

	}
	break;
	}//switch

	wxVariant newVariant;
	newVariant = dt;
	return newVariant;

}
//-----------------------------------------------------------------------------
/*
wxString  wxDateTimeProperty::ValueToString(wxVariant &  value, int  argFlags)  const
{
if ("datetime" == value.GetType())
{
wxDateTime dt = value.GetDateTime();
if (!dt.IsValid())
return "unknown datetime";

wxString format_dt = wxLocale::GetInfo(wxLOCALE_DATE_TIME_FMT);
wxString ret = dt.Format(format_dt);
return ret;
}
return "unknown type";
}
*/

//-----------------------------------------------------------------------------