#include "_pch.h"
#include "detail_model.h"

using namespace wh;
using namespace wh::detail::model;

//-----------------------------------------------------------------------------
ObjProp::ObjProp(const char option)
	:TModelData<rec::PropVal>(option)
{
}
//-----------------------------------------------------------------------------
bool ObjProp::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	data.mProp.mId = table->GetAsLong(0, row);
	data.mProp.mLabel = table->GetAsString(1, row);
	data.mProp.mType = ToFieldType(table->GetAsString(2, row));
	data.mVal = table->GetAsString(3, row);
	SetData(data);
	return true;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ObjPropArray::ObjPropArray(const char option)
	:TModelArray<ObjProp>()
{
}
//-----------------------------------------------------------------------------
bool ObjPropArray::GetSelectChildsQuery(wxString& query)const
{
	auto parentCls = dynamic_cast<Obj*>(GetParent());
	if (!parentCls)
		return false;
	const auto& data = parentCls->GetData();
	if (data.mCls.mType.IsNull() || data.mCls.IsAbstract())
		return false;
	
	query = wxString::Format(
		"SELECT distinct(ref_act_prop.prop_id) , prop.title , prop.kind "
		"  FROM ref_cls_act "
		"  INNER JOIN ref_act_prop ON ref_act_prop.act_id = ref_cls_act.act_id "
		"  LEFT JOIN prop ON prop.id = ref_act_prop.prop_id "
		"  WHERE ref_cls_act.cls_id IN (SELECT id FROM get_path_cls_info(%s, 0))" 
		, data.mCls.mId.SqlVal() );
	return true;
}
