#include "_pch.h"
#include "TableRowEditor.h"
#include "PGPTypeId.h"

using namespace wh;


//-----------------------------------------------------------------------------
//---------------------------------------------------------------------------
// TableRowPGDefaultEditor
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TableRowPGDefaultEditor::TableRowPGDefaultEditor(wxWindow*		parent,
	wxWindowID		id,
	const wxString& title,
	const wxPoint&	pos,
	const wxSize&	size,
	long style,
	const wxString& name)
	:wxDialog(parent, id, title, pos, size, style, name), mModel(nullptr)
{
	SetTitle("Редактирование");
	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);

	mPropGrid = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_SPLITTER_AUTO_CENTER);
	szrMain->Insert(0, mPropGrid, 1, wxALL | wxEXPAND, 0);

	auto m_sdbSizer = new wxStdDialogButtonSizer();
	auto m_btnOK = new wxButton(this, wxID_OK);
	m_sdbSizer->AddButton(m_btnOK);
	auto m_btnCancel = new wxButton(this, wxID_CANCEL);
	m_sdbSizer->AddButton(m_btnCancel);
	m_sdbSizer->Realize();
	szrMain->Add(m_sdbSizer, 0, wxALL | wxEXPAND, 10);

	this->SetSizer(szrMain);
	this->Layout();

	//std::function<void(wxCommandEvent&)>	onOk = [this](wxCommandEvent& evt){		//evt.Skip();	};
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &TableRowPGDefaultEditor::OnCmdCancel,this, wxID_CANCEL);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &TableRowPGDefaultEditor::OnCmdOk, this, wxID_OK);
}
//-----------------------------------------------------------------------------
void TableRowPGDefaultEditor::OnCmdCancel(wxCommandEvent& evt)
{
	this->SetModel(std::shared_ptr<ITableRow>(nullptr));
	evt.Skip();
}
//-----------------------------------------------------------------------------
void TableRowPGDefaultEditor::OnCmdOk(wxCommandEvent& evt)
{
	if (mModel)
	{
		auto rec = mModel->GetData();
		GetData(rec);
		mModel->SetData(rec);
		this->SetModel(std::shared_ptr<ITableRow>(nullptr));
	}
	evt.Skip();
}

//-----------------------------------------------------------------------------
void TableRowPGDefaultEditor::SetModel(std::shared_ptr<ITableRow>& newModel)
{
	mChangeConnection.disconnect();
	mModel = std::dynamic_pointer_cast<ITableRow>(newModel);
	if (!mModel)
		return;
	auto table = dynamic_cast<ITable*>(mModel->GetParent());
	if (!table)
		return;

	auto funcOnChange = std::bind(&TableRowPGDefaultEditor::OnChangeModel,
		this, std::placeholders::_1, std::placeholders::_2);
	mChangeConnection = mModel->DoConnect(moAfterUpdate, funcOnChange);

	BaseGroup bg = whDataMgr::GetInstance()->mCfg.Prop.mBaseGroup;
	if ((int)bg < (int)bgTypeDesigner)
		this->GetWindowChild(wxID_OK)->Enable(false);//	m_btnOK

	mPropGrid->Clear();
	const auto& field_vec = table->mFieldVec;
	for (unsigned int i = 0; i < field_vec->size(); ++i)
	{
		const auto& field = field_vec->at(i)->GetData();
		wxPGProperty* pgp = nullptr;
		switch (field.mType)
		{
		case ftText:	pgp = mPropGrid->Append(new wxLongStringProperty(field.mTitle)); break;
		case ftName:	pgp = mPropGrid->Append(new wxStringProperty(field.mTitle)); break;
		case ftLong:	
			if (FieldEditor::Type == field.mEditor)
			{
				pgp = mPropGrid->Append(new wxPGPSmallType(field.mTitle));
				//auto chs = pgp->GetChoices();
				//chs.Insert(wxEmptyString, 0, -1);
			}
			else if (FieldEditor::Normal == field.mEditor)
			{
				pgp = mPropGrid->Append(new wxStringProperty(field.mTitle));
				pgp->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
			}
			break;
		case ftDouble:	pgp = mPropGrid->Append(new wxFloatProperty(field.mTitle));  break;
		case ftDate:	pgp = mPropGrid->Append(new wxDateProperty(field.mTitle));  break;
		case ftLink:	pgp = mPropGrid->Append(new wxStringProperty(field.mTitle));  break;
		case ftFile:	pgp = mPropGrid->Append(new wxStringProperty(field.mTitle));  break;
		case ftJSON:	pgp = mPropGrid->Append(new wxLongStringProperty(field.mTitle));  break;
		default:break;
		}
		if (pgp)
			pgp->Enable(field.mGuiEdit);

	}
	
	OnChangeModel(dynamic_cast<IModel*>(mModel.get()), &mModel->GetData());
}//SetModel
//---------------------------------------------------------------------------
void TableRowPGDefaultEditor::GetData(TableRowData& rec) const
{
	mPropGrid->CommitChangesFromEditor();
	auto table = dynamic_cast<ITable*>(mModel->GetParent());
	if (!table)
		return;
	const auto& field_vec = table->mFieldVec;
	rec.resize(field_vec->size());
	for (unsigned int i = 0; i < field_vec->size(); ++i)
	{
		const auto& field = field_vec->at(i)->GetData();
		auto pgp = mPropGrid->GetPropertyByLabel(field.mTitle);
		if (pgp)
		{
			if (ftLong == field.mType && field.mEditor == FieldEditor::Type)
			{
				int cs = pgp->GetChoiceSelection();
				int ftype = pgp->GetChoices().GetValue(cs);
				if (-1 != ftype)
					rec[i] = wxString::Format("%d", ftype);
			}
			else
				rec[i] = pgp->GetValueAsString();
		}
			
	}
}
//---------------------------------------------------------------------------
void TableRowPGDefaultEditor::SetData(const TableRowData& rec)
{
	mPropGrid->CommitChangesFromEditor();
	auto table = dynamic_cast<ITable*>(mModel->GetParent());
	if (!table)
		return;

	const auto& field_vec = table->mFieldVec;
	for (unsigned int i = 0; i < field_vec->size(); ++i)
	{
		const auto& field = field_vec->at(i)->GetData();
		auto pgp = mPropGrid->GetPropertyByLabel(field.mTitle);
		if (pgp && rec.size() > i)
		{
			const auto& val = rec[i];
			pgp->SetValueFromString(val);
		}
		
	}
}
//---------------------------------------------------------------------------
void TableRowPGDefaultEditor::OnChangeModel(const IModel* model, const TableRowData* data)
{
	if (!model || !data)
		return;
	(msNull == model->GetState()) ? SetData(TableRowData()) : SetData(*data);
	//const auto state = model->GetState();
	//if (msNull == state)
	//	SetData(TableRowData());
	//else
	//	SetData(*data);
}
