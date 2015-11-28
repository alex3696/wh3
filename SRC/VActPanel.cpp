#include "_pch.h"
#include "VActPanel.h"
#include "PGClsPid.h"

using namespace wh;
using namespace wh::view;


//-----------------------------------------------------------------------------
//---------------------------------------------------------------------------
//VActPanel
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
VActPanel::VActPanel(wxWindow*		parent,
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

	auto lbl = mPropGrid->Append(new wxStringProperty(L"Имя"));
	lbl->SetValidator(titleValidator);
	mPropGrid->Append(new wxLongStringProperty(L"Описание"));
	mPropGrid->Append(new wxColourProperty(L"Цвет"));
	mPropGrid->Append(new wxLongStringProperty(L"Скрипт"));
	mPropGrid->Append(new wxStringProperty(L"Идентификатор"))->Enable(false);
	//mPropGrid->Append(new wxStringProperty(L"Версия"))->Enable(false);

	mPropGrid->ResetColumnSizes();
	this->Layout();


	//mPropGrid->Bind(wxEVT_PG_CHANGED, [](wxPropertyGridEvent&)
	//{
	//});
	//mPropGrid->Bind(wxEVT_PG_CHANGING, [](wxPropertyGridEvent&)
	//{
	//});
}
//---------------------------------------------------------------------------
void VActPanel::GetData(rec::Act& rec) const
{
	mPropGrid->CommitChangesFromEditor();

	rec.mLabel = mPropGrid->GetPropertyByLabel("Имя")->GetValueAsString();
	rec.mComment = mPropGrid->GetPropertyByLabel("Описание")->GetValueAsString();

	wxAny value = mPropGrid->GetPropertyByLabel("Цвет")->GetValue();
	rec.mColor = value.As<wxColour>().GetAsString();

	rec.mScript = mPropGrid->GetPropertyByLabel("Скрипт")->GetValueAsString();
	rec.mID = mPropGrid->GetPropertyByLabel("Идентификатор")->GetValueAsString();
	//rec.mVID = mPropGrid->GetPropertyByLabel("Версия")->GetValueAsString();
}
//---------------------------------------------------------------------------
void VActPanel::SetData(const rec::Act& rec)
{
	mPropGrid->CommitChangesFromEditor();

	mPropGrid->GetPropertyByLabel(L"Имя")->SetValueFromString(rec.mLabel);
	mPropGrid->GetPropertyByLabel(L"Описание")->SetValueFromString(rec.mComment);
	mPropGrid->GetPropertyByLabel(L"Цвет")->SetValueFromString(rec.mColor);
	mPropGrid->GetPropertyByLabel(L"Скрипт")->SetValueFromString(rec.mScript);
	mPropGrid->GetPropertyByLabel("Идентификатор")->SetValueFromString(rec.mID);
	//mPropGrid->GetPropertyByLabel("Версия")->SetValueFromString(rec.mVID);
}
//-----------------------------------------------------------------------------
void VActPanel::SetModel(std::shared_ptr<IModel>& newModel)
{
	if (newModel != mModel)
	{
		mChangeConnection.disconnect();
		mModel = std::dynamic_pointer_cast<T_Model>(newModel);
		if (mModel)
		{
			auto funcOnChange = std::bind(&VActPanel::OnChangeModel,
				this, std::placeholders::_1, std::placeholders::_2);
			mChangeConnection = mModel->DoConnect(moAfterUpdate, funcOnChange);
			OnChangeModel( mModel.get(), nullptr);
		}//if (mModel)
	}//if
}//SetModel
//---------------------------------------------------------------------------

void VActPanel::OnChangeModel(const IModel* model, const MAct::T_Data* data)
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
void VActPanel::UpdateModel()const
{
	if (mModel)
	{
		auto rec = mModel->GetData();
		GetData(rec);
		mModel->SetData(rec);
	}
}
