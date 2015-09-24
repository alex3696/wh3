#include "_pch.h"
#include "PGClsPid.h"

using namespace wh;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ(wh_rec_Base)

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxClsParentProperty, wxPGProperty,
								wh_rec_Base, const wh_rec_Base&, TextCtrl)

//-----------------------------------------------------------------------------
wxClsParentProperty::wxClsParentProperty(const wxString& label,
const wxString& name, const wh_rec_Base& value)
	: wxPGProperty(label, name)
{
	SetValue(WXVARIANT(value));
	AddPrivateChild(new wxStringProperty("Имя", wxPG_LABEL, value.mLabel));
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
	Item(1)->SetValue(WXVARIANT((wxString)parent.mId));
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



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ(wh_rec_ObjParent)
WX_PG_IMPLEMENT_PROPERTY_CLASS(wxObjParentProperty, wxPGProperty,
								wh_rec_ObjParent, const wh_rec_ObjParent&, TextCtrl)

//-----------------------------------------------------------------------------
wxObjParentProperty::wxObjParentProperty(const wxString& label,
const wxString& name, const wh_rec_ObjParent& value)
: wxPGProperty(label, name)
{
	SetValue(WXVARIANT(value));
	AddPrivateChild(new wxClsParentProperty("Класс", wxPG_LABEL, wh_rec_Base() ));
	AddPrivateChild(new wxClsParentProperty("Объект", wxPG_LABEL, wh_rec_Base() ));
}
//-----------------------------------------------------------------------------
wxObjParentProperty::~wxObjParentProperty() { }
//-----------------------------------------------------------------------------
void wxObjParentProperty::RefreshChildren()
{
	if (!GetChildCount()) return;
	const wh_rec_ObjParent& parent = wh_rec_ObjParentRefFromVariant(m_value);
	Item(0)->SetValue(WXVARIANT(parent.mCls));
	Item(1)->SetValue(WXVARIANT(parent.mObj));
}
//-----------------------------------------------------------------------------
wxVariant wxObjParentProperty::ChildChanged(wxVariant& thisValue,
	int childIndex,
	wxVariant& childValue) const
{
	wh_rec_ObjParent parent;
	parent << thisValue;
	switch (childIndex)
	{
	case 0: parent.mCls = wh_rec_BaseRefFromVariant(childValue); break;
	case 1: parent.mObj = wh_rec_BaseRefFromVariant(childValue); break;
	}
	wxVariant newVariant;
	newVariant << parent;
	return newVariant;
}
//-----------------------------------------------------------------------------
wxString  wxObjParentProperty::ValueToString(wxVariant &  value, int  argFlags)  const

{
	const auto& obj = wh_rec_ObjParentRefFromVariant(m_value);

	#ifdef _DEBUG
	return wxString::Format("[%s]%s pid=%s", 
		obj.mCls.mLabel, obj.mObj.mLabel, obj.mObj.mId.SqlVal());
	#else
	return wxString::Format("[%s]%s", obj.mCls.mLabel, obj.mObj.mLabel);
	#endif
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ(wh_rec_Cls)

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxClsProperty, wxPGProperty,
wh_rec_Cls, const wh_rec_Cls&, TextCtrl)

//-----------------------------------------------------------------------------
wxClsProperty::wxClsProperty(const wxString& label,
const wxString& name, const wh_rec_Cls& value)
: wxPGProperty(label, name)
{
	SetValue(WXVARIANT(value));

	wxPGChoices soc;
	soc.Add(L"Абстрактный", 0);
	soc.Add(L"Номерной", 1);
	soc.Add(L"Количественный(целочисленный)", 2);
	soc.Add(L"Количественный(дробный)", 3);

	AddPrivateChild(new wxStringProperty("Имя"));
	AddPrivateChild(new wxStringProperty(L"Ед.измерений"));
	AddPrivateChild(new wxEnumProperty(L"Тип экземпляров", wxPG_LABEL, soc, 0));
	AddPrivateChild(new wxLongStringProperty(L"Описание"));
	AddPrivateChild(new wxStringProperty("#"));
	//AddPrivateChild(new wxStringProperty(L"VID"));
	AddPrivateChild(new wxClsParentProperty(L"Родительский класс"));
}
//-----------------------------------------------------------------------------
wxClsProperty::~wxClsProperty() { }
//-----------------------------------------------------------------------------
void wxClsProperty::RefreshChildren()
{
	if (!GetChildCount()) return;
	const wh_rec_Cls& cls = wh_rec_ClsRefFromVariant(m_value);
	Item(0)->SetValue(WXVARIANT(cls.mLabel));
	
	Item(1)->SetValue(WXVARIANT(cls.mMeasure));
	
	unsigned long items_type = 0;
	cls.mType.ToULong(&items_type);
	Item(2)->SetChoiceSelection(items_type);
	
	Item(3)->SetValue(WXVARIANT(cls.mComment));
	Item(4)->SetValue(WXVARIANT(cls.mID));
	Item(5)->SetValue(WXVARIANT(cls.mParent));
}
//-----------------------------------------------------------------------------
wxVariant wxClsProperty::ChildChanged(wxVariant& thisValue,
	int childIndex,
	wxVariant& childValue) const
{
	wh_rec_Cls cls;
	cls << thisValue;
	switch (childIndex)
	{
	default: break; 
	case 0: cls.mLabel = childValue.GetString(); break;
	case 1: cls.mMeasure = childValue.GetString(); break;
	case 2: cls.mType = childValue.GetString(); break;
	case 3: cls.mComment = childValue.GetString(); break;
	case 4: cls.mID = childValue.GetString(); break;
	case 5: cls.mParent = wh_rec_BaseRefFromVariant(childValue); break;
	}
	wxVariant newVariant;
	newVariant << cls;
	return newVariant;
}
//-----------------------------------------------------------------------------
wxString  wxClsProperty::ValueToString(wxVariant &  value, int  argFlags)  const

{
	const wh_rec_Cls& cls = wh_rec_ClsRefFromVariant(value);

	return wxString::Format("%s (%s) [%s]"
		,cls.mLabel
		,cls.mMeasure
		,cls.mID);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ(wh_rec_ObjTitle)

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxObjTitleProperty, wxPGProperty,
wh_rec_ObjTitle, const wh_rec_ObjTitle&, TextCtrl)

//-----------------------------------------------------------------------------
wxObjTitleProperty::wxObjTitleProperty(const wxString& label,
const wxString& name, const wh_rec_ObjTitle& value)
: wxPGProperty(label, name)
{
	SetValue(WXVARIANT(value));

	AddPrivateChild(new wxStringProperty("Имя"));
	AddPrivateChild(new wxStringProperty(L"Количество"));
	AddPrivateChild(new wxStringProperty("#"));
	AddPrivateChild(new wxObjParentProperty("Местоположение"));
}
//-----------------------------------------------------------------------------
wxObjTitleProperty::~wxObjTitleProperty() { }
//-----------------------------------------------------------------------------
void wxObjTitleProperty::RefreshChildren()
{
	if (!GetChildCount()) return;
	const wh_rec_ObjTitle& obj = wh_rec_ObjTitleRefFromVariant(m_value);
	Item(0)->SetValue(WXVARIANT(obj.mLabel));
	Item(1)->SetValue(WXVARIANT(obj.mQty));
	Item(2)->SetValue(WXVARIANT(obj.mID));
	Item(3)->SetValue(WXVARIANT(wh_rec_ObjParent(wxEmptyString, wxEmptyString, 
		obj.mPID, wxEmptyString)));
}
//-----------------------------------------------------------------------------
wxVariant wxObjTitleProperty::ChildChanged(wxVariant& thisValue,
	int childIndex,
	wxVariant& childValue) const
{
	wh_rec_ObjTitle obj;
	obj << thisValue;
	switch (childIndex)
	{
	default: break;
	case 0: obj.mLabel = childValue.GetString(); break;
	case 1: obj.mQty = childValue.GetString(); break;
	case 2: obj.mID = childValue.GetString(); break;
	case 3: obj.mPID = wh_rec_ObjParentRefFromVariant(m_value).mObj.mId; break;
	}
	wxVariant newVariant;
	newVariant << obj;
	return newVariant;
}
//-----------------------------------------------------------------------------
wxString  wxObjTitleProperty::ValueToString(wxVariant &  value, int  argFlags)  const

{
	const auto& obj = wh_rec_ObjTitleRefFromVariant(value);

	return wxString::Format("%s (кол-во %s) [%s]"
		, obj.mLabel
		, obj.mQty
		, obj.mID);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
wxRegExpValidator::wxRegExpValidator(const wxString& regstr)
	:wxValidator(), mRegExp(regstr), mRegStr(regstr)
{
}
//-----------------------------------------------------------------------------
wxRegExpValidator::wxRegExpValidator(const wxRegExpValidator& tocopy)
{ 
	mRegStr = tocopy.mRegStr;
	mRegExp.Compile(mRegStr, wxRE_DEFAULT);
}
//-----------------------------------------------------------------------------
bool wxRegExpValidator::Validate(wxWindow *WXUNUSED(parent))
{
	wxTextCtrl* tc = wxDynamicCast(GetWindow(), wxTextCtrl);
	wxCHECK_MSG(tc, true, wxT("validator window must be wxTextCtrl"));

	const wxString& val = tc->GetValue();

	if (mRegExp.IsValid() && mRegExp.Matches(val))
		return true;
	return false;
}

wxRegExpValidator titleValidator("^([[:alnum:][:space:]!()*+,-.:;<=>^_|№])+$"); //wxRE_DEFAULT

//-----------------------------------------------------------------------------
/*
bool wxRegExpValidator::TransferToWindow()
{
	wxASSERT(GetWindow()->IsKindOf(CLASSINFO(wxPGProperty)));

	if (mVar)
	{
		wxPGProperty* p = (wxPGProperty*)GetWindow();
		if (!p)
			return false;

		p->SetValueFromString(*mVar);
	}

	return true;
}
//-----------------------------------------------------------------------------
bool wxRegExpValidator::TransferFromWindow()
{
	wxASSERT(GetWindow()->IsKindOf(CLASSINFO(wxPGProperty)));

	if (mVar)
	{
		wxPGProperty* p = (wxPGProperty*)GetWindow();
		if (!p)
			return false;

		*mVar = p->GetValueAsString();
	}

	return true;
}
*/