#include "_pch.h"
#include "detail_model.h"

using namespace wh;
using namespace wh::detail::model;

//-----------------------------------------------------------------------------
ObjPropValLoader::ObjPropValLoader(const char option)
	:IModel(option)
{
}
//-----------------------------------------------------------------------------
void ObjPropValLoader::LoadData()
{
	wxString query;
	const bool queryExist = GetSelectQuery(query);
	if (queryExist)
	{
		auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
		if (table && table->GetRowCount())
			LoadThisDataFromDb(table, 0);
	}
}
//-----------------------------------------------------------------------------
bool ObjPropValLoader::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	auto obj = dynamic_cast<Obj*>(GetParent());
	if (!obj)
		return false;

	const auto& obj_data = obj->GetData();
	if (!obj_data.mCls.IsNumberic())
		return false;

	auto propArray = obj->GetObjPropArray();

	auto prop_qty = propArray->GetChildQty();
	for (size_t i = 0; i < prop_qty; ++i)
	{
		auto prop = std::dynamic_pointer_cast<ObjProp>(propArray->GetChild(i));
		if (prop)
		{
			auto prop_data = prop->GetData();
			prop_data.mVal = table->GetAsString(i + 1, 0);
			prop->SetData(prop_data);
		}
	}

	return true;
}
//-----------------------------------------------------------------------------
bool ObjPropValLoader::GetSelectQuery(wxString& query)const
{
	auto obj = dynamic_cast<Obj*>(GetParent());
	if (!obj)
		return false;

	const auto& obj_data = obj->GetData();
	if (!obj_data.mCls.IsNumberic())
		return false;

	auto propArray = obj->GetObjPropArray();

	wxString prop_fields;
	auto prop_qty = propArray->GetChildQty();
	for (size_t i = 0; i < prop_qty; ++i)
	{
		auto prop = std::dynamic_pointer_cast<ObjProp>(propArray->GetChild(i));
		if (prop)
		{
			const auto& prop_data = prop->GetData();
			prop_fields += wxString::Format(", \"%s\"", prop_data.mProp.mLabel.toStr());
		}
	}

	query = wxString::Format(
		"SELECT obj_id %s "
		" FROM t_state_%s "
		" WHERE obj_id = %s "
		, prop_fields
		, obj_data.mCls.mID.SqlVal()
		, obj_data.mObj.mId.SqlVal() 
		);

	return true;
}
//-----------------------------------------------------------------------------
