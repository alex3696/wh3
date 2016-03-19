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
	if (obj_data.mCls.IsAbstract())
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
	if (obj_data.mCls.mType.IsNull() || obj_data.mCls.IsAbstract())
		return false;

	auto propArray = obj->GetObjPropArray();

	wxString fields, leftJoin;
	auto prop_qty = propArray->GetChildQty();

	wxString qq;
	for (size_t i = 0; i < prop_qty; ++i)
	{
		auto prop = std::dynamic_pointer_cast<ObjProp>(propArray->GetChild(i));
		if (prop)
		{
			const auto& prop_data = prop->GetData();
			qq += wxString::Format("\"%s\" TEXT,", prop_data.mProp.mId.toStr() );
		}
	}

	if (!qq.IsEmpty())
	{
		qq.replace(qq.size() - 1, 1, " ");
		leftJoin = wxString::Format(
			" LEFT JOIN LATERAL jsonb_to_record(prop) as x(%s) ON true "
			, qq);
		fields = ", x.*";
	}


	query = wxString::Format(
		"SELECT id %s "
		" FROM obj "
		" %s "
		" WHERE id = %s "
		, fields
		, leftJoin
		, obj_data.mObj.mId.SqlVal() 
		);

	return true;
}
//-----------------------------------------------------------------------------
