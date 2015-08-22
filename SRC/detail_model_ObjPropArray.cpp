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
