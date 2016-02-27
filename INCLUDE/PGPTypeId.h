#ifndef __PGPTYPEID_H
#define __PGPTYPEID_H

#include "_pch.h"
//-----------------------------------------------------------------------------

class wxPGPSmallType : public wxEnumProperty
{
	WX_PG_DECLARE_PROPERTY_CLASS(wxPGPSmallType)
public:

	wxPGPSmallType(const wxString&	label = wxPG_LABEL,
		const wxString& name = wxPG_LABEL,
		int value = 0);
};









#endif //__*_H