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
	:wxDialog(parent, id, title, pos, size, style, name), mModel(nullptr)
{
	this->SetSize(500, 300);
	this->SetMinSize(wxSize(500, 300));
	SetTitle("Редактирование информации о разрешении действия");
	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);

	auto mTextPath = new wxStaticText(this, wxID_ANY, "Место(путь) объекта во время выполнения действия");
	mTextPath->Wrap(-1);
	szrMain->Add(mTextPath, 0, wxALL, 5);


	mPathEditor = new PathPatternEditor(this);
	szrMain->Add(mPathEditor, 0, wxALL | wxEXPAND, 1);
	
	auto mTextProp = new wxStaticText(this, wxID_ANY, "Атрибуты");
	mTextProp->Wrap(-1);
	szrMain->Add(mTextProp, 0, wxALL, 5);


	mPropGrid = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_SPLITTER_AUTO_CENTER);
	szrMain->Add(mPropGrid, 1, wxALL | wxEXPAND, 0);

	
	m_sdbSizer = new wxStdDialogButtonSizer();
	m_btnOK = new wxButton(this, wxID_OK);//,"Сохранить и закрыть" );
	m_sdbSizer->AddButton(m_btnOK);
	m_btnCancel = new wxButton(this, wxID_CANCEL);//," Закрыть" );
	m_sdbSizer->AddButton(m_btnCancel);
	m_sdbSizer->Realize();
	szrMain->Add(m_sdbSizer, 0, wxALL | wxEXPAND, 10);
	this->SetSizer(szrMain);



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


	BtnProperty* btnProp = new BtnProperty("Действие");
	btnProp->SetOnClickButonFunc(selecAct);
	mPropGrid->Append(btnProp);

	mPropGrid->Append(new wxBoolProperty("Запретить"));
	
	BtnProperty* groupProp = new BtnProperty("Группа");
	groupProp->SetOnClickButonFunc(selectGroup);
	mPropGrid->Append(groupProp);
	
	mPropGrid->Append(new wxLongStringProperty(L"Скрипт"));

	//mPropGrid->Append(new wxStringProperty(L"Объект"));
	//mPropGrid->Append(new wxStringProperty(L"Путь"));

	mPropGrid->Append(new wxStringProperty(L"ID"))->Enable(false);



	this->Layout();
}
//---------------------------------------------------------------------------
void DClsActEditor::SetModel(std::shared_ptr<IModel>& newModel)
{
	namespace ph = std::placeholders;
	namespace cat = wh::object_catalog;

	if (newModel == mModel)
		return;
	
	mChangeConnection.disconnect();
	mModel = std::dynamic_pointer_cast<MClsAct>(newModel);
	
	if (!mModel)
		return;

			
	auto onChangePerm = std::bind(&DClsActEditor::OnChangeModel, this, ph::_1, ph::_2);
	mChangeConnection = mModel->DoConnect(moAfterUpdate, onChangePerm);
	OnChangeModel(mModel.get(), nullptr);

	auto permArr = mModel->GetParent();
	if (!permArr)
		return;

	auto clsIModel = permArr->GetParent();
	auto clsModel = dynamic_cast<cat::MTypeItem*>(clsIModel);
	const auto& clsData = clsModel->GetData();

	auto srcPathArr = mModel->mSrcPathArr;

	if ( 0 == srcPathArr->GetChildQty() )
	{ 
		
		auto anyItem = std::make_shared<temppath::model::Item>();
		auto linkedCls = std::make_shared<temppath::model::Item>();
		
		rec::PathNode pn;
		anyItem->SetData(pn);
		
		pn.mCls.mId = clsData.mId;
		pn.mCls.mLabel = clsData.mLabel;
		linkedCls->SetData(pn);

		srcPathArr->AddChild(anyItem);
		srcPathArr->AddChild(linkedCls);
	}
	mPathEditor->SetModel(srcPathArr);
	mPathEditor->DoNotDeleteLast(true);
	

}//SetModel

//---------------------------------------------------------------------------
void DClsActEditor::GetData(rec::ClsActAccess& rec) const
{
	mPropGrid->CommitChangesFromEditor();

	rec.mAct.mLabel = mPropGrid->GetPropertyByLabel(L"Действие")->GetValueAsString();


	wxString accessDisabled = mPropGrid->GetPropertyByLabel(L"Запретить")->GetValueAsString();
	rec.mAccessDisabled = (0 == accessDisabled.CmpNoCase("true")) ? "1" : "0";

	rec.mAcessGroup = mPropGrid->GetPropertyByLabel(L"Группа")->GetValueAsString();
	rec.mScriptRestrict = mPropGrid->GetPropertyByLabel(L"Скрипт")->GetValueAsString();
	

	/*
	rec.mObj.mLabel = mPropGrid->GetPropertyByLabel("Объект")->GetValueAsString();
	wh::ObjKeyPath path;
	path.ParsePath(mPropGrid->GetPropertyByLabel("Путь")->GetValueAsString());

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

	mPropGrid->GetPropertyByLabel(L"Действие")->SetValueFromString(rec.mAct.mLabel.toStr());
	mPropGrid->GetPropertyByLabel(L"Запретить")->
		SetValueFromString(("1" == rec.mAccessDisabled) ? "true" : "false");
	mPropGrid->GetPropertyByLabel(L"Группа")->SetValueFromString(rec.mAcessGroup);
	mPropGrid->GetPropertyByLabel(L"Скрипт")->SetValueFromString(rec.mScriptRestrict);
	
	/*
	mPropGrid->GetPropertyByLabel(L"Объект")->SetValueFromString(rec.mObj.mLabel.toStr());
	wh::ObjKeyPath path;
	path.ParseArray(rec.mSrcPath);
	wxString pathStr;
	path.GeneratePath(pathStr);
	mPropGrid->GetPropertyByLabel("Путь")->SetValueFromString(pathStr);
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
	//(*data).mId = "ghgfhf";
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
	return wxDialog::ShowModal();
}
