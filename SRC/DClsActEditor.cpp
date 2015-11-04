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
	SetTitle("�������������� ���������� � ���������� ��������");
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
				
				clsAct.mAct.mId = actData.mID;
				clsAct.mAct.mLabel = actData.mLabel;
				
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


	BtnProperty* btnProp = new BtnProperty("��������");
	btnProp->SetOnClickButonFunc(selecAct);
	mPropGrid->Append(btnProp);

	mPropGrid->Append(new wxBoolProperty("���������"));
	
	BtnProperty* groupProp = new BtnProperty("������");
	groupProp->SetOnClickButonFunc(selectGroup);
	mPropGrid->Append(groupProp);
	
	mPropGrid->Append(new wxLongStringProperty(L"������"));

	mPropGrid->Append(new wxStringProperty(L"������"));
	mPropGrid->Append(new wxStringProperty(L"����"));

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
				this, std::placeholders::_1, std::placeholders::_2);
			mChangeConnection = mModel->DoConnect(MClsAct::Op::AfterChange, funcOnChange);
			OnChangeModel( mModel.get(), nullptr);
		}//if (mModel)
	}//if
}//SetModel

//---------------------------------------------------------------------------
void DClsActEditor::GetData(rec::ClsActAccess& rec) const
{
	mPropGrid->CommitChangesFromEditor();

	rec.mAct.mLabel = mPropGrid->GetPropertyByLabel(L"��������")->GetValueAsString();


	wxString accessDisabled = mPropGrid->GetPropertyByLabel(L"���������")->GetValueAsString();
	rec.mAccessDisabled = (0 == accessDisabled.CmpNoCase("true")) ? "1" : "0";

	rec.mAcessGroup = mPropGrid->GetPropertyByLabel(L"������")->GetValueAsString();
	rec.mScriptRestrict = mPropGrid->GetPropertyByLabel(L"������")->GetValueAsString();
	rec.mObj.mLabel = mPropGrid->GetPropertyByLabel("������")->GetValueAsString();

	/*
	wh::ObjKeyPath path;
	path.ParsePath(mPropGrid->GetPropertyByLabel("����")->GetValueAsString());

	if (path.size())
	{
		wxString generated_path;
		path.GenerateArray(generated_path, true);
		rec.mSrcPath = generated_path;
	}
	else
		rec.mSrcPath.SetNull();
		*/
	rec.mId = mPropGrid->GetPropertyByLabel("ID")->GetValueAsString();
}
//---------------------------------------------------------------------------
void DClsActEditor::SetData(const rec::ClsActAccess& rec)
{
	mPropGrid->CommitChangesFromEditor();

	mPropGrid->GetPropertyByLabel(L"��������")->SetValueFromString(rec.mAct.mLabel.toStr());
	mPropGrid->GetPropertyByLabel(L"���������")->
		SetValueFromString(("1" == rec.mAccessDisabled) ? "true" : "false");
	mPropGrid->GetPropertyByLabel(L"������")->SetValueFromString(rec.mAcessGroup);
	mPropGrid->GetPropertyByLabel(L"������")->SetValueFromString(rec.mScriptRestrict);
	mPropGrid->GetPropertyByLabel(L"������")->SetValueFromString(rec.mObj.mLabel.toStr());
	/*
	wh::ObjKeyPath path;
	path.ParseArray(rec.mSrcPath);
	wxString pathStr;
	path.GeneratePath(pathStr);
	mPropGrid->GetPropertyByLabel("����")->SetValueFromString(pathStr);
	*/
	mPropGrid->GetPropertyByLabel(L"ID")->SetValueFromString(rec.mId.toStr());
}
//---------------------------------------------------------------------------

void DClsActEditor::OnChangeModel(const IModel* model, const MClsAct::T_Data* data)
{
	if (mModel && mModel.get() == model)
	{
		const auto state = model->GetState();
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

