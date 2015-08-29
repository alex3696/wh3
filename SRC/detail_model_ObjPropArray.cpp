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
	table->GetAsString(0, row, data.mProp.mID);
	table->GetAsString(1, row, data.mProp.mLabel);
	table->GetAsString(2, row, data.mProp.mType);
	table->GetAsString(3, row, data.mVal);
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
	if (data.mCls.IsAbstract())
		return false;
	
	query = wxString::Format(
		" SELECT distinct(t_ref_act_prop.prop_id), t_prop.label, t_prop.type "
		" FROM t_ref_class_act "
		" INNER JOIN t_ref_act_prop ON t_ref_act_prop.act_id = t_ref_class_act.act_id "
		" LEFT JOIN t_prop ON t_prop.id = t_ref_act_prop.prop_id "
		" WHERE t_ref_class_act.cls_id = %s "
		, data.mCls.mID);
	return true;
}
