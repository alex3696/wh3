#include "_pch.h"
#include "whPGFileLinkProperty.h"


/// BtnStringEditor 
class PGFileLinkEditor : public wxPGTextCtrlEditor
{
public:
	DECLARE_DYNAMIC_CLASS(PGFileLinkEditor)

	typedef std::function<bool(wxPGProperty*)>	OnBtnFunc;

	PGFileLinkEditor(){}
	virtual ~PGFileLinkEditor() {}
	virtual wxPGWindowList CreateControls(wxPropertyGrid* propGrid,
		wxPGProperty* property,
		const wxPoint& pos,
		const wxSize& sz) const override
	{
		bool is_read_only = property->HasFlag(wxPG_PROP_READONLY) != 0;//(property->GetFlags() & wxPG_PROP_READONLY)!=0;

		auto btnprop = dynamic_cast<whPGFileLinkProperty*>(property);

		// Create and populate buttons-subwindow
		wxPGMultiButton* buttons = new wxPGMultiButton(propGrid, sz);

		// Add regular buttons
		buttons->Add(wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_MENU));
		buttons->Add(wxString("..."));

		auto btn0 = buttons->GetButton(0);
		auto btn1 = buttons->GetButton(1);

		btn0->SetToolTip("Открыть");
		btn1->SetToolTip("Выбрать");
		if (is_read_only)
			btn1->Disable();

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
			whPGFileLinkProperty* bprop = dynamic_cast<whPGFileLinkProperty*>(prop);
			if (!buttons || !bprop)
				return false;

			const std::function<bool(wxPGProperty*)>* func = nullptr;
			if (evt.GetId() == buttons->GetButtonId(1))
				func = &bprop->GetOpenFunc();
			else if (evt.GetId() == buttons->GetButtonId(0))
				func = &bprop->GetExecFunc();
			if (func && func->operator bool() )
				return (*func)(prop);
			return false;
		}//if (evt.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED)
		return false;
	}


};


class whPGFileLinkProperty::LinkImpl
{
public:
	std::function<bool(wxPGProperty*)> funcOpen = nullptr;
	std::function<bool(wxPGProperty*)> funcExec = nullptr;
};




IMPLEMENT_DYNAMIC_CLASS(PGFileLinkEditor, wxPGTextCtrlEditor)




//-------------------------------------------------------------------------------------------------
PGFileLinkEditor* gPGFileLinkEditor = NULL;

whPGFileLinkProperty::whPGFileLinkProperty(const wxString& label, const wxString& name, const wxString& value)
: wxStringProperty(label, name, value)
{
	SetValue(WXVARIANT(value));

	mImpl.reset(new LinkImpl);

	mImpl->funcOpen = [this](wxPGProperty* prop)->bool
	{ 
		wxFileDialog openFileDialog(nullptr);
		if (openFileDialog.ShowModal() == wxID_CANCEL)        
			return false;
		prop->SetValueFromString (openFileDialog.GetPath());
		return true; 
	};

	mImpl->funcExec = [this](wxPGProperty* prop)->bool
	{
		auto str = prop->GetValueAsString();
		if (str.IsEmpty())
			return false;
		wxBusyCursor			busyCursor;
		// win execute
		HWND hwnd = nullptr;
		LPCWSTR path = str.operator const wchar_t *();
		ShellExecuteW(hwnd, L"open", path, L"", NULL, SW_SHOWNORMAL);
		//wxWidget
		//wxShell(str);
		// std
		//#include "unistd.h"
		//execl(prop->GetValueAsString().c_str(),nullptr);
		return true;
	};

	if (!gPGFileLinkEditor)
	{
		gPGFileLinkEditor = new PGFileLinkEditor;
		wxPropertyGrid::RegisterEditorClass(gPGFileLinkEditor);
	}
	this->SetEditor("PGFileLinkEditor");
}
//-------------------------------------------------------------------------------------------

whPGFileLinkProperty::~whPGFileLinkProperty()
{
}
//-------------------------------------------------------------------------------------------

void whPGFileLinkProperty::SetOpenFunc(std::function<bool(wxPGProperty*)>& func)
{
	mImpl->funcOpen = func;
}
//-------------------------------------------------------------------------------------------

void whPGFileLinkProperty::SetExecFunc(std::function<bool(wxPGProperty*)>& func)
{
	mImpl->funcExec = func;
}
//-------------------------------------------------------------------------------------------

const std::function<bool(wxPGProperty*)>& whPGFileLinkProperty::GetOpenFunc()const
{
	return mImpl->funcOpen;
}

//-------------------------------------------------------------------------------------------
const std::function<bool(wxPGProperty*)>& whPGFileLinkProperty::GetExecFunc()const
{
	return mImpl->funcExec; 
}
