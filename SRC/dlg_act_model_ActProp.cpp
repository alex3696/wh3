#include "_pch.h"
#include "dlg_act_model_ActProp.h"
#include "dlg_act_model_Obj.h"

using namespace wh;
using namespace wh::dlg_act::model;


//-----------------------------------------------------------------------------
// Act
//-----------------------------------------------------------------------------
Prop::Prop(const char option)
	: TModelData<DataType>(option)
{
}


//-------------------------------------------------------------------------
bool Prop::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	data.mProp.mId = table->GetAsLong(0, row);
	data.mProp.mLabel = table->GetAsString(1, row);
	table->GetAsString(2, row, data.mProp.mType);
	//table->GetAsString(3, row, data.mVal);
	SetData(data);
	return true;
};


//-----------------------------------------------------------------------------
// PropArray
//-----------------------------------------------------------------------------
PropArray::PropArray()
	: TModelArray<ItemType>(ModelOption::EnableParentNotify | ModelOption::CascadeLoad)
{
}

//-----------------------------------------------------------------------------
bool PropArray::GetSelectChildsQuery(wxString& query)const
{
	auto actModel = dynamic_cast<Act*>(GetParent());
	if (!actModel)
		return false;

	const rec::Act& act = actModel->GetData();

	query = wxString::Format(
		" SELECT t_prop.id, t_prop.label, t_prop.type "
		" FROM t_ref_act_prop "
		" LEFT JOIN t_prop ON t_ref_act_prop.prop_id = t_prop.id "
		" WHERE act_id = %s "
		, act.mID
		);
	return true;
}
