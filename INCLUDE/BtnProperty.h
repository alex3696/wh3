#ifndef __BTNPROPERTY_H
#define __BTNPROPERTY_H

#include "BaseOkCancelDialog.h"




//-------------------------------------------------------------------------------------------------
/// Свойство с кнопкой "..."
class BtnProperty 
	//: public wxPGProperty 
	: public wxStringProperty
	
{
	//WX_PG_DECLARE_PROPERTY_CLASS(BtnProperty)
	
public:
	std::function<bool(wxPGProperty*)> mFunc;

	BtnProperty(const wxString& label = wxPG_LABEL,
		const wxString& name = wxPG_LABEL,
		const wxString& value = wxEmptyString);
	~BtnProperty();

	void SetOnClickButonFunc(std::function<bool(wxPGProperty*)>& func);
};


//-------------------------------------------------------------------------------------------------
/// BtnStringEditor 
class BtnStringEditor : public wxPGTextCtrlEditor
{
public:
	DECLARE_DYNAMIC_CLASS(BtnStringEditor)

	typedef std::function<bool(wxPGProperty*)>	OnBtnFunc;

	//std::map<wxPGProperty*, OnBtnFunc>			mBtnPropMap;

	//std::function<bool(wxPGProperty*)> mFuncOnBtn;

	BtnStringEditor(){}
	virtual ~BtnStringEditor() {}
	virtual wxPGWindowList CreateControls(wxPropertyGrid* propGrid,
		wxPGProperty* property,
		const wxPoint& pos,
		const wxSize& sz) const override
	{
		bool is_read_only = property->HasFlag(wxPG_PROP_READONLY)!=0;//(property->GetFlags() & wxPG_PROP_READONLY)!=0;
		
		// Create and populate buttons-subwindow
		wxPGMultiButton* buttons = new wxPGMultiButton(propGrid, sz);
		buttons->Add("...");// Add one regular buttons
		if (is_read_only)
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





#endif //__*_H

