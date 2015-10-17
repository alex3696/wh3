#ifndef __BTNPROPERTY_H
#define __BTNPROPERTY_H

#include "BaseOkCancelDialog.h"

//-------------------------------------------------------------------------------------------------
/// BtnStringEditor 
class BtnStringEditor : public wxPGTextCtrlEditor
{
public:
	DECLARE_DYNAMIC_CLASS(BtnStringEditor)

	typedef std::function<bool(wxPGProperty*)>	OnBtnFunc;

	std::map<wxPGProperty*, OnBtnFunc>			mBtnPropMap;



	std::function<bool(wxPGProperty*)> mFuncOnBtn;
	BtnStringEditor(){}
	virtual ~BtnStringEditor() {}
	virtual wxPGWindowList CreateControls(wxPropertyGrid* propGrid,
		wxPGProperty* property,
		const wxPoint& pos,
		const wxSize& sz) const override
	{
		// Create and populate buttons-subwindow
		wxPGMultiButton* buttons = new wxPGMultiButton(propGrid, sz);
		// Add two regular buttons
		buttons->Add("...");
		// Create the 'primary' editor control (textctrl in this case)
		wxPGWindowList wndList = wxPGTextCtrlEditor::CreateControls(propGrid, property, pos, buttons->GetPrimarySize());
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
				auto it = mBtnPropMap.find(prop);
				if (mBtnPropMap.end() != it)
					return (it->second) ? it->second(prop) : false;
			}//if (evt.GetId() == buttons->GetButtonId(0))
		}//if (evt.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED)
		return false;
		// всё что не выбрано кнопкой "..." не принимаем return wxPGTextCtrlEditor::OnEvent(propGrid, property, ctrl, event);	
	}

};


//-------------------------------------------------------------------------------------------------
/// Свойство с кнопкой "..."
class BtnProperty 
	: public wxPGProperty 
	//: public wxStringProperty
	
{
	//WX_PG_DECLARE_PROPERTY_CLASS(BtnProperty)
public:

	BtnProperty(const wxString& label = wxPG_LABEL,
		const wxString& name = wxPG_LABEL,
		const wxString& value = wxEmptyString);
	~BtnProperty();

	void SetOnClickButonFunc(std::function<bool(wxPGProperty*)>& func);
};








#endif //__*_H

