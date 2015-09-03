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
		int childIndex, wxVariant& childValue) const;
	virtual void RefreshChildren();

protected:
};


//-----------------------------------------------------------------------------
using wh_rec_ObjParent = wh::rec::ObjParent;
WX_PG_DECLARE_VARIANT_DATA(wh_rec_ObjParent)
/// PG Свойство - родитель-объект 
class wxObjParentProperty : public wxPGProperty
{
	WX_PG_DECLARE_PROPERTY_CLASS(wxObjParentProperty)
public:

	wxObjParentProperty(const wxString& label = wxPG_LABEL,
		const wxString& name = wxPG_LABEL,
		const wh_rec_ObjParent& value = wh_rec_ObjParent());
	virtual ~wxObjParentProperty();

	virtual wxVariant ChildChanged(wxVariant& thisValue,
		int childIndex, wxVariant& childValue) const;
	virtual void RefreshChildren();

	virtual wxString  ValueToString(wxVariant &  value, int  argFlags = 0)  const;
};

//-----------------------------------------------------------------------------

using wh_rec_Cls = wh::rec::Cls;
WX_PG_DECLARE_VARIANT_DATA(wh_rec_Cls)

class wxClsProperty : public wxPGProperty
{
	WX_PG_DECLARE_PROPERTY_CLASS(wxClsProperty)
public:

	wxClsProperty(const wxString& label = wxPG_LABEL,
		const wxString& name = wxPG_LABEL,
		const wh_rec_Cls& value = wh_rec_Cls());
	virtual ~wxClsProperty();

	virtual wxVariant ChildChanged(wxVariant& thisValue,
		int childIndex, wxVariant& childValue) const;
	virtual void RefreshChildren();

	virtual wxString  ValueToString(wxVariant &  value, int  argFlags = 0)  const;
protected:
};

//-----------------------------------------------------------------------------

using wh_rec_ObjTitle = wh::rec::ObjTitle;
WX_PG_DECLARE_VARIANT_DATA(wh_rec_ObjTitle)

class wxObjTitleProperty : public wxPGProperty
{
	WX_PG_DECLARE_PROPERTY_CLASS(wxClsProperty)
public:

	wxObjTitleProperty(const wxString& label = wxPG_LABEL,
		const wxString& name = wxPG_LABEL,
		const wh_rec_ObjTitle& value = wh_rec_ObjTitle());
	virtual ~wxObjTitleProperty();

	virtual wxVariant ChildChanged(wxVariant& thisValue,
		int childIndex, wxVariant& childValue) const;
	virtual void RefreshChildren();

	virtual wxString  ValueToString(wxVariant &  value, int  argFlags = 0)  const;
protected:
};


//-----------------------------------------------------------------------------

class wxRegExpValidator : public wxValidator
{
public:
	wxRegExpValidator(const wxString& regexp);
	wxRegExpValidator(const wxRegExpValidator& tocopy);
	virtual wxObject* Clone() const { return new wxRegExpValidator(*this); }

	virtual bool Validate(wxWindow* parent);

	// Called to transfer data to the window
	//virtual bool TransferToWindow();

	// Called to transfer data from the window
	//virtual bool TransferFromWindow();

protected:
	wxRegEx		mRegExp;
	wxString	mRegStr;
};

//-----------------------------------------------------------------------------

#endif //__*_H
