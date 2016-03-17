#include "_pch.h"
#include "MObjCatalog.h"

using namespace wh;
using namespace wh::object_catalog;



//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MFavProp::MFavProp(const char option)
	: TModelData<rec::FavProps>(option)
{
}

//-------------------------------------------------------------------------
void MFavProp::LoadData()
{
	wxString query;
	if (!GetSelectQuery(query))
		return;

	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (!table)
		return;

	rec::FavProps favProps;

	unsigned int rowQty = table->GetRowCount();
	for (unsigned int row = 0; row < rowQty; ++row)
	{
		wxString label = table->GetAsString(1, row);
		wxString cls = table->GetAsString(3, row);

		auto it = favProps.get<1>().find(label);
		if (favProps.get<1>().end() == it)
		{
			rec::FavProp newFav;
			table->GetAsString(0, row, newFav.mID);
			newFav.mLabel = label;
			table->GetAsString(2, row, newFav.mType);
			newFav.mCls->insert(cls);
			favProps.push_back(newFav);
		}
		else
			it->mCls->insert(cls);
	}
		
	this->SetData(favProps, true);
	
};
//-------------------------------------------------------------------------
bool MFavProp::GetSelectQuery(wxString& query)const
{
	auto catalog = dynamic_cast<MObjCatalog*>(this->GetParent());
	if (!catalog)
		return false;

	const auto& root = catalog->GetRoot();

	if (catalog->IsObjTree())
	{
		query = wxString::Format(
			"SELECT distinct prop.id, prop.title, prop.kind, cls_id "
			" FROM favorite_prop "
			" LEFT JOIN prop  ON prop.id = favorite_prop.prop_id "
			" WHERE favorite_prop.user_label = CURRENT_USER "
			" AND (favorite_prop.cls_id IN (SELECT distinct(upcls.id) FROM "
			"                                 (SELECT distinct(obj.cls_id) FROM obj WHERE obj.pid = %s) downcls, "
			"                                  LATERAL(SELECT * FROM get_path_cls_info(downcls.cls_id)) upcls)) "
			" ORDER BY prop.title"
		    , root.mObj.mId.SqlVal() );
		return true;
	}
	else
	{
		query = wxString::Format(
			"SELECT distinct prop.id, prop.title, prop.kind, cls_id "
			" FROM favorite_prop "
			" LEFT JOIN prop  ON prop.id = favorite_prop.prop_id "
			" WHERE favorite_prop.user_label = CURRENT_USER "
			" AND (favorite_prop.cls_id IN(SELECT id FROM cls WHERE pid = %s)"
			"     OR favorite_prop.cls_id IN(SELECT id FROM get_path_cls_info(%s, 0))) "
			" ORDER BY prop.title"
			, root.mCls.mId.SqlVal()
		    , root.mCls.mId.SqlVal() );
		return true;
	}
	
	return false;
}


