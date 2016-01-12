#include "_pch.h"
#include "MAct.h"

using namespace wh;




const std::vector<Field> gActFieldVec = {
		{ "Имя", FieldType::ftName, true },
		{ "Описание", FieldType::ftText, true },
		{ "Цвет", FieldType::ftName, true },
		{ "ID", FieldType::ftLong, true },
		{ "VID", FieldType::ftLong, true }
};







//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MAct::MAct(const char option)
	:TModelData<rec::Act>(option)
{
}
//-------------------------------------------------------------------------
std::shared_ptr<MActPropArray> MAct::GetPropArray()
{
	if (!mPropArray)
	{
		mPropArray.reset(new MActPropArray);
		std::shared_ptr<IModel> item = std::dynamic_pointer_cast<IModel>(mPropArray);
		this->Insert(item);
	}
	return mPropArray;
}
//-------------------------------------------------------------------------
bool MAct::GetSelectQuery(wxString& query)const
{
	auto data = GetData();
	query = wxString::Format(
		"SELECT id, title, note, color, script "
		"FROM act WHERE id=%s",
		data.mID);
	return true;
}
//-------------------------------------------------------------------------
bool MAct::GetInsertQuery(wxString& query)const
{
	auto act = GetData();
	query = wxString::Format("INSERT INTO act (title,note,color,script) "
		"VALUES ('%s', %s, %s, %s) RETURNING id,title,note,color,script",
		act.mLabel,
		act.mComment.IsEmpty() ? L"NULL" : wxString::Format(L"'%s'", act.mComment),
		act.mColor.IsEmpty() ? L"NULL" : wxString::Format(L"'%s'", act.mColor),
		act.mScript.IsEmpty() ? L"NULL" : wxString::Format(L"'%s'", act.mScript));
	return true;
}
//-------------------------------------------------------------------------
bool MAct::GetUpdateQuery(wxString& query)const
{
	auto act = GetData();
	query = wxString::Format("UPDATE t_act SET "
		"title='%s', note=%s, color=%s, script=%s "
		"WHERE id = %s ",
		act.mLabel,
		act.mComment.IsEmpty() ? L"NULL" : wxString::Format(L"'%s'", act.mComment),
		act.mColor.IsEmpty() ? L"NULL" : wxString::Format(L"'%s'", act.mColor),
		act.mScript.IsEmpty() ? L"NULL" : wxString::Format(L"'%s'", act.mScript),
		act.mID);
	return true;
}
//-------------------------------------------------------------------------
bool MAct::GetDeleteQuery(wxString& query)const
{
	auto act = GetData();
	query = wxString::Format("DELETE FROM act WHERE id = %s ",
		act.mID);
	return true;
}
//-------------------------------------------------------------------------
bool MAct::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	table->GetAsString(0, row, data.mID);
	table->GetAsString(1, row, data.mLabel);
	table->GetAsString(2, row, data.mComment);
	table->GetAsString(3, row, data.mColor);
	table->GetAsString(4, row, data.mScript);
	SetData(data);
	return true;
};
//-------------------------------------------------------------------------
void MAct::LoadChilds()
{
	if (!mPropArray)
	{
		mPropArray.reset(new MActPropArray);
		std::shared_ptr<IModel> item = std::dynamic_pointer_cast<IModel>(mPropArray);
		this->Insert(item);
	}
	mPropArray->Load();
}
//-------------------------------------------------------------------------
bool MAct::GetFieldValue(unsigned int col, wxVariant &variant)
{
	const auto& data = this->GetData();
	auto mgr = ResMgr::GetInstance();
	switch (col)
	{
	default:	break;
	case 1:		variant = variant << wxDataViewIconText(data.mLabel, mgr->m_ico_act24);	break;
	case 2:		variant = data.mComment; break;
	case 3: 	variant = data.mColor; 	break;
	case 4:		variant = data.mID;		break;
	case 5:		variant = data.mVID; 	break;
	}//switch(col) 
	return true;
}
//-------------------------------------------------------------------------
const std::vector<Field>& MAct::GetFieldVector()const
{
	return gActFieldVec;
	//return gEmptyFieldVec;
}







//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MActArray::MActArray(const char option)
	:TModelArray<T_Item>(option)
{
}
//-------------------------------------------------------------------------
bool MActArray::GetSelectChildsQuery(wxString& query)const
{
	query = wxString::Format(
		"SELECT id, title, note, color, script  FROM act");
	return true;
}
//-------------------------------------------------------------------------
std::shared_ptr<IModel> MActArray::CreateChild()
{
	auto child = new T_Item;
	child->SetData(T_Item::T_Data());
	return std::shared_ptr<IModel>(child);
}

//-------------------------------------------------------------------------
bool MActArray::LoadChildDataFromDb(std::shared_ptr<IModel>& model,
	std::shared_ptr<whTable>& table, const size_t pos)
{
	auto child = std::dynamic_pointer_cast<MAct>(model);
	if (child)
		return child->LoadThisDataFromDb(table, pos);
	return false;
};
//-------------------------------------------------------------------------
const std::vector<Field>& MActArray::GetFieldVector()const
{
	return gActFieldVec;
	//return gEmptyFieldVec;
}







