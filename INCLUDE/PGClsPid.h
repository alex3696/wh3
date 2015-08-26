#ifndef __PGCLSPID_H
#define __PGCLSPID_H

#include "db_rec.h"

using wh_rec_Base = wh::rec::Base;
WX_PG_DECLARE_VARIANT_DATA(wh_rec_Base)
//-----------------------------------------------------------------------------
class wxClsParentProperty : public wxPGProperty
{
	WX_PG_DECLARE_PROPERTY_CLASS(wxClsParentProperty)
public:

	wxClsParentProperty(const wxString& label = wxPG_LABEL,
		const wxString& name = wxPG_LABEL,
		const wh_rec_Base& value = wh_rec_Base());
	virtual ~wxClsParentProperty();

	virtual wxVariant ChildChanged(wxVariant& thisValue,
		int childIndex,
		wxVariant& childValue) const;
	virtual void RefreshChildren();

protected:
};

//-----------------------------------------------------------------------------
#endif //__*_H
