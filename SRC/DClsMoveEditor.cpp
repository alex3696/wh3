#include "_pch.h"
#include "DClsMoveEditor.h"
#include "DClsActEditor.h"
#include "BtnProperty.h"

using namespace wh;
using namespace view;






//-----------------------------------------------------------------------------
//---------------------------------------------------------------------------
//DClsMoveEditor
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
DClsMoveEditor::DClsMoveEditor(wxWindow*		parent,
	wxWindowID		id,
	const wxString& title,
	const wxPoint&	pos,
	const wxSize&	size,
	long style,
	const wxString& name)
	:DlgBaseOkCancel(parent, id, title, pos, size, style, name), mModel(nullptr)
{
	SetTitle("Редактирование информации о разрешении действия");
	wxSizer*	szrMain = GetSizer();

	mPropGrid = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_SPLITTER_AUTO_CENTER);
	szrMain->Insert(0, mPropGrid, 1, wxALL | wxEXPAND, 0);

	mGroupArray.reset(new MGroupArray);
	mGroupArray->Load();

	std::function<bool(wxPGProperty*)> selectGroup = [this](wxPGProperty* prop)
	{
		DGroupSelector dlg(nullptr);
		dlg.SetSrcVec(mGroupArray);

		if (wxID_OK == dlg.ShowModal())
		{
			wxDataViewItemArray selected;
			dlg.GetSelections(selected);
			if (!selected.empty())
			{
				unsigned int row = dlg.GetRow(selected[0]);
				auto childModel = mGroupArray->GetChild(row);
				auto groupModel = std::dynamic_pointer_cast<MGroup>(childModel);
				const auto& groupData = groupModel->GetData();

				mPropGrid->CommitChangesFromEditor();

				auto clsAct = mModel->GetData();

				clsAct.mAcessGroup = groupData.mLabel;

				mModel->SetData(clsAct);
				return true;
			}

		}
		return false;
	};


	mPropGrid->Append(new wxBoolProperty("Запретить"));

	BtnProperty* groupProp = new BtnProperty("Группа");
	groupProp->SetOnClickButonFunc(selectGroup);
	mPropGrid->Append(groupProp);

	mPropGrid->Append(new wxLongStringProperty(L"Скрипт"));

	mPropGrid->Append(new wxStringProperty(L"Класс"));
	mPropGrid->Append(new wxStringProperty(L"Объект"));
	

	mPropGrid->Append(new wxStringProperty(L"Источник"));
	mPropGrid->Append(new wxStringProperty(L"Приемник"));

	mPropGrid->Append(new wxStringProperty(L"ID"))->Enable(false);



	this->Layout();
}
//---------------------------------------------------------------------------
void DClsMoveEditor::SetModel(std::shared_ptr<IModel>& newModel)
{
	if (newModel != mModel)
	{
		mChangeConnection.disconnect();
		mModel = std::dynamic_pointer_cast<MClsMove>(newModel);
		if (mModel)
		{
			auto funcOnChange = std::bind(&DClsMoveEditor::OnChangeModel,
				this, std::placeholders::_1, std::placeholders::_2);
			mChangeConnection = mModel->DoConnect(MClsMove::Op::AfterChange, funcOnChange);
			OnChangeModel(mModel.get(), nullptr);
		}//if (mModel)
	}//if
}//SetModel

//---------------------------------------------------------------------------
void DClsMoveEditor::GetData(rec::ClsSlotAccess& rec) const
{
	mPropGrid->CommitChangesFromEditor();

	wxString accessDisabled = mPropGrid->GetPropertyByLabel(L"Запретить")->GetValueAsString();
	rec.mAccessDisabled = (0 == accessDisabled.CmpNoCase("true")) ? "1" : "0";

	rec.mAcessGroup = mPropGrid->GetPropertyByLabel(L"Группа")->GetValueAsString();
	rec.mScriptRestrict = mPropGrid->GetPropertyByLabel(L"Скрипт")->GetValueAsString();
	
	rec.mCls.mLabel = mPropGrid->GetPropertyByLabel("Класс")->GetValueAsString();
	rec.mObj.mLabel = mPropGrid->GetPropertyByLabel("Объект")->GetValueAsString();

	/*
	wh::ObjKeyPath path;
	wxString generated_path;

	path.ParsePath(mPropGrid->GetPropertyByLabel("Источник")->GetValueAsString());
	path.GenerateArray(generated_path, true);
	rec.mSrcPath = generated_path;

	path.ParsePath(mPropGrid->GetPropertyByLabel("Приемник")->GetValueAsString());
	path.GenerateArray(generated_path, true);
	rec.mDstPath = generated_path;
	*/


	rec.mId = mPropGrid->GetPropertyByLabel("ID")->GetValueAsString();
}
//---------------------------------------------------------------------------
void DClsMoveEditor::SetData(const rec::ClsSlotAccess& rec)
{
	mPropGrid->CommitChangesFromEditor();

	mPropGrid->GetPropertyByLabel(L"Запретить")->	SetValueFromString(("1" == rec.mAccessDisabled) ? "true" : "false");
	mPropGrid->GetPropertyByLabel(L"Группа")->SetValueFromString(rec.mAcessGroup);
	mPropGrid->GetPropertyByLabel(L"Скрипт")->SetValueFromString(rec.mScriptRestrict);

	mPropGrid->GetPropertyByLabel(L"Класс")->SetValueFromString(rec.mCls.mLabel);
	mPropGrid->GetPropertyByLabel(L"Объект")->SetValueFromString(rec.mObj.mLabel);

	/*
	wh::ObjKeyPath path;
	wxString pathStr;

	path.ParseArray(rec.mSrcPath);
	path.GeneratePath(pathStr);
	mPropGrid->GetPropertyByLabel("Источник")->SetValueFromString(pathStr);

	path.ParseArray(rec.mDstPath);
	path.GeneratePath(pathStr);
	mPropGrid->GetPropertyByLabel("Приемник")->SetValueFromString(pathStr);
*/
	mPropGrid->GetPropertyByLabel(L"ID")->SetValueFromString(rec.mId);
}
//---------------------------------------------------------------------------

void DClsMoveEditor::OnChangeModel(const IModel* model, const MClsMove::T_Data* data)
{
	if (mModel && mModel.get() == model)
	{
		const auto state = model->GetState();
		const auto& rec = mModel->GetData();
		SetData(rec);
	}
}
//---------------------------------------------------------------------------
void DClsMoveEditor::UpdateModel()const
{
	if (mModel)
	{
		auto rec = mModel->GetData();
		GetData(rec);
		mModel->SetData(rec);
	}
}
//---------------------------------------------------------------------------
int DClsMoveEditor::ShowModal()
{
	return DlgBaseOkCancel::ShowModal();
}

