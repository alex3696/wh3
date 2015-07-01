#include "_pch.h"
#include "MUser2.h"

using namespace wh;

const std::vector<Field> gUserGroupFieldVec = {
	{ "Èìÿ", FieldType::ftName, true },
	{ "ID", FieldType::ftInt, true }
};


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MUserGroup::MUserGroup(const char option)
:TModelData<rec::UserRole>(option)
{
}

//-------------------------------------------------------------------------
bool MUserGroup::GetSelectQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MUserGroupArray*>(this->mParent);
	auto parentUser = dynamic_cast<MUser2*>(parentArray->GetParent());

	if (parentUser)
	{
		const auto& group = GetData();
		const auto& user = parentUser->GetData();
		
		query = wxString::Format(
			"SELECT groupid,groupname FROM wh_membership "
			" WHERE username='%s' AND groupname='%s' "
			, user.mLabel, group.mLabel);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MUserGroup::GetInsertQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MUserGroupArray*>(this->mParent);
	auto parentUser = dynamic_cast<MUser2*>(parentArray->GetParent());

	if (parentUser)
	{
		const auto& group = GetData();
		const auto& user = parentUser->GetData();

		query = wxString::Format(
			"INSERT INTO wh_auth_members(roleid,member) "
			" VALUES('%s', '%s') RETURNING NULL "
			, group.mID, user.mID);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MUserGroup::GetUpdateQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MUserGroupArray*>(this->mParent);
	auto parentUser = dynamic_cast<MUser2*>(parentArray->GetParent());

	if (parentUser)
	{
		const auto& group = GetData();
		const auto& user = parentUser->GetData();

		query = wxString::Format(
			"UPDATE wh_auth_members SET roleid=%s "
			" WHERE member=%s AND roleid=% ERROR previos id "
			, group.mID, user.mID);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MUserGroup::GetDeleteQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MUserGroupArray*>(this->mParent);
	auto parentUser = dynamic_cast<MUser2*>(parentArray->GetParent());

	if (parentUser)
	{
		const auto& group = GetData();
		const auto& user = parentUser->GetData();

		query = wxString::Format(
		" DELETE FROM wh_auth_members "
			" WHERE roleid = %s  AND member=%s "
			, group.mID, user.mID);

		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MUserGroup::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	table->GetAsString(0, row, data.mID);
	table->GetAsString(1, row, data.mLabel);
	SetData(data);
	return true;
};
//-------------------------------------------------------------------------
bool MUserGroup::GetFieldValue(unsigned int col, wxVariant &variant)
{
	const auto& data = this->GetData();
	auto mgr = ResMgr::GetInstance();
	switch (col)
	{
	default: break;
	case 1: variant = variant << wxDataViewIconText(data.mLabel, mgr->m_ico_usergroup24); break;
	case 2: variant = data.mID; break;
	}//switch(col) 
	mgr->FreeInst();
	return true;
}

//-------------------------------------------------------------------------
const std::vector<Field>& MUserGroup::GetFieldVector()const
{
	return gUserGroupFieldVec;
	//return gEmptyFieldVec;
}




//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MUserGroupArray::MUserGroupArray(const char option)
:IModel(option)
{
}
//-------------------------------------------------------------------------
bool MUserGroupArray::GetSelectChildsQuery(wxString& query)const
{
	auto parentUser = dynamic_cast<MUser2*>(this->mParent);
	if (parentUser)
	{
		const auto& user = parentUser->GetData();
		query = wxString::Format(
			"SELECT groupid,groupname FROM wh_membership "
			" WHERE username='%s' "
			, user.mLabel);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
std::shared_ptr<IModel> MUserGroupArray::CreateChild()
{
	auto child = new T_Item;
	child->SetData(T_Item::T_Data());
	return std::shared_ptr<IModel>(child);
}

//-------------------------------------------------------------------------
bool MUserGroupArray::LoadChildDataFromDb(std::shared_ptr<IModel>& model,
	std::shared_ptr<whTable>& table, const size_t pos)
{
	auto child = std::dynamic_pointer_cast<MUserGroup>(model);
	if (child)
		return child->LoadThisDataFromDb(table, pos);
	return false;
};

//-------------------------------------------------------------------------
const std::vector<Field>& MUserGroupArray::GetFieldVector()const
{
	return gUserGroupFieldVec;
	//return gEmptyFieldVec;
}
