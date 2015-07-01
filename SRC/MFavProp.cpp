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

	const auto& root = catalog->GetData();

	if (catalog->mCfg->GetData().mObjCatalog)
	{
		query = wxString::Format(
			" SELECT distinct "
			"t_prop.id, t_prop.label, t_prop.type, cls_id "
			"FROM t_favorite_prop "
			"LEFT JOIN t_prop  ON t_prop.id = t_favorite_prop.prop_id "
			"WHERE t_favorite_prop.user_label = CURRENT_USER "
			"AND   t_favorite_prop.cls_id IN(SELECT distinct(cls_id)FROM t_objnum WHERE pid = %s)"
			, root.mObj.mID);
		return true;
	}
	else
	{
		query = wxString::Format(
			" SELECT distinct "
			"t_prop.id, t_prop.label, t_prop.type, cls_id "
			"FROM t_favorite_prop "
			"LEFT JOIN t_prop  ON t_prop.id = t_favorite_prop.prop_id "
			"WHERE t_favorite_prop.user_label = CURRENT_USER "
			"AND t_favorite_prop.cls_id IN(SELECT id FROM t_cls WHERE pid = %s) "
			, root.mCls.mID );
		return true;
	}
	
	return false;
}


