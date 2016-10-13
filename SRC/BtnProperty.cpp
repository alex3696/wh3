#include "_pch.h"
#include "BtnProperty.h"


/// BtnStringEditor 
class BtnStringEditor : public wxPGTextCtrlEditor
{
public:
	DECLARE_DYNAMIC_CLASS(BtnStringEditor)

	typedef std::function<bool(wxPGProperty*)>	OnBtnFunc;

	BtnStringEditor(){}
	virtual ~BtnStringEditor() {}
	virtual wxPGWindowList CreateControls(wxPropertyGrid* propGrid,
		wxPGProperty* property,
		const wxPoint& pos,
		const wxSize& sz) const override
	{
		bool is_read_only = property->HasFlag(wxPG_PROP_READONLY) != 0;//(property->GetFlags() & wxPG_PROP_READONLY)!=0;

		auto btnprop = dynamic_cast<BtnProperty*>(property);

		// Create and populate buttons-subwindow
		wxPGMultiButton* buttons = new wxPGMultiButton(propGrid, sz);
		
		// Add one regular buttons
		if (btnprop->GetButtonBitmap() && btnprop->GetButtonBitmap()->IsOk())
			buttons->Add(*btnprop->GetButtonBitmap());
		else
			buttons->Add(btnprop->GetButtonTitle());


		if (is_read_only && !btnprop->GetForceEnableButton())
		{
			buttons->Disable();
			auto btn0 = buttons->GetButton(0);
			if (btn0)
				btn0->Disable();
		}

		// Create the 'primary' editor control (textctrl in this case)
		property->ChangeFlag(wxPG_PROP_READONLY, true);  // wxPG_EDITABLE_VALUE
		wxPGWindowList wndList = wxPGTextCtrlEditor::CreateControls(propGrid, property, pos, buttons->GetPrimarySize());
		property->ChangeFlag(wxPG_PROP_READONLY, is_read_only);

		// Finally, move buttons-subwindow to correct position and make sure returned wxPGWindowList contains our custom button list.
		buttons->Finalize(propGrid, pos);
		wndList.SetSecondary(buttons);
		return wndList;
	}
	virtual bool OnEvent(wxPropertyGrid* propGrid,
		wxPGProperty* prop,
		wxWindow* ctrl,
		wxEvent& evt) const override
	{
		if (evt.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED)
		{
			wxPGMultiButton* buttons = (wxPGMultiButton*)propGrid->GetEditorControlSecondary();

			if (evt.GetId() == buttons->GetButtonId(0))
			{
				auto bprop = dynamic_cast<BtnProperty*>(prop);
				if (bprop && bprop->mFunc)
				{
					OnBtnFunc& f = bprop->mFunc;
					return f(prop);
				}
				return false;

				//auto it = mBtnPropMap.find(prop);
				//if (mBtnPropMap.end() != it)
				//	return (it->second) ? it->second(prop) : false;


			}//if (evt.GetId() == buttons->GetButtonId(0))
		}//if (evt.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED)
		return false;
		// всё что не выбрано кнопкой "..." не принимаем return wxPGTextCtrlEditor::OnEvent(propGrid, property, ctrl, event);	
	}


};



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
	delete mEditor;
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
//-------------------------------------------------------------------------------------------
void BtnProperty::ForceEnableButton(bool enable)
{
	mForceEnable = true;
	this->SetEditor("BtnStringEditor");

	/*
	auto old_erditor = this->GetEditorClass();
	if (old_erditor != gBtnStringEditor)
		delete old_erditor;
	mEditor = new BtnStringEditor;
	this->SetEditor(mEditor);
	*/
}
//-------------------------------------------------------------------------------------------

void BtnProperty::SetButtonBitmap(const wxBitmap& bmp)
{
	mBmp = bmp;
	auto old_erditor = this->GetEditorClass();
	this->SetEditor("BtnStringEditor");
	/*
	if (old_erditor != gBtnStringEditor)
		delete old_erditor;
	mEditor = new BtnStringEditor;
	this->SetEditor(mEditor);
	*/
}
//-------------------------------------------------------------------------------------------

void BtnProperty::SetButtonTitle(const wxString& str)
{
	mTitle = str;
	auto old_erditor = this->GetEditorClass();
	this->SetEditor("BtnStringEditor");
	/*
	if (old_erditor != gBtnStringEditor)
		delete old_erditor;
	mEditor = new BtnStringEditor;
	this->SetEditor(mEditor);
	*/
}
//-------------------------------------------------------------------------------------------



