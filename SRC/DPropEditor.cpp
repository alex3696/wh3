#include "_pch.h"
#include "DPropEditor.h"
#include "PGClsPid.h"

using namespace wh;
using namespace wh::view;


//-----------------------------------------------------------------------------
//---------------------------------------------------------------------------
//DPropEditor
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
DPropEditor::DPropEditor(wxWindow*		parent,
	wxWindowID		id,
	const wxString& title,
	const wxPoint&	pos,
	const wxSize&	size,
	long style,
	const wxString& name)
	:DlgBaseOkCancel(parent, id, title, pos, size, style, name), mModel(nullptr)
{
	SetTitle("Редактирование информации о свойстве");


	mPropGrid = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_SPLITTER_AUTO_CENTER);
	GetSizer()->Insert(0, mPropGrid, 1, wxALL | wxEXPAND, 0);


	wxPGChoices soc;

	for (const auto& ft : gFieldTypeArray)
		soc.Add(ft.mTitle, ft.mType);

	auto lbl = mPropGrid->Append(new wxStringProperty(L"Имя", wxPG_LABEL));

	//\p{ Punct }	Punctuation: One of !"#$%&'()*+,-./:;<=>?@[\]^_`{|}~
	//wxRegEx re;
	//bool valid;
	//valid = re.Compile("^([\\s\\w!()*+,-.:;<=>^_|№])+$", wxRE_ADVANCED);
	//valid = re.Compile("^([[:alnum:][:space:]!()*+,-.:;<=>^_|№])+$", wxRE_DEFAULT);
	//bool match = re.Matches("1q|w.u*i-h+h:%j;e");
	//if (match)
	//	wxMessageBox("OK");

	wxRegExpValidator vl("^([[:alnum:][:space:]!()*+,-.:;<=>^_|№])+$"); //wxRE_DEFAULT
	lbl->SetValidator(vl);

	mPropGrid->Append(new wxEnumProperty(L"Тип", wxPG_LABEL, soc, 0));
	mPropGrid->Append(new wxStringProperty(L"ID", wxPG_LABEL))->Enable(false);

	//mToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, 
	//wxDefaultSize, wxAUI_TB_DEFAULT_STYLE);
	//GetSizer()->Insert(0, mToolBar, 0, wxEXPAND, 0);

	this->Layout();
	
}
//---------------------------------------------------------------------------
void DPropEditor::GetData(rec::Prop& rec) const
{
	mPropGrid->CommitChangesFromEditor();

	rec.mLabel = mPropGrid->GetPropertyByLabel("Имя")->GetValueAsString();

	int cs = mPropGrid->GetPropertyByLabel("Тип")->GetChoiceSelection();
	rec.mType = (FieldType)mPropGrid->GetPropertyByLabel("Тип")->GetChoices().GetValue(cs);
	rec.mId = mPropGrid->GetPropertyByLabel("ID")->GetValueAsString();
}
//---------------------------------------------------------------------------
void DPropEditor::SetData(const rec::Prop& rec)
{
	mPropGrid->CommitChangesFromEditor();
	wxWindowUpdateLocker	wndLockUpdater(mPropGrid);

	mPropGrid->GetPropertyByLabel(L"Имя")->SetValueFromString(rec.mLabel);

	auto pgPropType = mPropGrid->GetPropertyByLabel(L"Тип");
	const auto& choices = pgPropType->GetChoices();
	for (unsigned int i = 0; i < choices.GetCount(); ++i)
	{
		if (choices[i].GetValue() == (int)rec.mType )
		{ 
			pgPropType->SetChoiceSelection(i);
			pgPropType->SetValue(choices[i].GetText());
			break;
		}
	}

	mPropGrid->GetPropertyByLabel(L"ID")->SetValueFromString(rec.mId.toStr());
}
//-----------------------------------------------------------------------------
void DPropEditor::SetModel(std::shared_ptr<IModel>& newModel)
{
	if (newModel != mModel)
	{
		mChangeConnection.disconnect();
		mModel = std::dynamic_pointer_cast<T_Model>(newModel);
		if (mModel)
		{
			auto funcOnChange =	std::bind(&DPropEditor::OnChangeModel, 
				this, std::placeholders::_1, std::placeholders::_2);
			mChangeConnection = mModel->DoConnect(moAfterUpdate, funcOnChange);
			OnChangeModel(mModel.get(), nullptr);

			BaseGroup bg = whDataMgr::GetInstance()->mCfg.Prop.mBaseGroup;
			if ((int)bg < (int)bgTypeDesigner)
				m_btnOK->Enable(false);
		}//if (mModel)
	}//if
}//SetModel
//---------------------------------------------------------------------------

void DPropEditor::OnChangeModel(const IModel* model, const T_Model::T_Data* data)
{
	if (mModel && mModel.get() == model)
	{
		const auto state = model->GetState();
		const auto& rec = mModel->GetData();
		SetData(rec);
	}
	
}
//---------------------------------------------------------------------------
void DPropEditor::UpdateModel()const
{
	if (mModel)
	{
		auto rec = mModel->GetData();
		GetData(rec);
		mModel->SetData(rec);
	}
}
//---------------------------------------------------------------------------
int DPropEditor::ShowModal()
{
	return DlgBaseOkCancel::ShowModal();
}