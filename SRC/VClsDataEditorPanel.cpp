#include "_pch.h"
#include "VClsDataEditorPanel.h"
#include "PGClsPid.h"
#include "dlgselectcls_ctrlpnl.h"

using namespace wh;
using namespace wh::view;


//-----------------------------------------------------------------------------
//---------------------------------------------------------------------------
//VClsDataEditorPanel
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
VClsDataEditorPanel::VClsDataEditorPanel(wxWindow*		parent,
	wxWindowID		id,
	const wxPoint&	pos,
	const wxSize&	size,
	long style,
	const wxString& name)
	:wxPanel(parent, id, pos, size, style, name), mModel(nullptr)
{
	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(szrMain);

	mPropGrid = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_SPLITTER_AUTO_CENTER);
	GetSizer()->Insert(0, mPropGrid, 1, wxALL | wxEXPAND, 0);

	wxPGChoices soc;
	soc.Add(L"�����������", 0);
	soc.Add(L"��������", 1);
	soc.Add(L"��������������(�������������)", 2);
	soc.Add(L"��������������(�������)", 3);

	auto pgp_name = mPropGrid->Append(new wxStringProperty(L"���"));
	mPropGrid->Append(new wxLongStringProperty(L"��������"));
	mPropGrid->Append(new wxEnumProperty(L"��� �����������", wxPG_LABEL, soc, 0));
	auto pgp_measure = mPropGrid->Append(new wxStringProperty(L"��.���������", wxPG_LABEL));

	pgp_name->SetValidator(wxRegExpValidator(titleValidator));
	pgp_measure->SetValidator(wxRegExpValidator(titleValidator));
	
	auto clsparent = new wxClsParentProperty(L"������������ �����");
	mPropGrid->Append(clsparent);
	mPropGrid->Append(new wxStringProperty(L"#"))->Enable(false);
		
	std::function<bool(wxPGProperty*)> selecFunc = [this](wxPGProperty* prop)
	{
		select::ClsDlg dlg(nullptr);

		auto catalog = std::make_shared<wh::object_catalog::MObjCatalog>();
		catalog->SetCatalog(false, true, false, "1");

		wh::rec::PathItem root;
		root.mCls.mID = 1;
		catalog->SetData(root);

		catalog->Load();

		dlg.SetModel(catalog);

		if (wxID_OK == dlg.ShowModal())
		{
			/*
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
			*/
		}
		
		return false;
	};

	clsparent->SetOnClickButonFunc(selecFunc);


	mPropGrid->ResetColumnSizes();
	this->Layout();

	Bind(wxEVT_PG_CHANGED, &VClsDataEditorPanel::OnClassTypeChange, this);
}
//---------------------------------------------------------------------------
void VClsDataEditorPanel::GetData(rec::Cls& rec) const
{
	mPropGrid->CommitChangesFromEditor();

	rec.mLabel = mPropGrid->GetPropertyByLabel(L"���")->GetValueAsString();
	rec.mComment = mPropGrid->GetPropertyByLabel(L"��������")->GetValueAsString();
	rec.mType = wxString::Format("%d", mPropGrid->GetPropertyByLabel(L"��� �����������")->GetChoiceSelection());
	rec.mMeasure = mPropGrid->GetPropertyByLabel(L"��.���������")->GetValueAsString();
	rec.mParent << mPropGrid->GetPropertyByLabel(L"������������ �����")->GetValue();
	rec.mID = mPropGrid->GetPropertyByLabel(L"#")->GetValueAsString();
}
//---------------------------------------------------------------------------
void VClsDataEditorPanel::SetData(const rec::Cls& rec)
{
	mPropGrid->CommitChangesFromEditor();

	mPropGrid->GetPropertyByLabel(L"���")->SetValueFromString(rec.mLabel);
	mPropGrid->GetPropertyByLabel(L"��������")->SetValueFromString(rec.mComment);

	unsigned long items_type = 0;
	rec.mType.ToULong(&items_type);
	mPropGrid->GetPropertyByLabel(L"��� �����������")->SetChoiceSelection(items_type);
	mPropGrid->GetPropertyByLabel(L"��.���������")->SetValueFromString(rec.mMeasure);

	mPropGrid->GetPropertyByLabel(L"������������ �����")->SetValue(wxVariant(rec.mParent));
	mPropGrid->GetPropertyByLabel(L"#")->SetValueFromString(rec.mID);
}
//-----------------------------------------------------------------------------
void VClsDataEditorPanel::SetModel(std::shared_ptr<IModel>& newModel)
{
	if (newModel != mModel)
	{
		mChangeConnection.disconnect();
		mModel = std::dynamic_pointer_cast<T_Model>(newModel);
		if (mModel)
		{
			auto funcOnChange = std::bind(&VClsDataEditorPanel::OnChangeModel,
				this, std::placeholders::_1, std::placeholders::_2);
			mChangeConnection = mModel->DoConnect(T_Model::Op::AfterChange, funcOnChange);
			OnChangeModel(mModel.get(), nullptr);
		}//if (mModel)
	}//if
}//SetModel
//---------------------------------------------------------------------------
void VClsDataEditorPanel::OnChangeModel(const IModel* model, const T_Model::T_Data* data)
{
	if (mModel && mModel.get() == model)
	{
		const auto state = model->GetState();
		const auto& rec = mModel->GetData();
		SetData(rec);

		switch (state)
		{
		default: //msNull
			mPropGrid->SetForegroundColour(wxColour(255, 255, 255)); break;
		case msCreated: mPropGrid->SetForegroundColour(wxColour(230, 240, 255)); break;
		case msDeleted: mPropGrid->SetForegroundColour(wxColour(255, 230, 230)); break;
		case msExist:   mPropGrid->SetForegroundColour(wxColour(240, 255, 240)); break;
		case msUpdated: mPropGrid->SetForegroundColour(wxColour(255, 240, 200)); break;
		}

	}

}
//---------------------------------------------------------------------------
void VClsDataEditorPanel::UpdateModel()const
{
	if (mModel)
	{
		auto rec = mModel->GetData();
		GetData(rec);
		mModel->SetData(rec);
	}
}
//---------------------------------------------------------------------------
void VClsDataEditorPanel::OnClassTypeChange(wxPropertyGridEvent& evt)
{
	UpdateModel();

}