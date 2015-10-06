#include "_pch.h"
#include "DClsPropEditor.h"
#include "BtnProperty.h"

using namespace wh;
using namespace view;






//-----------------------------------------------------------------------------
//---------------------------------------------------------------------------
//DClsPropEditor
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
DClsPropEditor::DClsPropEditor(wxWindow*		parent,
	wxWindowID		id,
	const wxString& title,
	const wxPoint&	pos,
	const wxSize&	size,
	long style,
	const wxString& name)
	:DlgBaseOkCancel(parent, id, title, pos, size, style, name), mModel(nullptr)
{
	SetTitle("Редактирование информации о свойстве класса");
	wxSizer*	szrMain = GetSizer();

	mPropGrid = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_SPLITTER_AUTO_CENTER);
	szrMain->Insert(0, mPropGrid, 1, wxALL | wxEXPAND, 0);


	mPropArray.reset(new MPropArray);
	mPropArray->Load();



	std::function<bool(wxPGProperty*)> selectProp = [this](wxPGProperty* prop)
	{
		DPropSelector dlg(nullptr);
		dlg.SetSrcVec(mPropArray);

		if (wxID_OK == dlg.ShowModal())
		{
			wxDataViewItemArray selected;
			dlg.GetSelections(selected);
			if (!selected.empty())
			{
				unsigned int row = dlg.GetRow(selected[0]);
				auto childModel = mPropArray->GetChild(row);
				auto propModel = std::dynamic_pointer_cast<MPropChild>(childModel);
				const auto& propData = propModel->GetData();

				mPropGrid->CommitChangesFromEditor();
				auto clsProp = mModel->GetData();
				clsProp.mProp = propData;
				mModel->SetData(clsProp);
				return true;
			}

		}
		return false;
	};



	BtnProperty* btnProp = new BtnProperty("Свойство");
	btnProp->SetOnClickButonFunc(selectProp);
	mPropGrid->Append(btnProp);

	mPropGrid->Append(new wxStringProperty(L"Значение", wxPG_LABEL));
	mPropGrid->Append(new wxStringProperty(L"ID", wxPG_LABEL))->Enable(false);



	this->Layout();
}
//---------------------------------------------------------------------------
void DClsPropEditor::SetModel(std::shared_ptr<IModel>& newModel)
{
	if (newModel != mModel)
	{
		mChangeConnection.disconnect();
		mModel = std::dynamic_pointer_cast<MClsProp>(newModel);
		if (mModel)
		{
			auto funcOnChange = std::bind(&DClsPropEditor::OnChangeModel,
				this, std::placeholders::_1, std::placeholders::_2);
			mChangeConnection = mModel->DoConnect(MClsProp::Op::AfterChange, funcOnChange);
			OnChangeModel(mModel.get(), nullptr);
		}//if (mModel)
	}//if
}//SetModel

//---------------------------------------------------------------------------
void DClsPropEditor::GetData(rec::ClsProp& rec) const
{
	mPropGrid->CommitChangesFromEditor();

	// значение свойства получается из диалоа
	//rec.mProp.mLabel = mPropGrid->GetPropertyByLabel("Свойство")->GetValueAsString();
	rec.mVal = mPropGrid->GetPropertyByLabel("Значение")->GetValueAsString();
	rec.mId = mPropGrid->GetPropertyByLabel("ID")->GetValueAsString();
}
//---------------------------------------------------------------------------
void DClsPropEditor::SetData(const rec::ClsProp& rec)
{
	mPropGrid->CommitChangesFromEditor();

	mPropGrid->GetPropertyByLabel(L"Свойство")->SetValueFromString(rec.mProp.mLabel);
	mPropGrid->GetPropertyByLabel(L"Значение")->SetValueFromString(rec.mVal);
	mPropGrid->GetPropertyByLabel(L"ID")->SetValueFromString(rec.mId.toStr());
}
//---------------------------------------------------------------------------

void DClsPropEditor::OnChangeModel(const IModel* model, const MClsProp::T_Data* data)
{
	if (mModel && mModel.get() == model)
	{
		const auto state = model->GetState();
		const auto& rec = mModel->GetData();
		SetData(rec);
	}
}
//---------------------------------------------------------------------------
void DClsPropEditor::UpdateModel()const
{
	if (mModel)
	{
		auto rec = mModel->GetData();
		GetData(rec);
		mModel->SetData(rec);
	}
}
//---------------------------------------------------------------------------
int DClsPropEditor::ShowModal()
{
	return DlgBaseOkCancel::ShowModal();
}

