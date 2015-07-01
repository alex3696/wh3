#include "_pch.h"
#include "DPropEditor.h"

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
	SetTitle("�������������� ���������� � ��������");


	mPropGrid = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_SPLITTER_AUTO_CENTER);
	GetSizer()->Insert(0, mPropGrid, 1, wxALL | wxEXPAND, 0);


	wxPGChoices soc;
	soc.Add(L"�����", 0);
	soc.Add(L"�����", 1);
	soc.Add(L"����", 2);
	soc.Add(L"������", 3);
	soc.Add(L"����", 4);

	mPropGrid->Append(new wxStringProperty(L"���", wxPG_LABEL));
	mPropGrid->Append(new wxEnumProperty(L"���", wxPG_LABEL, soc, 0));
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

	rec.mLabel = mPropGrid->GetPropertyByLabel("���")->GetValueAsString();
	rec.mType = wxString::Format("%d", mPropGrid->GetPropertyByLabel("���")->GetChoiceSelection());
	rec.mID = mPropGrid->GetPropertyByLabel("ID")->GetValueAsString();
}
//---------------------------------------------------------------------------
void DPropEditor::SetData(const rec::Prop& rec)
{
	mPropGrid->CommitChangesFromEditor();

	mPropGrid->GetPropertyByLabel(L"���")->SetValueFromString(rec.mLabel);

	unsigned long sel = 0;
	rec.mType.ToULong(&sel);
	mPropGrid->GetPropertyByLabel(L"���")->SetChoiceSelection(sel);

	mPropGrid->GetPropertyByLabel(L"ID")->SetValueFromString(rec.mID);
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
											this, std::placeholders::_1);
			mChangeConnection = mModel->ConnectChangeSlot(funcOnChange);
			OnChangeModel(*mModel.get());
		}//if (mModel)
	}//if
}//SetModel
//---------------------------------------------------------------------------

void DPropEditor::OnChangeModel(const IModel& model)
{
	if (mModel && mModel.get() == &model)
	{
		const auto state = model.GetState();
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