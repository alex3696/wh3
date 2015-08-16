#include "_pch.h"
#include "VUserPanel.h"

using namespace wh;
using namespace wh::view;


//-----------------------------------------------------------------------------
//---------------------------------------------------------------------------
//VUserPanel
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
VUserPanel::VUserPanel(wxWindow*		parent,
	wxWindowID		id,
	const wxPoint&	pos,
	const wxSize&	size,
	long style,
	const wxString& name)
	:wxPanel(parent, id, pos, size, style, name), mModel(nullptr)
{
	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(szrMain);

	mPropGrid = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_SPLITTER_AUTO_CENTER);
	GetSizer()->Insert(0, mPropGrid, 1, wxALL | wxEXPAND, 0);

	mPropGrid->Append(new wxStringProperty(L"���"));
	mPropGrid->Append(new wxLongStringProperty(L"��������"));
	mPropGrid->Append(new wxIntProperty(L"����� �����������", wxPG_LABEL, -1));
	mPropGrid->Append(new wxStringProperty(L"����� ��"));
	mPropGrid->Append(new wxStringProperty(L"������"));
	mPropGrid->Append(new wxStringProperty(L"ID"))->Enable(false);


	mPropGrid->ResetColumnSizes();
	this->Layout();

}
//---------------------------------------------------------------------------
void VUserPanel::GetData(rec::User& rec) const
{
	mPropGrid->CommitChangesFromEditor();

	rec.mLabel = mPropGrid->GetPropertyByLabel(L"���")->GetValueAsString();
	rec.mComment = mPropGrid->GetPropertyByLabel(L"��������")->GetValueAsString();
	rec.mConnLimit= mPropGrid->GetPropertyByLabel(L"����� �����������")->GetValueAsString();
	rec.mValidUntil = mPropGrid->GetPropertyByLabel(L"����� ��")->GetValueAsString();
	rec.mPassword = mPropGrid->GetPropertyByLabel(L"������")->GetValueAsString();
	rec.mID = mPropGrid->GetPropertyByLabel(L"ID")->GetValueAsString();

}
//---------------------------------------------------------------------------
void VUserPanel::SetData(const rec::User& rec)
{
	mPropGrid->CommitChangesFromEditor();

	mPropGrid->GetPropertyByLabel(L"���")->SetValueFromString(rec.mLabel);
	mPropGrid->GetPropertyByLabel(L"��������")->SetValueFromString(rec.mComment);
	mPropGrid->GetPropertyByLabel(L"����� �����������")->SetValueFromString(rec.mConnLimit);
	mPropGrid->GetPropertyByLabel(L"����� ��")->SetValueFromString(rec.mValidUntil);
	mPropGrid->GetPropertyByLabel(L"������")->SetValueFromString(rec.mPassword);
	mPropGrid->GetPropertyByLabel(L"ID")->SetValueFromString(rec.mID);

}
//-----------------------------------------------------------------------------
void VUserPanel::SetModel(std::shared_ptr<IModel>& newModel)
{
	if (newModel != mModel)
	{
		mChangeConnection.disconnect();
		mModel = std::dynamic_pointer_cast<T_Model>(newModel);
		if (mModel)
		{
			auto funcOnChange = std::bind(&VUserPanel::OnChangeModel,
				this, std::placeholders::_1, std::placeholders::_2);
			mChangeConnection = mModel->DoConnect(MUser2::Op::AfterChange, funcOnChange);
			OnChangeModel(mModel.get(), nullptr);

		}//if (mModel)
	}//if
}//SetModel
//---------------------------------------------------------------------------

void VUserPanel::OnChangeModel(const IModel* model, const MUser2::T_Data* data)
{
	if (mModel && mModel.get() == model)
	{
		const auto state = model->GetState();
		const auto& rec = mModel->GetData();
		SetData(rec);

		switch (state)
		{
		default: //msNull
			mPropGrid->SetForegroundColour(wxColour(255, 255, 255)); break;
		case msCreated: mPropGrid->SetForegroundColour(wxColour(230, 240, 255)); break;
		case msDeleted: mPropGrid->SetForegroundColour(wxColour(255, 230, 230)); break;
		case msExist:   mPropGrid->SetForegroundColour(wxColour(240, 255, 240)); break;
		case msUpdated: mPropGrid->SetForegroundColour(wxColour(255, 240, 200)); break;
		}

	}

}
//---------------------------------------------------------------------------
void VUserPanel::UpdateModel()const
{
	if (mModel)
	{
		auto rec = mModel->GetData();
		GetData(rec);
		mModel->SetData(rec);
	}
}
