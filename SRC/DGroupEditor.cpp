#include "_pch.h"
#include "DGroupEditor.h"
#include "config.h"

using namespace wh;
using namespace wh::view;


//-----------------------------------------------------------------------------
//---------------------------------------------------------------------------
//DGroupEditor
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
DGroupEditor::DGroupEditor(wxWindow*		parent,
	wxWindowID		id,
	const wxString& title,
	const wxPoint&	pos,
	const wxSize&	size,
	long style,
	const wxString& name)
	:DlgBaseOkCancel(parent, id, title, pos, size, style, name), mModel(nullptr)
{
	SetTitle("Редактирование информации о группе пользователей");


	mPropGrid = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, 
		wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_SPLITTER_AUTO_CENTER);
	GetSizer()->Insert(0, mPropGrid, 1, wxALL | wxEXPAND, 0);

	mPropGrid->Append(new wxStringProperty(L"Имя", wxPG_LABEL));
	mPropGrid->Append(new wxStringProperty(L"Комментарий", wxPG_LABEL));
	mPropGrid->Append(new wxStringProperty(L"ID", wxPG_LABEL))->Enable(false);

	this->Layout();

}
//---------------------------------------------------------------------------
void DGroupEditor::GetData(rec::Role& rec) const
{
	mPropGrid->CommitChangesFromEditor();

	rec.mLabel = mPropGrid->GetPropertyByLabel("Имя")->GetValueAsString();
	rec.mComment = mPropGrid->GetPropertyByLabel("Комментарий")->GetValueAsString();
	rec.mID = mPropGrid->GetPropertyByLabel("ID")->GetValueAsString();
}
//---------------------------------------------------------------------------
void DGroupEditor::SetData(const rec::Role& rec)
{
	mPropGrid->CommitChangesFromEditor();

	mPropGrid->GetPropertyByLabel(L"Имя")->SetValueFromString(rec.mLabel);
	mPropGrid->GetPropertyByLabel(L"Комментарий")->SetValueFromString(rec.mComment);
	mPropGrid->GetPropertyByLabel(L"ID")->SetValueFromString(rec.mID);

}
//-----------------------------------------------------------------------------
void DGroupEditor::SetModel(std::shared_ptr<IModel>& newModel)
{
	if (newModel != mModel)
	{
		mChangeConnection.disconnect();
		mModel = std::dynamic_pointer_cast<T_Model>(newModel);
		if (mModel)
		{
			auto funcOnChange = std::bind(&DGroupEditor::OnChangeModel,
				this, std::placeholders::_1, std::placeholders::_2);
			mChangeConnection = mModel->DoConnect(moAfterUpdate, funcOnChange);
			OnChangeModel(mModel.get(), nullptr);

			const auto& bg = whDataMgr::GetInstance()->mDbCfg->mBaseGroup->GetData();
			if ((int)bg < (int)bgAdmin)
				m_btnOK->Enable(false);
		}//if (mModel)
	}//if
}//SetModel
//---------------------------------------------------------------------------

void DGroupEditor::OnChangeModel(const IModel* model, const T_Model::T_Data* data)
{
	if (mModel && mModel.get() == model)
	{
		const auto state = model->GetState();
		const auto& rec = mModel->GetData();
		SetData(rec);
	}

}
//---------------------------------------------------------------------------
void DGroupEditor::UpdateModel()const
{
	if (mModel)
	{
		auto rec = mModel->GetData();
		GetData(rec);
		mModel->SetData(rec);
	}
}
//---------------------------------------------------------------------------
int DGroupEditor::ShowModal()
{
	return DlgBaseOkCancel::ShowModal();
}