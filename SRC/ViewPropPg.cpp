#include "_pch.h"
#include "ViewPropPg.h"
#include "whPGFileLinkProperty.h"

using namespace wh;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewPropPg::ViewPropPg(wxWindow* parent)
{
	mPG = new wxPropertyGrid(parent, wxID_ANY
		, wxDefaultPosition, wxDefaultSize
		, wxPG_SPLITTER_AUTO_CENTER
		);
	
}
//-----------------------------------------------------------------------------
ViewPropPg::ViewPropPg(const std::shared_ptr<IViewWindow>& parent)
	:ViewPropPg(parent->GetWnd())
{
}
//-----------------------------------------------------------------------------

wxWindow* ViewPropPg::GetWnd()const
{
	return mPG;
}

//-----------------------------------------------------------------------------
void ViewPropPg::SetBeforeRefresh(std::shared_ptr<const ModelPropTable> table)
{

}
//-----------------------------------------------------------------------------
void ViewPropPg::SetAfterRefresh(std::shared_ptr<const ModelPropTable> table)
{
	if (!mPG)
		return;
	TEST_FUNC_TIME;
	wxBusyCursor busyCursor;
	wxWindowUpdateLocker lock(mPG);

	mPG->Clear();

	for (const auto& curr : table->GetStorage())
	{
		wxPGProperty* pgp = nullptr;
		const wxString& pgp_title = curr->GetTitle();
		const wxString  pgp_name = wxString::Format("ObjProp_%s", curr->GetIdAsString());

		switch (curr->GetKind())
		{
		case ftText:
		{
			//pgp = new wxLongStringProperty(pgp_title, pgp_name);
			if (true != curr->GetVar().IsEmpty())
			{
				wxPGChoices eech = curr->GetVar();
				if (curr->GetVarStrict())
					pgp = new wxEnumProperty(pgp_title, pgp_name, eech);
				else
					pgp = new wxEditEnumProperty(pgp_title, pgp_name, eech);
			}
			else
				pgp = new wxLongStringProperty(pgp_title, pgp_name);
			break;
		}break;
		case ftName:	pgp = new wxStringProperty(pgp_title, pgp_name); break;
		case ftLong:
			pgp = new wxStringProperty(pgp_title, pgp_name);
			pgp->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
			break;
		case ftDouble:	pgp = new wxFloatProperty(pgp_title, pgp_name);  break;
		case ftDate:
		{
			pgp = new wxDateProperty(pgp_title, pgp_name);
			pgp->SetAttribute(wxPG_DATE_PICKER_STYLE
				, (long)(wxDP_DROPDOWN | wxDP_SHOWCENTURY | wxDP_ALLOWNONE));
		}break;
		case ftBool:
		{
			pgp = new wxBoolProperty(pgp_title, pgp_name);
			pgp->SetAttribute(wxPG_BOOL_USE_CHECKBOX, true);
		}
		break;

		case ftLink:	pgp = new whPGFileLinkProperty(pgp_title, pgp_name);  break;
		case ftFile:	pgp = new wxStringProperty(pgp_title, pgp_name);  break;
		case ftJSON:	pgp = new wxLongStringProperty(pgp_title, pgp_name);  break;
		default:break;
		}
		mPG->Append(pgp);
	}
	//mPG->FitColumns();
	mPG->Sort(); // wxPG_SORT_TOP_LEVEL_ONLY
	//auto w = mPG->FitColumns().x;
	//mPG->ResetColumnSizes(true);
	//mPG->SetSplitterPosition(w);
	//mPG->SetSplitterLeft(true);
	
}
