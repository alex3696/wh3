#include "_pch.h"
#include "MGroup.h"

using namespace wh;

const std::vector<Field> gGroupFieldVec = {
	{ "Имя", FieldType::ftName, true },
	{ "Описание", FieldType::ftText, true },
	{ "ID", FieldType::ftInt, true }
};



//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MGroup::MGroup(const char option)
	:TModelData<rec::Role>(option)
{
	//mOption = option;
}
//-------------------------------------------------------------------------
bool MGroup::GetSelectQuery(wxString& query)const
{
	auto data = GetData();
	query = wxString::Format("groupid,groupname,rolcomment "
							" FROM wh_group WHERE id=%s", data.mID);
	return true;
}
//-------------------------------------------------------------------------
bool MGroup::GetInsertQuery(wxString& query)const
{
	auto data = GetData();
	query = wxString::Format( "INSERT INTO wh_role "
		"(rolname, rolcomment, rolcanlogin, rolcreaterole) "
		" VALUES ('%s', '%s', false, false) " 
		" RETURNING id, rolname, rolcomment "
		, data.mLabel, data.mComment);
	return true;
}
//-------------------------------------------------------------------------
bool MGroup::GetUpdateQuery(wxString& query)const
{
	auto data = GetData();
	query = wxString::Format("UPDATE wh_role SET "
							" rolname='%s', rolcomment='%s' "
							" WHERE id = %s "
			, data.mLabel, data.mComment, data.mID);
	return true;
}
//-------------------------------------------------------------------------
bool MGroup::GetDeleteQuery(wxString& query)const
{
	auto data = GetData();
	query = wxString::Format("DELETE FROM wh_role WHERE id = %s", data.mID);
	return true;
}
//-------------------------------------------------------------------------
bool MGroup::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	table->GetAsString(0, row, data.mID);
	table->GetAsString(1, row, data.mLabel);
	table->GetAsString(2, row, data.mComment);
	SetData(data);
	return true;
};

//-------------------------------------------------------------------------
bool MGroup::GetFieldValue(unsigned int col, wxVariant &variant)
{
	const auto& data = this->GetData();
	auto mgr = ResMgr::GetInstance();
	switch (col)
	{
	default: break;
	case 1: variant = variant << wxDataViewIconText(data.mLabel, mgr->m_ico_usergroup24); break;
	case 2: variant = data.mComment; break;
	case 3: variant = data.mID; break;
	}//switch(col) 
	return true;
}
//-------------------------------------------------------------------------
const std::vector<Field>& MGroup::GetFieldVector()const
{
	return gGroupFieldVec;
	//return gEmptyFieldVec;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MGroupArray::MGroupArray(const char option)
	:IModel(option)
{
}
//-------------------------------------------------------------------------
bool MGroupArray::GetSelectChildsQuery(wxString& query)const
{
	query = L"SELECT groupid, groupname, rolcomment "
			L" FROM wh_group ORDER BY groupname ASC";
	return true;
}
//-------------------------------------------------------------------------
std::shared_ptr<IModel> MGroupArray::CreateChild()
{
	auto child = new MGroup;
	child->SetData(rec::Role());
	return std::shared_ptr<IModel>(child);
}

//-------------------------------------------------------------------------
bool MGroupArray::LoadChildDataFromDb(std::shared_ptr<IModel>& child,
	std::shared_ptr<whTable>& table, const size_t pos)
{
	//auto propChild = std::dynamic_pointer_cast<MPropChild>(child);

	auto propChild = std::dynamic_pointer_cast<TModelData<rec::Role>>(child);
	if (propChild)
		return propChild->LoadThisDataFromDb(table, pos);
	return false;
};
//-------------------------------------------------------------------------
const std::vector<Field>& MGroupArray::GetFieldVector()const
{
	return gGroupFieldVec;
	//return gEmptyFieldVec;
}