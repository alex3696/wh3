#include "_pch.h"
#include "MProp.h"

using namespace wh;



const std::vector<Field> gPropFieldVec = {
	{ "Èìÿ", FieldType::ftName, true },
	{ "Òèï", FieldType::ftText, true },
	{ "ID", FieldType::ftInt, true }
};



//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MPropChild::MPropChild(const char option)
:TModelData<rec::Prop>(option)
{

}
//-------------------------------------------------------------------------
bool MPropChild::GetSelectQuery(wxString& query)const
{
	auto data = GetData();
	query = wxString::Format(
		"SELECT id, title, kind FROM prop WHERE id=%s"
		, data.mId.SqlVal());
	return true;
}
//-------------------------------------------------------------------------
bool MPropChild::GetInsertQuery(wxString& query)const
{
	auto data = GetData();
	query = wxString::Format
		("INSERT INTO prop(title, kind)VALUES('%s', %s)RETURNING id, title, kind",
		data.mLabel.SqlVal(), data.mType);
	return true;
}
//-------------------------------------------------------------------------
bool MPropChild::GetUpdateQuery(wxString& query)const
{
	const auto& data = GetData();
	query = wxString::Format(
		"UPDATE prop SET title='%s' , kind=%s WHERE id=%s"
		, data.mLabel.SqlVal(), data.mType, data.mId.SqlVal());
	return true;
}
//-------------------------------------------------------------------------
bool MPropChild::GetDeleteQuery(wxString& query)const
{
	const auto& data = GetData();
	query = wxString::Format(
		"DELETE FROM prop WHERE id = %s "
		, data.mId.SqlVal());
	return true;
}
//-------------------------------------------------------------------------
bool MPropChild::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	data.mId = table->GetAsLong(0, row);
	data.mLabel =table->GetAsString(1, row);
	table->GetAsString(2, row, data.mType);
	SetData(data);
	return true;
};
//-------------------------------------------------------------------------
bool MPropChild::GetFieldValue(unsigned int col, wxVariant &variant)
{
	auto mgr = ResMgr::GetInstance();

	const auto& data = this->GetData();

	switch (col)
	{
	default: break;
	case 1: variant = variant << wxDataViewIconText(data.mLabel, mgr->m_ico_classprop24);
		break;
	case 2: variant = data.GetTypeString(); break;
	case 3: variant = data.mId.toStr(); break;
	}//switch(col) 
	return true;
}
//-------------------------------------------------------------------------
const std::vector<Field>& MPropChild::GetFieldVector()const
{
	return gPropFieldVec;
	//return gEmptyFieldVec;
}




//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
bool MPropArray::GetSelectChildsQuery(wxString& query)const
{
	query = L"SELECT id, title, kind FROM prop";
	return true;
}
