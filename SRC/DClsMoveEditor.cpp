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
	:wxDialog(parent, id, title, pos, size, style, name), mModel(nullptr)
{
	mMovPattern.reset(new temppath::model::Array());
	mSrcPatternPath.reset(new temppath::model::Array());
	mDstPatternPath.reset(new temppath::model::Array());

	this->SetSize(600, 400);
	this->SetMinSize(wxSize(600, 400));
	SetTitle("Редактирование информации о разрешении перемещения");
	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);

	auto mMovPathLabel = new wxStaticText(this, wxID_ANY, "Перемещаемый объект");
	mMovPathLabel->Wrap(-1);
	szrMain->Add(mMovPathLabel, 0, wxALL, 5);

	mMovEditor = new PathPatternEditor(this);
	szrMain->Add(mMovEditor, 0, wxALL | wxEXPAND, 1);

	auto mSrcPathLabet = new wxStaticText(this, wxID_ANY, "Источник");
	mSrcPathLabet->Wrap(-1);
	szrMain->Add(mSrcPathLabet, 0, wxALL, 5);

	mSrcPathEditor = new PathPatternEditor(this);
	szrMain->Add(mSrcPathEditor, 0, wxALL | wxEXPAND, 1);

	auto mDstPathLabet = new wxStaticText(this, wxID_ANY, "Приёмник");
	mDstPathLabet->Wrap(-1);
	szrMain->Add(mDstPathLabet, 0, wxALL, 5);

	mDstPathEditor = new PathPatternEditor(this);
	szrMain->Add(mDstPathEditor, 0, wxALL | wxEXPAND, 1);

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
				prop->SetValueFromString(groupData.mLabel);
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
	mPropGrid->Append(new wxStringProperty(L"ID"))->Enable(false);
	
	this->Layout();
}
//---------------------------------------------------------------------------
void DClsMoveEditor::SetModel(std::shared_ptr<IModel>& newModel)
{
	if (newModel == mModel)
		return;

	mChangeConnection.disconnect();
	mModel = std::dynamic_pointer_cast<MClsMove>(newModel);
	
	if (!mModel)
		return;
	
	auto funcOnChange = std::bind(&DClsMoveEditor::OnChangeModel,
		this, std::placeholders::_1, std::placeholders::_2);
	mChangeConnection = mModel->DoConnect(moAfterUpdate, funcOnChange);
	OnChangeModel(mModel.get(), nullptr);

}//SetModel

//---------------------------------------------------------------------------
void DClsMoveEditor::GetData(rec::ClsSlotAccess& rec) const
{
	mPropGrid->CommitChangesFromEditor();
	wxString accessDisabled = mPropGrid->GetPropertyByLabel(L"Запретить")->GetValueAsString();
	rec.mAccessDisabled = (0 == accessDisabled.CmpNoCase("true")) ? "1" : "0";
	rec.mAcessGroup = mPropGrid->GetPropertyByLabel(L"Группа")->GetValueAsString();
	rec.mScriptRestrict = mPropGrid->GetPropertyByLabel(L"Скрипт")->GetValueAsString();
	rec.mId = mPropGrid->GetPropertyByLabel("ID")->GetValueAsString();


	if (mMovPattern->GetChildQty())
	{
		const auto& mov = mMovPattern->at(0)->GetData();
		rec.mCls = mov.mCls;
		rec.mObj = mov.mObj;
	}

	auto srcLastItem = mSrcPatternPath->GetLast();
	if (srcLastItem)
	{
		const auto& srcLastItemData = srcLastItem->GetData();
		rec.mSrcCls = srcLastItemData.mCls;
		rec.mSrcObj = srcLastItemData.mObj;
	}
	rec.mSrcArrId = mSrcPatternPath->GetArr2Id(false);
	rec.mSrcArrTitle = mSrcPatternPath->GetArr2Title(false);

	auto dstLastItem = mDstPatternPath->GetLast();
	if (dstLastItem)
	{
		const auto& dstLastItemData = dstLastItem->GetData();
		rec.mDstCls = dstLastItemData.mCls;
		rec.mDstObj = dstLastItemData.mObj;
	}
	rec.mDstArrId = mDstPatternPath->GetArr2Id(false);
	rec.mDstArrTitle = mDstPatternPath->GetArr2Title(false);

	
}
//---------------------------------------------------------------------------
void DClsMoveEditor::SetData(const rec::ClsSlotAccess& rec)
{
	if (!mModel)
		return;

	mPropGrid->CommitChangesFromEditor();

	mPropGrid->GetPropertyByLabel(L"Запретить")->	SetValueFromString(("1" == rec.mAccessDisabled) ? "true" : "false");
	mPropGrid->GetPropertyByLabel(L"Группа")->SetValueFromString(rec.mAcessGroup);
	mPropGrid->GetPropertyByLabel(L"Скрипт")->SetValueFromString(rec.mScriptRestrict);
	mPropGrid->GetPropertyByLabel(L"ID")->SetValueFromString(rec.mId);

	namespace cat = wh::object_catalog;
	auto permArr = mModel->GetParent();
	if (!permArr)
		return;
	auto clsIModel = permArr->GetParent();
	auto clsModel = dynamic_cast<cat::MTypeItem*>(clsIModel);
	if (!clsModel)
		return;

	// mov
	rec::PathNode movItemData(rec.mCls.mId, rec.mCls.mLabel,
								rec.mObj.mId, rec.mObj.mLabel);
	auto movItemModel = mMovPattern->CreateItem(movItemData);
	mMovPattern->Insert(movItemModel);
	mMovEditor->SetMode(PathPatternEditor::FixOne_ReqCls);
	mMovEditor->SetModel(mMovPattern);

	// srcPath = srcPath || {%} + [srcCls]srcObj + [movCls]movObj
	mSrcPatternPath->SetArr2Id2Title(rec.mSrcArrId, rec.mSrcArrTitle);
	rec::PathNode srcItemData(rec.mSrcCls.mId, rec.mSrcCls.mLabel,
								rec.mSrcObj.mId, rec.mSrcObj.mLabel);
	auto srcItemModel = mSrcPatternPath->CreateItem(srcItemData);
	mSrcPatternPath->Insert(srcItemModel);
	mSrcPathEditor->SetMode(PathPatternEditor::ReqOne_ReqCls);
	mSrcPathEditor->SetModel(mSrcPatternPath);

	// dstPath = dstPath || {%}+ [dstCls]dstObj 
	mDstPatternPath->SetArr2Id2Title(rec.mDstArrId, rec.mDstArrTitle);
	const auto& clsData = clsModel->GetData();
	rec::PathNode dstItemData(clsData.mId, clsData.mLabel,
								rec.mDstObj.mId, rec.mDstObj.mLabel);
	auto dstItemModel = mDstPatternPath->CreateItem(dstItemData);
	mDstPatternPath->Insert(dstItemModel);
	mDstPathEditor->SetMode(PathPatternEditor::ReqOne_FixCls);
	mDstPathEditor->SetModel(mDstPatternPath);
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
	return wxDialog::ShowModal();
}

