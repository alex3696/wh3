#include "_pch.h"
#include "ViewObjPropList.h"
#include "whPGFileLinkProperty.h"

using namespace wh;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewObjPropList::ViewObjPropList(wxWindow* parent)
{
	wxSizer *mainSz = new wxBoxSizer(wxVERTICAL);
	mPanel = new wxPanel(parent, wxID_ANY);

	mPG = new wxPropertyGrid(mPanel, wxID_ANY
		, wxDefaultPosition, wxDefaultSize
		, wxPG_SPLITTER_AUTO_CENTER);
	mPG->CenterSplitter(true);
	mainSz->Add(mPG, 1, wxALL | wxEXPAND, 0);
	
	mPanel->SetSizer(mainSz);
	mPanel->Layout();
}
//-----------------------------------------------------------------------------
ViewObjPropList::ViewObjPropList(std::shared_ptr<IViewWindow> parent)
	:ViewObjPropList(parent->GetWnd())
{
}
//-----------------------------------------------------------------------------

wxWindow* ViewObjPropList::GetWnd()const
{
	return mPanel;
}
//-----------------------------------------------------------------------------
//virtual 
void ViewObjPropList::SetPropList(const PropValTable& rt, const IAct* act) //override;
{
	auto p0 = GetTickCount();

	mPG->Clear();

	for (const auto& curr : rt)
	{
		wxPGProperty* pgp = nullptr;
		const wxString& pgp_title = curr->GetProp().GetTitle();
		const wxString  pgp_name = wxString::Format("ObjProp_%s", curr->GetProp().GetId());

		switch (curr->GetProp().GetType() )
		{
		case ftText:	pgp = new wxLongStringProperty(pgp_title, pgp_name); break;
		case ftName:	pgp = new wxStringProperty(pgp_title, pgp_name); break;
		case ftLong:
			pgp = new wxStringProperty(pgp_title, pgp_name);
			pgp->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
			break;
		case ftDouble:	pgp = new wxFloatProperty(pgp_title, pgp_name);  break;
		case ftDate:	pgp = new wxStringProperty(pgp_title, pgp_name);  break;
		case ftLink:	pgp = new whPGFileLinkProperty(pgp_title, pgp_name);  break;
		case ftFile:	pgp = new wxStringProperty(pgp_title, pgp_name);  break;
		case ftJSON:	pgp = new wxLongStringProperty(pgp_title, pgp_name);  break;
		default:break;
		}

		
		pgp->SetValueFromString(curr->GetValue());
		pgp->SetFlagRecursively(wxPG_PROP_READONLY, true);
		mPG->Append(pgp);

		if (act)
		{
			const auto& propIdIdx = act->GetPropList().get<1>();
			const auto it = propIdIdx.find(curr->GetProp().GetId());
			if (propIdIdx.end() != it)
			{
				 
				wxColour clr = act->GetColour().IsEmpty() ? *wxWHITE : act->GetColour();
				pgp->SetBackgroundColour(clr);
			}
		}
	}
	//mPG->FitColumns();
	wxLogMessage(wxString::Format("%d \t SetPropList : ", GetTickCount() - p0));
}
//-----------------------------------------------------------------------------
void ViewObjPropList::OnCmd_Update(wxCommandEvent& evt)
{
	wxBusyCursor busyCursor;
	wxWindowUpdateLocker lock(mPanel);
	sigUpdate();
}
//-----------------------------------------------------------------------------
//IViewWindow virtual 
void ViewObjPropList::OnShow()//override 
{
	//OnCmd_Update(); 
}
//-----------------------------------------------------------------------------
