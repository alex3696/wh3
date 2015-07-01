#include "_pch.h"
#include "DClsObjQtyEditor.h"
#include "BtnProperty.h"

using namespace wh;
using namespace view;






//-----------------------------------------------------------------------------
//---------------------------------------------------------------------------
//DClsObjQtyEditor
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
DClsObjQtyEditor::DClsObjQtyEditor(wxWindow*		parent,
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
	mPropGrid->Append(new wxStringProperty(L"Количество", wxPG_LABEL));



	this->Layout();
}
//---------------------------------------------------------------------------
void DClsObjQtyEditor::SetModel(std::shared_ptr<IModel>& newModel)
{
	if (newModel != mModel)
	{
		mChangeConnection.disconnect();
		mModel = std::dynamic_pointer_cast<MClsObjQty>(newModel);
		if (mModel)
		{
			auto funcOnChange = std::bind(&DClsObjQtyEditor::OnChangeModel,
				this, std::placeholders::_1);
			mChangeConnection = mModel->ConnectChangeDataSlot(funcOnChange);
			OnChangeModel(*mModel.get());
		}//if (mModel)
	}//if
}//SetModel

//---------------------------------------------------------------------------
void DClsObjQtyEditor::GetData(rec::ClsObjQty& rec) const
{
	mPropGrid->CommitChangesFromEditor();

	rec.mLabel = mPropGrid->GetPropertyByLabel("Имя")->GetValueAsString();
	rec.mPID = mPropGrid->GetPropertyByLabel("PID")->GetValueAsString();
	rec.mQty = mPropGrid->GetPropertyByLabel("Количество")->GetValueAsString();
}
//---------------------------------------------------------------------------
void DClsObjQtyEditor::SetData(const rec::ClsObjQty& rec)
{
	mPropGrid->CommitChangesFromEditor();

	mPropGrid->GetPropertyByLabel(L"Имя")->SetValueFromString(rec.mLabel);
	mPropGrid->GetPropertyByLabel(L"PID")->SetValueFromString(rec.mPID);
	mPropGrid->GetPropertyByLabel(L"Количество")->SetValueFromString(rec.mQty);
}
//---------------------------------------------------------------------------

void DClsObjQtyEditor::OnChangeModel(const IModel& model)
{
	if (mModel && mModel.get() == &model)
	{
		const auto state = model.GetState();
		const auto& rec = mModel->GetData();
		SetData(rec);
	}
}
//---------------------------------------------------------------------------
void DClsObjQtyEditor::UpdateModel()const
{
	if (mModel)
	{
		auto rec = mModel->GetData();
		GetData(rec);
		mModel->SetData(rec);
	}
}
//---------------------------------------------------------------------------
int DClsObjQtyEditor::ShowModal()
{
	return DlgBaseOkCancel::ShowModal();
}

