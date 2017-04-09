#include "_pch.h"
#include "PGClsPid.h"
#include "dlgselectcls_ctrlpnl.h"

using namespace wh;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ(wh_rec_Base)

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxPGPBaseProperty, wxStringProperty,
								wh_rec_Base, const wh_rec_Base&, TextCtrl)


//-----------------------------------------------------------------------------
wxPGPBaseProperty::wxPGPBaseProperty(const wxString& label,
const wxString& name, const wh_rec_Base& value)
	//: wxPGProperty(label, name)
	: BtnProperty(label, name)
{
	SetValue(WXVARIANT(value));

	auto pgp_title = new wxStringProperty("Имя", wxPG_LABEL, value.mLabel);
	auto pgp_id = new wxStringProperty("#", wxPG_LABEL, value.mId);

	pgp_title->ChangeFlag(wxPG_PROP_READONLY, true);
	pgp_id->ChangeFlag(wxPG_PROP_READONLY, true);

	ChangeFlag(wxPG_PROP_READONLY, true);
	
	AddPrivateChild(pgp_title);
	AddPrivateChild(pgp_id);

}
//-----------------------------------------------------------------------------
wxPGPBaseProperty::~wxPGPBaseProperty() { }
//-----------------------------------------------------------------------------
void wxPGPBaseProperty::SetCatalog(const std::shared_ptr<MCat>& cat, bool isTargetObj)
{

	std::function<bool(wxPGProperty*)> selecFunc = [this, cat, isTargetObj](wxPGProperty* prop)
	{
		auto tv = [isTargetObj](const wh::rec::Cls* cls, const wh::rec::Obj* obj)->bool
		{
			return cls && ((isTargetObj) ? obj : true);
		};

		CatDlg dlg(nullptr);
		dlg.SetTargetValidator(tv);
		dlg.SetModel(cat);
		cat->Load();
		

		if (wxID_OK == dlg.ShowModal())
		{
			if (cat->IsObjTree())
			{
				wh::rec::ObjInfo obj;
				if (dlg.GetSelectedObj(obj))
				{
					wh::rec::ObjTitle objTitle;
					objTitle = obj.mObj;
					auto var = WXVARIANT((wh_rec_Base)objTitle);
					SetValueInEvent(var);
					//SetValue(var,0, wxPG_SETVAL_AGGREGATED);
					return true;
				}
			}
			else
			{
				wh::rec::Cls cls;
				if (dlg.GetSelectedCls(cls))
				{
					auto var = WXVARIANT((wh_rec_Base)cls);
					SetValueInEvent(var);
					//SetValue(var,0, wxPG_SETVAL_AGGREGATED);
					return true;
				}
			}
		}

		return false;
	};

	SetOnClickButonFunc(selecFunc);
}
//-----------------------------------------------------------------------------
void wxPGPBaseProperty::RefreshChildren()
{
	if (!GetChildCount()) return;
	const wh_rec_Base& parent = wh_rec_BaseRefFromVariant(m_value);
	Item(0)->SetValue(WXVARIANT(parent.mLabel.toStr() ));
	Item(1)->SetValue(WXVARIANT(parent.mId.toStr() ));

	const wxColour light_red(255, 200, 200);
	if (parent.mLabel.IsNull() || parent.mId.IsNull())
		this->SetBackgroundColour(light_red);
	else
		this->SetBackgroundColour(*wxWHITE);

}
//-----------------------------------------------------------------------------
wxVariant wxPGPBaseProperty::ChildChanged(wxVariant& thisValue,
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
wxString  wxPGPBaseProperty::ValueToString(wxVariant &  value, int  argFlags)  const
{
	if ("wh_rec_Base" == value.GetType())
	{
		const auto& obj = wh_rec_BaseRefFromVariant(m_value);
		return wxString::Format("%s #%s"
			, obj.mLabel.toStr()
			, obj.mId.toStr()
			);
	}
	return "unknown type";
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ(wh_rec_ObjParent)
//WX_PG_IMPLEMENT_PROPERTY_CLASS(wxObjParentProperty, wxPGProperty,
//								wh_rec_ObjParent, const wh_rec_ObjParent&, TextCtrl)
WX_PG_IMPLEMENT_PROPERTY_CLASS(wxObjParentProperty, wxStringProperty,
								wh_rec_ObjParent, const wh_rec_ObjParent&, TextCtrl)
//-----------------------------------------------------------------------------
wxObjParentProperty::wxObjParentProperty(const wxString& label,
const wxString& name, const wh_rec_ObjParent& value)
: BtnProperty(label, name)
{
	SetValue(WXVARIANT(value));
	AddPrivateChild(new wxPGPBaseProperty("Класс", wxPG_LABEL, wh_rec_Base() ));
	AddPrivateChild(new wxPGPBaseProperty("Объект", wxPG_LABEL, wh_rec_Base() ));
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

	return wxString::Format("[%s]%s pid=%s", 
		obj.mCls.mLabel.toStr(),
		obj.mObj.mLabel.toStr(), 
		obj.mObj.mId.toStr()
		);
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

	auto pgp_title = new wxStringProperty("Имя");
	auto pgp_measure = new wxStringProperty(L"Ед.измерений");
	auto pgp_kind = new wxEnumProperty(L"Тип экземпляров", wxPG_LABEL, soc, 0);
	auto pgp_note = new wxLongStringProperty(L"Описание");
	auto pgp_id = new wxStringProperty("#");
	auto pgp_parent = new wxPGPBaseProperty(L"Родительский класс");
	auto pgp_defobj = new wxPGPBaseProperty("Родитель.объект по умолч.");
	
	AddPrivateChild(pgp_title);
	AddPrivateChild(pgp_measure);
	AddPrivateChild(pgp_kind);
	AddPrivateChild(pgp_note);
	AddPrivateChild(pgp_id);
	AddPrivateChild(pgp_parent);
	AddPrivateChild(pgp_defobj);

	//pgp_title->SetValidator(wxRegExpValidator(titleValidator));
	pgp_measure->SetValidator(wxRegExpValidator(titleValidator));

	pgp_id->ChangeFlag(wxPG_PROP_READONLY, true);

	// setup for parent object
	auto parent_obj_cat = std::make_shared<MCat>();
	parent_obj_cat->SetCfg(rec::CatCfg(rec::catObj, false, true));
	parent_obj_cat->SetFilterClsKind(ctQtyByOne, foLess, true);
	pgp_defobj->SetCatalog(parent_obj_cat,true);
	pgp_defobj->ChangeFlag(wxPG_PROP_READONLY, false);

	// setup for parent cls
	auto parent_cls_cat = std::make_shared<MCat>();
	parent_cls_cat->SetCfg(rec::CatCfg(rec::catCls, false, false));
	parent_cls_cat->SetFilterClsKind(ctAbstract, foEq, true);
	pgp_parent->SetCatalog(parent_cls_cat,false);
	pgp_parent->ChangeFlag(wxPG_PROP_READONLY, false);

	//ChangeFlag(wxPG_PROP_COMPOSED_VALUE, true);
	//pgp_parent->ChangeFlag(wxPG_PROP_COMPOSED_VALUE, true);
	//pgp_defobj->ChangeFlag(wxPG_PROP_COMPOSED_VALUE, true);
}
//-----------------------------------------------------------------------------
wxClsProperty::~wxClsProperty() { }
//-----------------------------------------------------------------------------
void wxClsProperty::RefreshChildren()
{
	if (!GetChildCount()) return;
	const wh_rec_Cls& cls = wh_rec_ClsRefFromVariant(m_value);
	
	Item(0)->SetValue(WXVARIANT(cls.mLabel.toStr() ));
	
	Item(1)->SetValue(WXVARIANT(cls.mMeasure.toStr() ));

	Item(2)->SetChoiceSelection(cls.mType.IsNull() ? 0 : (long)cls.mType);
	
	Item(3)->SetValue(WXVARIANT(cls.mComment.toStr()));
	Item(4)->SetValue(WXVARIANT(cls.mId.toStr() ));
	Item(5)->SetValue(WXVARIANT(cls.mParent ));
	Item(6)->SetValue(WXVARIANT(cls.mDefaultObj));

	const wxColour light_red(255, 200, 200);
	
	Item(0)->SetBackgroundColour(cls.mLabel.IsNull() ? light_red : *wxWHITE);
	Item(1)->SetBackgroundColour(cls.mMeasure.IsNull() ? light_red : *wxWHITE);
	//Item(5)->SetBackgroundColour(cls.mParent.mId.IsNull() ? light_red : *wxWHITE);
	//Item(6)->SetBackgroundColour(cls.mDefaultObj.mId.IsNull() ? light_red : *wxWHITE);

}
//-----------------------------------------------------------------------------
wxVariant wxClsProperty::ChildChanged(wxVariant& thisValue,
	int childIndex, wxVariant& childValue) const
{
	wh_rec_Cls cls;
	cls << thisValue;

	wh_rec_Base tmp;
	switch (childIndex)
	{
	default: break; 
	case 0: cls.mLabel = childValue.GetString(); break;
	case 1: cls.mMeasure = childValue.GetString(); break;
	case 2: cls.mType = childValue.GetString(); break;
	case 3: cls.mComment = childValue.GetString(); break;
	case 4: cls.mId = childValue.GetString(); break;
	case 5: cls.mParent = wh_rec_BaseRefFromVariant(childValue);	break;
	case 6: cls.mDefaultObj = wh_rec_BaseRefFromVariant(childValue); break;
	}


	wxVariant newVariant;
	newVariant << cls;
	return newVariant;
}
//-----------------------------------------------------------------------------
wxString  wxClsProperty::ValueToString(wxVariant &  value, int  argFlags)  const

{
	const wh_rec_Cls& cls = wh_rec_ClsRefFromVariant(value);

	return wxString::Format("%s (%s) #%s"
		, cls.mLabel.toStr()
		, cls.mMeasure.toStr()
		, cls.mId.toStr() );
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

	auto pgp_title = new wxStringProperty("Имя");
	auto pgp_qty = new wxFloatProperty("Количество");
	auto pgp_id = new wxStringProperty("#");
	auto pgp_parentobj = new wxPGPBaseProperty("Родитель.объект");
	AddPrivateChild(pgp_title);
	AddPrivateChild(pgp_qty);
	AddPrivateChild(pgp_id);
	AddPrivateChild(pgp_parentobj);
	
	pgp_title->SetValidator(wxRegExpValidator(titleValidator));
	pgp_id->ChangeFlag(wxPG_PROP_READONLY, true);
	
	auto parent_obj_cat = std::make_shared<MCat>();
	parent_obj_cat->SetCfg(rec::CatCfg(rec::catObj, false, true));
	pgp_parentobj->SetCatalog(parent_obj_cat,true);

}
//-----------------------------------------------------------------------------
wxObjTitleProperty::~wxObjTitleProperty() { }
//-----------------------------------------------------------------------------
void wxObjTitleProperty::RefreshChildren()
{
	if (!GetChildCount()) return;
	const wh_rec_ObjTitle& obj = wh_rec_ObjTitleRefFromVariant(m_value);
	Item(0)->SetValue(WXVARIANT(obj.mLabel.toStr()));
	Item(1)->SetValue(WXVARIANT(obj.mQty.toStr()));
	Item(2)->SetValue(WXVARIANT(obj.mId.toStr()));
	Item(3)->SetValue(WXVARIANT(obj.mParent ));

	const wxColour light_red(255, 200, 200);
	Item(0)->SetBackgroundColour(obj.mLabel.IsNull() ? light_red : *wxWHITE);

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
	case 2: obj.mId = childValue.GetString(); break;
	case 3: obj.mParent = wh_rec_BaseRefFromVariant(childValue); break;
	}
	wxVariant newVariant;
	newVariant << obj;
	return newVariant;
}
//-----------------------------------------------------------------------------
wxString  wxObjTitleProperty::ValueToString(wxVariant &  value, int  argFlags)  const

{
	const auto& obj = wh_rec_ObjTitleRefFromVariant(value);

	return wxString::Format("%s (кол-во %s) #%s"
		, obj.mLabel.toStr()
		, obj.mQty.toStr()
		, obj.mId.toStr()
		);
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

	//::wxMessageBox(wxString::Format(wxT("%s is not allowed word"), val.c_str()),
	//	wxT("Validation Failure"));

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