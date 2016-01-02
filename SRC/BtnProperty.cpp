#include "_pch.h"
#include "BtnProperty.h"




IMPLEMENT_DYNAMIC_CLASS(BtnStringEditor, wxPGTextCtrlEditor)




//-------------------------------------------------------------------------------------------------
BtnStringEditor* gBtnStringEditor = NULL;


BtnProperty::BtnProperty(const wxString& label, const wxString& name, const wxString& value)
//: wxPGProperty(label, name)
: wxStringProperty(label, name, value)
{
	SetValue(WXVARIANT(value));


	if (!gBtnStringEditor)
	{
		gBtnStringEditor = new BtnStringEditor;
		wxPropertyGrid::RegisterEditorClass(gBtnStringEditor);
	}

	this->SetEditor("BtnStringEditor");
}
//-------------------------------------------------------------------------------------------------
BtnProperty::~BtnProperty()
{
	//if (gBtnStringEditor)
	//	gBtnStringEditor->mBtnPropMap.erase(this);
}
//-------------------------------------------------------------------------------------------
void BtnProperty::SetOnClickButonFunc(std::function<bool(wxPGProperty*)>& func)
{
	mFunc = func;
	//if (gBtnStringEditor)
	//	gBtnStringEditor->mBtnPropMap[this] = func;
}
