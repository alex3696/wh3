#include "_pch.h"
#include "VClsDataEditorPanel.h"
#include "PGClsPid.h"

using namespace wh;
using namespace wh::view;


//-----------------------------------------------------------------------------
//---------------------------------------------------------------------------
//VClsDataEditorPanel
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
VClsDataEditorPanel::VClsDataEditorPanel(wxWindow*		parent,
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

	wxPGChoices soc;
	soc.Add(L"Абстрактный", 0);
	soc.Add(L"Номерной", 1);
	soc.Add(L"Количественный(целочисленный)", 2);
	soc.Add(L"Количественный(дробный)", 3);

	mPropGrid->Append(new wxStringProperty(L"Имя"));
	mPropGrid->Append(new wxLongStringProperty(L"Описание"));
	mPropGrid->Append(new wxEnumProperty(L"Тип экземпляров", wxPG_LABEL, soc, 0));
	mPropGrid->Append(new wxStringProperty(L"Ед.измерений", wxPG_LABEL));
	mPropGrid->Append(new wxStringProperty(L"Родительский класс"));
	mPropGrid->Append(new wxStringProperty(L"#"))->Enable(false);
		

	mPropGrid->ResetColumnSizes();
	this->Layout();

	Bind(wxEVT_PG_CHANGED, &VClsDataEditorPanel::OnClassTypeChange, this);
}
//---------------------------------------------------------------------------
void VClsDataEditorPanel::GetData(rec::Cls& rec) const
{
	mPropGrid->CommitChangesFromEditor();

	rec.mLabel = mPropGrid->GetPropertyByLabel(L"Имя")->GetValueAsString();
	rec.mComment = mPropGrid->GetPropertyByLabel(L"Описание")->GetValueAsString();
	rec.mType = wxString::Format("%d", mPropGrid->GetPropertyByLabel(L"Тип экземпляров")->GetChoiceSelection());
	rec.mMeasure = mPropGrid->GetPropertyByLabel(L"Ед.измерений")->GetValueAsString();
	rec.mParent.mId = mPropGrid->GetPropertyByLabel(L"Родительский класс")->GetValueAsString();
	rec.mID = mPropGrid->GetPropertyByLabel(L"#")->GetValueAsString();
}
//---------------------------------------------------------------------------
void VClsDataEditorPanel::SetData(const rec::Cls& rec)
{
	mPropGrid->CommitChangesFromEditor();

	mPropGrid->GetPropertyByLabel(L"Имя")->SetValueFromString(rec.mLabel);
	mPropGrid->GetPropertyByLabel(L"Описание")->SetValueFromString(rec.mComment);

	unsigned long items_type = 0;
	rec.mType.ToULong(&items_type);
	mPropGrid->GetPropertyByLabel(L"Тип экземпляров")->SetChoiceSelection(items_type);
	mPropGrid->GetPropertyByLabel(L"Ед.измерений")->SetValueFromString(rec.mMeasure);
	mPropGrid->GetPropertyByLabel(L"Родительский класс")->SetValueFromString(rec.mParent.mId);
	mPropGrid->GetPropertyByLabel(L"#")->SetValueFromString(rec.mID);
}
//-----------------------------------------------------------------------------
void VClsDataEditorPanel::SetModel(std::shared_ptr<IModel>& newModel)
{
	if (newModel != mModel)
	{
		mChangeConnection.disconnect();
		mModel = std::dynamic_pointer_cast<T_Model>(newModel);
		if (mModel)
		{
			auto funcOnChange = std::bind(&VClsDataEditorPanel::OnChangeModel,
				this, std::placeholders::_1, std::placeholders::_2);
			mChangeConnection = mModel->DoConnect(T_Model::Op::AfterChange, funcOnChange);
			OnChangeModel(mModel.get(), nullptr);
		}//if (mModel)
	}//if
}//SetModel
//---------------------------------------------------------------------------
void VClsDataEditorPanel::OnChangeModel(const IModel* model, const T_Model::T_Data* data)
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
void VClsDataEditorPanel::UpdateModel()const
{
	if (mModel)
	{
		auto rec = mModel->GetData();
		GetData(rec);
		mModel->SetData(rec);
	}
}
//---------------------------------------------------------------------------
void VClsDataEditorPanel::OnClassTypeChange(wxPropertyGridEvent& evt)
{
	UpdateModel();

}