#include "_pch.h"
#include "dlg_act_view_PnlPropGrid.h"
#include "TViewCtrlPanel.h"
#include "whPGFileLinkProperty.h"

using namespace wh;
using namespace wh::dlg_act::view;

PnlPropGrid::PnlPropGrid(wxWindow* parent,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name)
	: wxPanel(parent, id, pos, size, style, name)
{

	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);
	SetSizer(szrMain);

	mPropGrid = new wxPropertyGrid(this);
	szrMain->Add(mPropGrid, 1, wxALL | wxEXPAND, 0);

	mPropGrid->AddActionTrigger(wxPG_ACTION_NEXT_PROPERTY, WXK_RETURN);
	mPropGrid->AddActionTrigger(wxPG_ACTION_EDIT, WXK_RETURN);
	mPropGrid->DedicateKey(WXK_RETURN);
	mPropGrid->DedicateKey(WXK_UP);
	mPropGrid->DedicateKey(WXK_DOWN);
	//Bind(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, &CtrlPanel::OnActivated, this);
	//Layout();
}//
//-----------------------------------------------------------------------------
PnlPropGrid::~PnlPropGrid()
{

}
//-----------------------------------------------------------------------------
void PnlPropGrid::SetModel(std::shared_ptr<dlg_act::model::PropArray>& model)
{
	mPropArray = model;

	mPropGrid->Clear();

	for (unsigned int i = 0; i < mPropArray->GetChildQty(); ++i)
	{
		auto prop = std::dynamic_pointer_cast<model::Prop>(mPropArray->GetChild(i));
		if (prop)
		{
			const auto& propData = prop->GetData();
			switch (propData.mProp.mType )
			{
			case ftText:	
				if (   true != propData.mProp.mVarArray.IsEmpty() 
					&& "{}" != propData.mProp.mVarArray)
				{
					auto arr = Sql2ArrayString(propData.mProp.mVarArray);
					wxPGChoices eech = arr;
					if (Sql2Bool(propData.mProp.mVarStrict))
						mPropGrid->Append(new wxEnumProperty(propData.mProp.mLabel, wxPG_LABEL, eech));
					else
						mPropGrid->Append(new wxEditEnumProperty(propData.mProp.mLabel, wxPG_LABEL, eech));
				}
				else
					mPropGrid->Append(new wxLongStringProperty(propData.mProp.mLabel));
				break;
			case ftName:	mPropGrid->Append(new wxStringProperty(propData.mProp.mLabel)); break;
			case ftLong:	mPropGrid->Append(new wxIntProperty(propData.mProp.mLabel));  break;
			case ftDouble:	mPropGrid->Append(new wxFloatProperty(propData.mProp.mLabel));  break;
			case ftDate:	mPropGrid->Append(new wxDateProperty(propData.mProp.mLabel));  break;
			case ftLink:	mPropGrid->Append(new whPGFileLinkProperty(propData.mProp.mLabel));  break;
			case ftFile:	mPropGrid->Append(new wxFileProperty(propData.mProp.mLabel));  break;
			case ftJSON:	mPropGrid->Append(new wxLongStringProperty(propData.mProp.mLabel));  break;
			case ftBool:	
				{
					auto pgp = mPropGrid->Append(new wxBoolProperty(propData.mProp.mLabel));
					pgp->SetAttribute(wxPG_BOOL_USE_CHECKBOX, true);
				}
				break;
			default:
				break;
			}
		}

	}

	
}
//-----------------------------------------------------------------------------
std::shared_ptr<dlg_act::model::PropArray> PnlPropGrid::GetModel()
{
	return mPropArray;
}

//-----------------------------------------------------------------------------
bool PnlPropGrid::UpdateModel()
{
	bool hasEmpty = false;

	for (unsigned int i = 0; i < mPropArray->GetChildQty(); ++i)
	{
		auto prop = std::dynamic_pointer_cast<model::Prop>(mPropArray->GetChild(i));
		if (prop)
		{
			auto propData = prop->GetData();
			auto pgProp = mPropGrid->GetProperty(propData.mProp.mLabel);
			if (pgProp)
			{
				auto val = pgProp->GetValueAsString();
				val.Replace("\"", "\\\"");
				propData.mVal = val;
				if (!propData.mVal.IsNull())
					prop->SetData(propData);
				else
					hasEmpty = true;
			}
		}
	}

	return hasEmpty;
}