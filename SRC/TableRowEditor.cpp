#include "_pch.h"
#include "TableRowEditor.h"
#include "PGPTypeId.h"

using namespace wh;

//-----------------------------------------------------------------------------
wxArrayString wh::Sql2ArrayString(const wxString& str_sql_value)
{
	wxArrayString arr;

	wxRegEx cut_arr("([^{|}$]+)");
	if (cut_arr.Matches(str_sql_value))
	{
		wxString curr = cut_arr.GetMatch(str_sql_value);

		wxRegEx re("([^,]+|\"[^\"]*\")");
		size_t start = 0;

		while (start < curr.Len())
		{
			curr = curr.Mid(start); //SubString(start, str_sql_value.Len());
			if (re.Matches(curr))
			{
				wxString s = re.GetMatch(curr);
				arr.push_back(s);
				start = s.Len() + 1;
			}
		}
	}

	return arr;
}
//-----------------------------------------------------------------------------
wxString wh::ArrayString2Sql(const wxArrayString& arr)
{
	/*
	wxString email="user@host.net"; 
	// bugly wxRegEx reEmail = "([^@]+)@([[:alnum:].-_].)+([[:alnum:]]+)";
	wxRegEx reEmail = "([^@]+)@([[:alnum:]\\-_]+).([[:alnum:]]+)";
	if (reEmail.Matches(email))
	{
		auto qty = reEmail.GetMatchCount();
		wxString text =     reEmail.GetMatch(email);
		wxString username = reEmail.GetMatch(email, 1);
		wxString domen =    reEmail.GetMatch(email, 2);
		wxString country =  reEmail.GetMatch(email, 3);
	}
	*/

	wxString str_sql_value;
	for (const auto& it : arr)
	{
		wxString sr = it;
		sr.Trim(true);
		sr.Trim(false);
		if (sr.size() > 0)
		{
			if ('\"' == sr[0] && '\"' == sr.Last())
				sr = sr.substr(1, sr.size() - 2);//wxRegEx cut_quote("\"(.*)\"");
			if (-1 == sr.Find(","))
				str_sql_value << sr << ",";
			else
				str_sql_value << "\"" << sr << "\",";
		}
		else
			str_sql_value << "\"\",";
	}
	str_sql_value.RemoveLast();
	str_sql_value = "{" + str_sql_value + "}";
	return str_sql_value;
}
//-----------------------------------------------------------------------------
bool wh::Sql2Bool(const wxString& sql_string)
{
	bool b = (0==sql_string.CmpNoCase("true") || 0==sql_string.CmpNoCase("t"));
	return b;
}
//-----------------------------------------------------------------------------
wxString wh::Bool2Sql(bool bool_value)
{
	return bool_value ? "TRUE" : "FALSE";
}


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
	SetTitle("–едактирование");
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
	this->SetModel(std::shared_ptr<IModel>(nullptr));
	evt.Skip();
}
//-----------------------------------------------------------------------------
void TableRowPGDefaultEditor::OnCmdOk(wxCommandEvent& evt)
{
	if (mModel)
	{
		TableRowData rec;
		if (msNull != mModel->GetState())
			rec = mModel->GetData();
		GetData(rec);
		mModel->SetData(rec);
		
		// обнул€ем модель редактора
		SetModel(std::shared_ptr<IModel>(nullptr));
	}
	evt.Skip();
}

//-----------------------------------------------------------------------------
void TableRowPGDefaultEditor::SetModel(std::shared_ptr<IModel>& newModel)
{
	mChangeConnection.disconnect();
	mModel = std::dynamic_pointer_cast<ITableRow>(newModel);
	if (!mModel)
		return;
	auto data_arr = mModel->GetParent();
	if (!data_arr)
		return;
	auto table = dynamic_cast<ITable*>(data_arr->GetParent());
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
		case ftBool:	pgp = mPropGrid->Append(new wxBoolProperty(field.mTitle));  
			pgp->SetAttribute(wxPG_BOOL_USE_CHECKBOX,true);
			break;
		case ftTextArray:pgp = mPropGrid->Append(new wxArrayStringProperty(field.mTitle));  break;
		default:break;
		}
		if (pgp)
			pgp->Enable(field.mGuiEdit);

	}
	
	if (msNull != mModel->GetState())
		OnChangeModel(dynamic_cast<IModel*>(mModel.get()), &mModel->GetData());
}//SetModel
//---------------------------------------------------------------------------
void TableRowPGDefaultEditor::GetData(TableRowData& rec) const
{
	mPropGrid->CommitChangesFromEditor();
	if (!mModel)
		return;
	auto data_arr = mModel->GetParent();
	if (!data_arr)
		return;
	auto table = dynamic_cast<ITable*>(data_arr->GetParent());
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
			switch (field.mType)
			{
			case ftLong:
				if (field.mEditor == FieldEditor::Type)
				{
					int cs = pgp->GetChoiceSelection();
					int ftype = pgp->GetChoices().GetValue(cs);
					if (-1 != ftype)
						rec[i] = wxString::Format("%d", ftype);
				}
				else
					rec[i] = pgp->GetValueAsString();
				break;
			case ftTextArray:
				{
					wxArrayString arr = pgp->GetValue().GetArrayString();
					rec[i] = ArrayString2Sql(arr);
				}
				break;
			case ftBool:rec[i] = Bool2Sql(pgp->GetValue().GetBool());	
				break;
			default:	rec[i] = pgp->GetValueAsString();				
				break;
			}//switch (field.mType)
		}//if (pgp)
	}//for
}
//---------------------------------------------------------------------------
void TableRowPGDefaultEditor::SetData(const TableRowData& rec)
{
	mPropGrid->CommitChangesFromEditor();
	if (!mModel)
		return;
	auto data_arr = mModel->GetParent();
	if (!data_arr)
		return;
	auto table = dynamic_cast<ITable*>(data_arr->GetParent());
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
			switch (field.mType)
			{
			case ftTextArray:
				pgp->SetValue(Sql2ArrayString(rec[i]));
				break;
			case ftBool:
				pgp->SetValue(Sql2Bool(rec[i]));
				break;
			case ftLong:
			default:	
				pgp->SetValueFromString(val);
				break;
			}//switch (field.mType)
		}//if (pgp && rec.size() > i)
		
	}//for
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
