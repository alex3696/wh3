#include "_pch.h"
#include "MFavFilterCls.h"
#include "MFav.h"

using namespace wh;




//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MFavFilterCls::MFavFilterCls(const char option)
	:TModelData<rec::FavFilterCls>(option)
{
}
//-------------------------------------------------------------------------
bool MFavFilterCls::GetSelectQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MFavArray*>(this->mParent);
	auto parentItem = dynamic_cast<MFav*>(parentArray->GetParent());
	if (!parentItem)
		return false;
	const rec::Fav& stored_fav = parentItem->GetStored();

	const rec::FavFilterCls& data = HaveStored() ? GetStored() : GetData();
	
	query = wxString::Format(
	"SELECT fid, id,title,kind"
	" FROM fav_filter_cls"
	" LEFT JOIN acls ON acls.id = cid"
	" WHERE fid=%s AND cid=%s"
	, stored_fav.mId, data.mCId);
	return true;
}
//-------------------------------------------------------------------------
bool MFavFilterCls::GetInsertQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MFavArray*>(this->mParent);
	auto parentItem = dynamic_cast<MFav*>(parentArray->GetParent());
	if (!parentItem)
		return false;
	const rec::Fav& stored_fav = parentItem->GetStored();

	const rec::FavFilterCls& data = GetData();

	query = wxString::Format(
		"WITH ins AS("
		" INSERT INTO fav_filter_cls(fid, cid)"
		" VALUES(%s, %s) "
		" RETURNING fid, cid) "
		" SELECT fid, id, title, kind "
		"  FROM ins"
		"  LEFT JOIN acls ON acls.id = ins.cid "
		, stored_fav.mId, data.mCId);
	return true;
}
//-------------------------------------------------------------------------
bool MFavFilterCls::GetUpdateQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MFavArray*>(this->mParent);
	auto parentItem = dynamic_cast<MFav*>(parentArray->GetParent());
	if (!parentItem)
		return false;
	const auto& stored_fav = parentItem->GetStored();

	const auto& data = GetData();
	const auto& stored = GetStored();
	query = wxString::Format(
		"UPDATE prop fav_filter_cls cid=%s "
		" WHERE fid=%s AND cid=%s"
		, data.mCId
		, stored_fav.mId, stored.mCId);

	return true;
}
//-------------------------------------------------------------------------
bool MFavFilterCls::GetDeleteQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MFavArray*>(this->mParent);
	auto parentItem = dynamic_cast<MFav*>(parentArray->GetParent());
	if (!parentItem)
		return false;
	const rec::Fav& stored_fav = parentItem->GetStored();

	const auto& data = GetData();
	
	query = wxString::Format(
		"DELETE FROM fav_filter_cls "
		" WHERE fid=%s AND cid=%s"
		, stored_fav.mId, data.mCId);
	return true;
}
//-------------------------------------------------------------------------
bool MFavFilterCls::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	data.mCId = table->GetAsString(0, row);
	data.mCTitle = table->GetAsString(1, row);
	data.mCKind = table->GetAsString(2, row);
	SetData(data);
	return true;
};


//-------------------------------------------------------------------------
//MFavFilterClsArray
//-------------------------------------------------------------------------
MFavFilterClsArray::MFavFilterClsArray(const char option)
	: TModelArray<MFavFilterCls>(option)
{
}

//-------------------------------------------------------------------------
std::shared_ptr<IModel> MFavFilterClsArray::CreateChild()
{
	auto child = new T_Item;
	child->SetData(T_Item::T_Data());
	return std::shared_ptr<IModel>(child);
}
//-------------------------------------------------------------------------
bool MFavFilterClsArray::GetSelectChildsQuery(wxString& query)const
{
	auto parentItem = dynamic_cast<MFav*>(GetParent());
	if (!parentItem)
		return false;
	const rec::Fav& stored_fav = parentItem->GetStored();

	query = wxString::Format(
		"SELECT fid, id,title,kind"
		" FROM fav_filter_cls"
		" LEFT JOIN acls ON acls.id = cid"
		" WHERE fid=%s"
		, stored_fav.mId);
	return true;
}


