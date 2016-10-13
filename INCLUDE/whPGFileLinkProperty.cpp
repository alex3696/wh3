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
		buttons->Add(wxString("..."));
		buttons->Add(wxArtProvider::GetBitmap(wxART_EXECUTABLE_FILE, wxART_BUTTON));

		if (is_read_only )
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
			whPGFileLinkProperty* bprop = dynamic_cast<whPGFileLinkProperty*>(prop);
			if (!buttons || !bprop)
				return false;

			const std::function<bool(wxPGProperty*)>* func = nullptr;
			if (evt.GetId() == buttons->GetButtonId(0))
				func = &bprop->GetOpenFunc();
			else if (evt.GetId() == buttons->GetButtonId(1))
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
#include "unistd.h"

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
		wxBusyCursor			busyCursor;
		//execl(prop->GetValueAsString().c_str(),nullptr);
		HWND hwnd=nullptr;
		auto str = prop->GetValueAsString();
		//str = "\\\\dlserver\\Arhiv_SC\\_������_���.��\\_������_��_�����\\2016\\2016_09_15 �����. ��� � ���\\���� ���_2017 ���������� 15.09.2016.xlsx";
		LPCWSTR path = str.operator const wchar_t *();
		ShellExecuteW(hwnd, L"open", path,L"", NULL, SW_SHOWNORMAL);
		//wxShell(str);
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
