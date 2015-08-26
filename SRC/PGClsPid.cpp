#include "_pch.h"
#include "PGClsPid.h"

using namespace wh;

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ(wh_rec_Base)

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxClsParentProperty, wxPGProperty,
rec::Base, const ClsParent&, TextCtrl)

//-----------------------------------------------------------------------------
wxClsParentProperty::wxClsParentProperty(const wxString& label,
const wxString& name, const wh_rec_Base& value)
	: wxPGProperty(label, name)
{
	SetValue(WXVARIANT(value));
	AddPrivateChild(new wxStringProperty("Èìÿ", wxPG_LABEL, value.mLabel));
	AddPrivateChild(new wxStringProperty("#", wxPG_LABEL, value.mId));
}
//-----------------------------------------------------------------------------
wxClsParentProperty::~wxClsParentProperty() { }
//-----------------------------------------------------------------------------
void wxClsParentProperty::RefreshChildren()
{
	if (!GetChildCount()) return;
	const wh_rec_Base& parent = wh_rec_BaseRefFromVariant(m_value);
	Item(0)->SetValue(WXVARIANT(parent.mLabel));
	Item(1)->SetValue(WXVARIANT(parent.mId));
}
//-----------------------------------------------------------------------------
wxVariant wxClsParentProperty::ChildChanged(wxVariant& thisValue,
	int childIndex,
	wxVariant& childValue) const
{
	wh_rec_Base cls_parent;
	cls_parent << thisValue;
	switch (childIndex)
	{
	case 0: cls_parent.mLabel = childValue.GetString(); break;
	case 1: cls_parent.mId = childValue.GetString(); break;
	}
	wxVariant newVariant;
	newVariant << cls_parent;
	return newVariant;
}


