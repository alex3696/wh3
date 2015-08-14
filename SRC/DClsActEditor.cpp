#include "_pch.h"
#include "DClsActEditor.h"
#include "BtnProperty.h"

using namespace wh;
using namespace view;






//-----------------------------------------------------------------------------
//---------------------------------------------------------------------------
//DClsActEditor
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
DClsActEditor::DClsActEditor(wxWindow*		parent,
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


	mActArray.reset(new MActArray);
	mActArray->Load();

	mGroupArray.reset(new MGroupArray);
	mGroupArray->Load();


	std::function<bool(wxPGProperty*)> selecAct = [this](wxPGProperty* prop)
	{
		DActSelector dlg(nullptr);
		dlg.SetSrcVec(mActArray);

		if (wxID_OK == dlg.ShowModal())
		{
			wxDataViewItemArray selected;
			dlg.GetSelections(selected);
			if (!selected.empty())
			{
				unsigned int row = dlg.GetRow(selected[0]);
				auto childModel = mActArray->GetChild(row);
				auto actModel = std::dynamic_pointer_cast<MAct>(childModel);
				const auto& actData = actModel->GetData();

				mPropGrid->CommitChangesFromEditor();
				
				auto clsAct = mModel->GetData();
				
				clsAct.mActID = actData.mID;
				clsAct.mActLabel = actData.mLabel;
				
				mModel->SetData(clsAct);
				return true;
			}

		}
		return false;
	};


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


	BtnProperty* btnProp = new BtnProperty("Действие");
	btnProp->SetOnClickButonFunc(selecAct);
	mPropGrid->Append(btnProp);

	mPropGrid->Append(new wxBoolProperty("Запретить"));
	
	BtnProperty* groupProp = new BtnProperty("Группа");
	groupProp->SetOnClickButonFunc(selectGroup);
	mPropGrid->Append(groupProp);
	
	mPropGrid->Append(new wxLongStringProperty(L"Скрипт"));

	mPropGrid->Append(new wxStringProperty(L"Объект"));
	mPropGrid->Append(new wxStringProperty(L"Путь"));

	mPropGrid->Append(new wxStringProperty(L"ID"))->Enable(false);



	this->Layout();
}
//---------------------------------------------------------------------------
void DClsActEditor::SetModel(std::shared_ptr<IModel>& newModel)
{
	if (newModel != mModel)
	{
		mChangeConnection.disconnect();
		mModel = std::dynamic_pointer_cast<MClsAct>(newModel);
		if (mModel)
		{
			auto funcOnChange = std::bind(&DClsActEditor::OnChangeModel,
				this, std::placeholders::_1);
			mChangeConnection = mModel->ConnectChangeDataSlot(funcOnChange);
			OnChangeModel(*mModel.get());
		}//if (mModel)
	}//if
}//SetModel

//---------------------------------------------------------------------------
void DClsActEditor::GetData(rec::ClsActAccess& rec) const
{
	mPropGrid->CommitChangesFromEditor();

	rec.mActLabel = mPropGrid->GetPropertyByLabel(L"Действие")->GetValueAsString();


	wxString accessDisabled = mPropGrid->GetPropertyByLabel(L"Запретить")->GetValueAsString();
	rec.mAccessDisabled = (0 == accessDisabled.CmpNoCase("true")) ? "1" : "0";

	rec.mAcessGroup = mPropGrid->GetPropertyByLabel(L"Группа")->GetValueAsString();
	rec.mScriptRestrict = mPropGrid->GetPropertyByLabel(L"Скрипт")->GetValueAsString();
	rec.mObjLabel = mPropGrid->GetPropertyByLabel("Объект")->GetValueAsString();

	wh::ObjKeyPath path;
	path.ParsePath(mPropGrid->GetPropertyByLabel("Путь")->GetValueAsString());

	if (path.size())
		path.GenerateArray(rec.mPath, true);
	else
		rec.mPath.clear();

	rec.mID = mPropGrid->GetPropertyByLabel("ID")->GetValueAsString();
}
//---------------------------------------------------------------------------
void DClsActEditor::SetData(const rec::ClsActAccess& rec)
{
	mPropGrid->CommitChangesFromEditor();

	mPropGrid->GetPropertyByLabel(L"Действие")->SetValueFromString(rec.mActLabel);
	mPropGrid->GetPropertyByLabel(L"Запретить")->
		SetValueFromString(("1" == rec.mAccessDisabled) ? "true" : "false");
	mPropGrid->GetPropertyByLabel(L"Группа")->SetValueFromString(rec.mAcessGroup);
	mPropGrid->GetPropertyByLabel(L"Скрипт")->SetValueFromString(rec.mScriptRestrict);
	mPropGrid->GetPropertyByLabel(L"Объект")->SetValueFromString(rec.mObjLabel);
	
	wh::ObjKeyPath path;
	path.ParseArray(rec.mPath);
	wxString pathStr;
	path.GeneratePath(pathStr);
	mPropGrid->GetPropertyByLabel("Путь")->SetValueFromString(pathStr);
	
	mPropGrid->GetPropertyByLabel(L"ID")->SetValueFromString(rec.mID);
}
//---------------------------------------------------------------------------

void DClsActEditor::OnChangeModel(const IModel& model)
{
	if (mModel && mModel.get() == &model)
	{
		const auto state = model.GetState();
		const auto& rec = mModel->GetData();
		SetData(rec);
	}
}
//---------------------------------------------------------------------------
void DClsActEditor::UpdateModel()const
{
	if (mModel)
	{
		auto rec = mModel->GetData();
		GetData(rec);
		mModel->SetData(rec);
	}
}
//---------------------------------------------------------------------------
int DClsActEditor::ShowModal()
{
	return DlgBaseOkCancel::ShowModal();
}

