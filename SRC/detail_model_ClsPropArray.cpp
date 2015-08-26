#include "_pch.h"
#include "detail_model.h"

using namespace wh;
using namespace wh::detail::model;

//-----------------------------------------------------------------------------
ClsProp::ClsProp(const char option)
	:TModelData<rec::PropVal>(option)
{
}
//-----------------------------------------------------------------------------
bool ClsProp::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
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
ClsPropArray::ClsPropArray(const char option)
	:TModelArray<ClsProp>()
{
}
//-----------------------------------------------------------------------------
bool ClsPropArray::GetSelectChildsQuery(wxString& query)const
{
	auto parentCls = dynamic_cast<Obj*>(GetParent());
	if (parentCls)
	{
		const auto& data = parentCls->GetData();

		query = wxString::Format(
			"SELECT t_prop.id AS prop_id "
			" , t_prop.label AS prop_label "
			" , t_prop.type AS prop_type "
			" , val "
			" , t_cls_prop.id AS id "
			" FROM t_cls_prop "
			" LEFT JOIN t_prop ON t_prop.id = t_cls_prop.prop_id "
			" WHERE t_cls_prop.cls_id = %s "
			, data.mCls.mID);
		return true;
	}
	return false;
}
