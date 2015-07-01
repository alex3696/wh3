#include "_pch.h"
#include "DClsObjNumEditor.h"
#include "BtnProperty.h"

using namespace wh;
using namespace view;






//-----------------------------------------------------------------------------
//---------------------------------------------------------------------------
//DClsObjNumEditor
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
DClsObjNumEditor::DClsObjNumEditor(wxWindow*		parent,
	wxWindowID		id,
	const wxString& title,
	const wxPoint&	pos,
	const wxSize&	size,
	long style,
	const wxString& name)
	:DlgBaseOkCancel(parent, id, title, pos, size, style, name), mModel(nullptr)
{
	SetTitle("Редактирование информации о объекте");
	wxSizer*	szrMain = GetSizer();

	mPropGrid = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_SPLITTER_AUTO_CENTER);
	szrMain->Insert(0, mPropGrid, 1, wxALL | wxEXPAND, 0);


	mPropGrid->Append(new wxStringProperty(L"Имя", wxPG_LABEL));
	mPropGrid->Append(new wxStringProperty(L"PID", wxPG_LABEL));
	mPropGrid->Append(new wxStringProperty(L"ID", wxPG_LABEL))->Enable(false);



	this->Layout();
}
//---------------------------------------------------------------------------
void DClsObjNumEditor::SetModel(std::shared_ptr<IModel>& newModel)
{
	if (newModel != mModel)
	{
		mChangeConnection.disconnect();
		mModel = std::dynamic_pointer_cast<MClsObjNum>(newModel);
		if (mModel)
		{
			auto funcOnChange = std::bind(&DClsObjNumEditor::OnChangeModel,
				this, std::placeholders::_1);
			mChangeConnection = mModel->ConnectChangeDataSlot(funcOnChange);
			OnChangeModel(*mModel.get());
		}//if (mModel)
	}//if
}//SetModel

//---------------------------------------------------------------------------
void DClsObjNumEditor::GetData(rec::ClsObjNum& rec) const
{
	mPropGrid->CommitChangesFromEditor();

	rec.mLabel = mPropGrid->GetPropertyByLabel("Имя")->GetValueAsString();
	rec.mPID = mPropGrid->GetPropertyByLabel("PID")->GetValueAsString();
	rec.mID = mPropGrid->GetPropertyByLabel("ID")->GetValueAsString();
}
//---------------------------------------------------------------------------
void DClsObjNumEditor::SetData(const rec::ClsObjNum& rec)
{
	mPropGrid->CommitChangesFromEditor();

	mPropGrid->GetPropertyByLabel(L"Имя")->SetValueFromString(rec.mLabel);
	mPropGrid->GetPropertyByLabel(L"PID")->SetValueFromString(rec.mPID);
	mPropGrid->GetPropertyByLabel(L"ID")->SetValueFromString(rec.mID);
}
//---------------------------------------------------------------------------

void DClsObjNumEditor::OnChangeModel(const IModel& model)
{
	if (mModel && mModel.get() == &model)
	{
		const auto state = model.GetState();
		const auto& rec = mModel->GetData();
		SetData(rec);
	}
}
//---------------------------------------------------------------------------
void DClsObjNumEditor::UpdateModel()const
{
	if (mModel)
	{
		auto rec = mModel->GetData();
		GetData(rec);
		mModel->SetData(rec);
	}
}
//---------------------------------------------------------------------------
int DClsObjNumEditor::ShowModal()
{
	return DlgBaseOkCancel::ShowModal();
}

