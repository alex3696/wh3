#include "_pch.h"
#include "TableRowEditor.h"

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

	std::function<void(wxCommandEvent&)>
	onExit = [this](wxCommandEvent& evt)
	{
		this->SetModel(std::shared_ptr<ITableRow>(nullptr));
		evt.Skip();
	};
	
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, onExit, wxID_CANCEL);
}
//-----------------------------------------------------------------------------
void TableRowPGDefaultEditor::SetModel(std::shared_ptr<ITableRow>& newModel)
{
	if (newModel == mModel)
		return;
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
	for (unsigned int i = 0; i < field_vec.size(); ++i)
	{
		const auto& field = field_vec.at(i)->GetData();
		wxPGProperty* pgp = nullptr;
		switch (field.mType)
		{
		case ftText:	pgp = mPropGrid->Append(new wxLongStringProperty(field.mTitle)); break;
		case ftName:	pgp = mPropGrid->Append(new wxStringProperty(field.mTitle)); break;
		case ftLong:	pgp = mPropGrid->Append(new wxIntProperty(field.mTitle));  break;
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
	DataToWindow();
}//SetModel
//---------------------------------------------------------------------------
void TableRowPGDefaultEditor::GetData(TableRowData& rec) const
{
	mPropGrid->CommitChangesFromEditor();
	auto table = dynamic_cast<ITable*>(mModel->GetParent());
	if (!table)
		return;
	const auto& field_vec = table->mFieldVec;
	rec.resize(field_vec.size());
	for (unsigned int i = 0; i < field_vec.size(); ++i)
	{
		const auto& field = field_vec.at(i)->GetData();
		auto pgp = mPropGrid->GetPropertyByLabel(field.mTitle);
		if (pgp)
			rec[i] = pgp->GetValueAsString();
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
	for (unsigned int i = 0; i < field_vec.size(); ++i)
	{
		const auto& field = field_vec.at(i)->GetData();
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
	if (mModel && mModel.get() == model)
		DataToWindow();
}
//---------------------------------------------------------------------------
void TableRowPGDefaultEditor::DataFromWindow()
{
	if (mModel)
	{
		auto rec = mModel->GetData();
		GetData(rec);
		mModel->SetData(rec);
	}
}
//---------------------------------------------------------------------------
void TableRowPGDefaultEditor::DataToWindow()
{
	if (!mModel)
		return;
	const auto state = mModel->GetState();

	if (msNull == state)
		SetData(TableRowData());
	else
		SetData(mModel->GetData());
}