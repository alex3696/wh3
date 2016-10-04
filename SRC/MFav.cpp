#include "_pch.h"
#include "MFav.h"

using namespace wh;



const std::vector<Field> gPropFieldVec = {
	{ "Имя", FieldType::ftName, true },
	{ "Примечание", FieldType::ftText, true },
	{ "ID", FieldType::ftName, true }
};



//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MFav::MFav(const char option)
	:TModelData<rec::Fav>(option)
	, mFilterClsArray(new MFavFilterClsArray)
	, mFilterPathArray(new MFavFilterPathArray)
	, mViewPropClsArray(new MFavViewPropClsArray)
	, mViewPropActCommonArray(new MFavViewPropActCommonArray)
	, mViewPropActUsrArray(new MFavViewPropActUsrArray)
{
	this->Insert(mFilterClsArray);
	this->Insert(mFilterPathArray);
	this->Insert(mViewPropClsArray);
	this->Insert(mViewPropActCommonArray);
	this->Insert(mViewPropActUsrArray);
}
//-------------------------------------------------------------------------
bool MFav::GetSelectQuery(wxString& query)const
{
	const auto& stored = GetStored();
	query = wxString::Format(
		"SELECT id, title, note, usr, view_group FROM fav WHERE id=%d "
		, stored.mId);
	return true;
}
//-------------------------------------------------------------------------
bool MFav::GetInsertQuery(wxString& query)const
{
	auto data = GetData();
	query = wxString::Format(
		"INSERT INTO prop(title, note, view_group)"
		" VALUES(%s, %s, %d)"
		" RETURNING id, title, note, usr, view_group"
		, data.mTitle, data.mNote, data.mViewGroup);
	return true;
}
//-------------------------------------------------------------------------
bool MFav::GetUpdateQuery(wxString& query)const
{
	const auto& data = GetData();
	const auto& stored = GetStored();
	query = wxString::Format(
		"UPDATE prop SET title=%s , note=%s, view_group=%d "
		" WHERE id=%s"
		, data.mTitle, data.mNote, data.mViewGroup
		, stored.mId);
	return true;
}
//-------------------------------------------------------------------------
bool MFav::GetDeleteQuery(wxString& query)const
{
	const auto& stored = GetStored();
	query = wxString::Format(
		"DELETE FROM prop WHERE id = %s "
		, stored.mId);
	return true;
}
//-------------------------------------------------------------------------
bool MFav::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	data.mId	= table->GetAsString(0, row);
	data.mTitle = table->GetAsString(1, row);
	data.mNote	= table->GetAsString(2, row);
	data.mUsr	= table->GetAsString(3, row);
	data.mViewGroup = table->GetAsLong(4, row);
	SetData(data);
	return true;
};
//-------------------------------------------------------------------------
bool MFav::GetFieldValue(unsigned int col, wxVariant &variant)
{
	auto mgr = ResMgr::GetInstance();

	const auto& data = this->GetData();

	switch (col)
	{
	default: break;
	case 1: variant = variant << wxDataViewIconText(data.mTitle, mgr->m_ico_classprop24);
		break;
	case 2: variant = data.mNote; break;
	case 3: variant = data.mId; break;
	}//switch(col) 
	return true;
}
//-------------------------------------------------------------------------
const std::vector<Field>& MFav::GetFieldVector()const
{
	return gPropFieldVec;//return gEmptyFieldVec;
}




//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
bool MFavArray::GetSelectChildsQuery(wxString& query)const
{
	query = "SELECT id, title, note, usr, view_group "
			" FROM fav "
			" WHERE usr=CURRENT_USER ";
	return true;
}


