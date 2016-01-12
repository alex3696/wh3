#include "_pch.h"
#include "MUser2.h"

using namespace wh;

const std::vector<Field> gUserFieldVec = {
	{ "Имя", FieldType::ftName, true },
	{ "Описание", FieldType::ftText, true },
	{ "ID", FieldType::ftLong, true },
	{ "Лимит подключений", FieldType::ftLong, true },
	{ "Дата окончания", FieldType::ftDate, true }
};

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MUser2::MUser2(const char option)
:TModelData<rec::User>(option)
{
}
//-------------------------------------------------------------------------
std::shared_ptr<MUserGroupArray> MUser2::GetGroupArray()
{
	if (!mGroupArray)
	{
		mGroupArray.reset(new MUserGroupArray);
		std::shared_ptr<IModel> item = std::dynamic_pointer_cast<IModel>(mGroupArray);
		this->Insert(item);
	}
	return mGroupArray;
}
//-------------------------------------------------------------------------
bool MUser2::GetSelectQuery(wxString& query)const
{
	const auto& user = GetData();
	query = wxString::Format(
		"SELECT userid, username, rolcomment, rolconnlimit, rolvaliduntil, rolpassword "
		" FROM wh_user"
		" WHERE userid = %s "
		" ORDER BY username ASC"
		, user.mID);
	return true;
}
//-------------------------------------------------------------------------
bool MUser2::GetInsertQuery(wxString& query)const
{
	const auto& user = GetData();
	query = wxString::Format(
		"INSERT INTO wh_role "
		"(rolname, rolcomment, rolconnlimit, rolvaliduntil, rolpassword, "
			"rolcanlogin, rolcreaterole) VALUES "
		"('%s', %s, %s, %s, %s, true, false) "
		" RETURNING id, rolname, rolcomment, rolconnlimit, rolvaliduntil, rolpassword",
		user.mLabel,
		user.mComment.IsEmpty() ? L"NULL" : wxString::Format(L"'%s'", user.mComment),
		user.mConnLimit.IsEmpty() ? L"-1" : user.mConnLimit,
		user.mValidUntil.IsEmpty() ? L"NULL" : wxString::Format(L"'%s'", user.mValidUntil),
		user.mPassword.IsEmpty() ? L"NULL" : wxString::Format(L"'%s'", user.mPassword)
		);
	return true;
}
//-------------------------------------------------------------------------
bool MUser2::GetUpdateQuery(wxString& query)const
{
	const auto& user = GetData();
	query = wxString::Format(
		"UPDATE wh_role SET"
		" rolname='%s', rolcomment=%s, rolconnlimit=%s, rolvaliduntil=%s, rolpassword=%s "
		" WHERE id = %s ",
		user.mLabel,
		user.mComment.IsEmpty() ? L"NULL" : wxString::Format(L"'%s'", user.mComment),
		user.mConnLimit.IsEmpty() ? L"NULL" : user.mConnLimit,
		user.mValidUntil.IsEmpty() ? L"NULL" : wxString::Format(L"'%s'", user.mValidUntil),
		user.mPassword.IsEmpty() ? L"NULL" : wxString::Format(L"'%s'", user.mPassword),
		user.mID);
	return true;
}
//-------------------------------------------------------------------------
bool MUser2::GetDeleteQuery(wxString& query)const
{
	const auto& user = GetData();
	query = wxString::Format("DELETE FROM wh_role WHERE id = %s ",
		user.mID);
	return true;
}
//-------------------------------------------------------------------------
bool MUser2::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	table->GetAsString(0, row, data.mID);
	table->GetAsString(1, row, data.mLabel);
	table->GetAsString(2, row, data.mComment);
	table->GetAsString(3, row, data.mConnLimit);
	table->GetAsString(4, row, data.mValidUntil);
	table->GetAsString(5, row, data.mPassword);	
	SetData(data);
	return true;
};
//-------------------------------------------------------------------------
void MUser2::LoadChilds()
{
	if (!mGroupArray)
	{
		mGroupArray.reset(new MUserGroupArray);
		std::shared_ptr<IModel> item = std::dynamic_pointer_cast<IModel>(mGroupArray);
		this->Insert(item);
	}
	mGroupArray->Load();
}
//-------------------------------------------------------------------------
bool MUser2::GetFieldValue(unsigned int col, wxVariant &variant)
{
	const auto& data = this->GetData();
	auto mgr = ResMgr::GetInstance();
	switch (col)
	{
	default: break;
	case 1: variant = variant << wxDataViewIconText(data.mLabel, mgr->m_ico_user24); break;
	case 2:	variant = data.mComment;		break;
	case 3:	variant = data.mID;				break;
	case 4:	variant = data.mConnLimit;		break;
	case 5:	variant = data.mValidUntil;		break;
	}//switch(col) 
	return true;
}
//-------------------------------------------------------------------------
const std::vector<Field>& MUser2::GetFieldVector()const
{
	return gUserFieldVec;
	//return gEmptyFieldVec;
}




//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MUserArray::MUserArray(const char option)
	:TModelArray<T_Item>(option)
{
}
//-------------------------------------------------------------------------
bool MUserArray::GetSelectChildsQuery(wxString& query)const
{
	query = 
		"SELECT userid, username, rolcomment, rolconnlimit, rolvaliduntil, rolpassword "
		" FROM wh_user";
	return true;
}
//-------------------------------------------------------------------------
std::shared_ptr<IModel> MUserArray::CreateChild()
{
	auto child = new T_Item;
	child->SetData(T_Item::T_Data());
	return std::shared_ptr<IModel>(child);
}

//-------------------------------------------------------------------------
bool MUserArray::LoadChildDataFromDb(std::shared_ptr<IModel>& model,
	std::shared_ptr<whTable>& table, const size_t pos)
{
	auto child = std::dynamic_pointer_cast<MUser2>(model);
	if (child)
		return child->LoadThisDataFromDb(table, pos);
	return false;
};
//-------------------------------------------------------------------------
const std::vector<Field>& MUserArray::GetFieldVector()const
{
	return gUserFieldVec;
	//return gEmptyFieldVec;
}







