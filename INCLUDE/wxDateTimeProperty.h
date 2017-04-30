#ifndef __WXDATETIMEPROPERTY_H
#define __WXDATETIMEPROPERTY_H
//-----------------------------------------------------------------------------
#include "_pch.h"

//using whModelFilter = wh::ModelFilter;
//WX_PG_DECLARE_VARIANT_DATA(whModelFilter)
//-----------------------------------------------------------------------------
class wxDateTimeProperty : public wxPGProperty
{
	WX_PG_DECLARE_PROPERTY_CLASS(wxPGPFilterProperty)
public:

	wxDateTimeProperty(const wxString& label = wxPG_LABEL,
		const wxString& name = wxPG_LABEL,
		const wxDateTime& value = wxDateTime());
	virtual ~wxDateTimeProperty();

	virtual wxVariant ChildChanged(wxVariant& thisValue,
		int childIndex, wxVariant& childValue) const;
	virtual void RefreshChildren();
protected:
	//virtual wxString  ValueToString(wxVariant &  value, int  argFlags = 0)  const;
};
//-----------------------------------------------------------------------------


#endif // __****_H