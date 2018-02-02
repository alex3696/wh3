#include "_pch.h"
#include "dlg_favprop_model.h"
#include "MObjCatalog.h"

using namespace wh;
using namespace wh::dlg::favprop::model;


//-----------------------------------------------------------------------------
// FavPropItem
//-----------------------------------------------------------------------------
FavPropItem::FavPropItem(const char option)
	: TModelData<DataType>(option)
{
}
//-----------------------------------------------------------------------------

void FavPropItem::LoadChilds()
{
}

//-------------------------------------------------------------------------
bool FavPropItem::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	data.mId = table->GetAsLong(0, row);
	data.mLabel = table->GetAsString(1, row);
	data.mType = ToFieldType(table->GetAsString(2, row));
	data.mSelected = !(table->GetAsString(3, row).IsEmpty());
	SetData(data);
	return true;
};

//-------------------------------------------------------------------------
bool FavPropItem::GetInsertQuery(wxString& query)const
{
	return false;
	/*
	auto parentArray = dynamic_cast<FavPropArray*>(this->mParent);
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(parentArray->GetParent());
	if (!parentCls)
		return false;

	const rec::Cls& cls = parentCls->GetStored();
	const auto& newObj = this->GetData();
	ClsType ct;
	if (!cls.GetClsType(ct) && ct != ClsKind::Single)
		return false;

	query = wxString::Format(
		"WITH upd AS( "
		" SELECT distinct(t_ref_act_prop.prop_id), t_ref_class_act.cls_id, t_ref_class_act.act_id "
		"  FROM t_ref_class_act "
		"  INNER JOIN t_ref_act_prop ON t_ref_act_prop.act_id = t_ref_class_act.act_id "
		"  WHERE t_ref_class_act.cls_id = %s "
		"  AND t_ref_act_prop.prop_id = %s "
		"  ) "
		" INSERT INTO t_favorite_prop(prop_id, cls_id, act_id) "
		"    SELECT upd.prop_id, upd.cls_id, upd.act_id  FROM upd "
		, cls.mID
		, this->GetData().mID);
	return true;
	*/
}
//-------------------------------------------------------------------------
bool FavPropItem::GetUpdateQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<FavPropArray*>(this->mParent);
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(parentArray->GetParent());
	if (!parentCls)
		return false;

	const rec::Cls& cls = parentCls->GetStored();
	//const auto& newObj = this->GetData();

	//if (cls.IsAbstract())
	//	return false;
	
	const auto& prop_data = this->GetData();
	const auto& prop_data_stored = this->GetStored();

	if (prop_data_stored.mSelected == prop_data.mSelected)
		return false;

	if (prop_data.mSelected)
		query = wxString::Format(
		"WITH upd AS( "
		" SELECT distinct(ref_act_prop.prop_id), ref_cls_act.cls_id, ref_cls_act.act_id "
		"  FROM ref_cls_act "
		"  INNER JOIN ref_act_prop ON ref_act_prop.act_id = ref_cls_act.act_id "
		"  WHERE ref_cls_act.cls_id IN(SELECT id FROM get_path_cls_info(%s, 0)) "
		"  AND ref_act_prop.prop_id = %s "
		"  ) "
		" INSERT INTO favorite_prop(prop_id, cls_id, act_id) "
		"    SELECT upd.prop_id, upd.cls_id, upd.act_id  FROM upd "
		, cls.mId.SqlVal()
		, prop_data.mId.SqlVal());
	else
		query = wxString::Format(
		"DELETE FROM favorite_prop "
		" WHERE cls_id IN (SELECT id FROM get_path_cls_info(%s, 0)) "
		" AND prop_id = %s AND user_label = CURRENT_USER "
		, cls.mId.SqlVal()
		, prop_data.mId.SqlVal());
	

	return true;
}


//-------------------------------------------------------------------------
bool FavPropItem::GetDeleteQuery(wxString& query)const
{
	return false;
	/*
	auto parentArray = dynamic_cast<FavPropArray*>(this->mParent);
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(parentArray->GetParent());
	if (!parentCls)
		return false;

	const rec::Cls& cls = parentCls->GetStored();
	const auto& newObj = this->GetData();
	ClsType ct;
	if (!cls.GetClsType(ct) && ct != ClsKind::Single)
		return false;

	const auto& prop_data = this->GetData();

	query = wxString::Format(
		"DELETE FROM t_favorite_prop "
		" WHERE cls_id = %s AND prop_id = %s AND user_label = CURRENT_USER "
		, cls.mID
		, prop_data.mID);
	return true;
	*/
}



//-----------------------------------------------------------------------------
// FavPropArray
//-----------------------------------------------------------------------------
bool FavPropArray::GetSelectChildsQuery(wxString& query)const
{
	auto typeItemModel = dynamic_cast<object_catalog::MTypeItem*>(this->GetParent());
	if (!typeItemModel)
		return false;

	const auto& typeData = typeItemModel->GetData();

	query = wxString::Format(
		"SELECT prop.id, prop.title, prop.kind, favprop.prop_id "
		"  FROM( "
		"    SELECT distinct(ref_act_prop.prop_id) "
		"      FROM ref_cls_act "
		"      INNER JOIN ref_act_prop ON ref_act_prop.act_id = ref_cls_act.act_id "
		"      WHERE ref_cls_act.cls_id IN(SELECT id FROM get_path_cls_info(%s,0)) "
		"  ) all_prop "
		"  LEFT JOIN( "
		"    SELECT distinct(favorite_prop.prop_id) FROM favorite_prop "
		"      WHERE favorite_prop.cls_id IN(SELECT id FROM get_path_cls_info(%s,0)) "
		"      AND favorite_prop.user_label = CURRENT_USER "
		"  ) favprop "
		"  ON favprop.prop_id = all_prop.prop_id "
		"  LEFT JOIN prop ON prop.id = all_prop.prop_id "
		, typeData.mId.SqlVal()
		, typeData.mId.SqlVal() );
	return true;
}
//-----------------------------------------------------------------------------
