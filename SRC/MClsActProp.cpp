#include "_pch.h"
#include "MClsActProp.h"
#include "MClsBase.h"

using namespace wh;




const std::vector<Field> gClsActPropFieldVec = {
		{ "Свойство", FieldType::ftName, true },
		{ "Действие", FieldType::ftName, true },
		{ "ID", FieldType::ftInt, true }
};



//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MClsActProp::MClsActProp(const char option)
	:TModelData<rec::ClsActProp>(option)
{
}

//-------------------------------------------------------------------------
bool MClsActProp::GetSelectQuery(wxString& query)const
{
	//auto parentArray = dynamic_cast<MClsActPropArray*>(this->mParent);
	//auto parentCls  = dynamic_cast<MClsBase*>(parentArray->GetParent());
	const auto& favProp = GetData();
	query = wxString::Format(
		"SELECT t_favorite_prop.id, act_id, prop_id, t_act.label, t_prop.label "
		" FROM t_favorite_prop "
		" LEFT JOIN t_prop ON t_prop.id = t_favorite_prop.prop_id "
		" LEFT JOIN t_act  ON t_act.id  = t_favorite_prop.act_id "
		" WHERE id = %s",
		favProp.mID);
	return true;
}
//-------------------------------------------------------------------------
bool MClsActProp::GetInsertQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsActPropArray*>(this->mParent);
	auto parentCls  = dynamic_cast<MClsBase*>(parentArray->GetParent());

	if (parentCls)
	{
		const auto& favProp = GetData();
		const auto& cls = parentCls->GetData();

		query = wxString::Format(
			"INSERT INTO t_favorite_prop(cls_id, act_id, prop_id) "
			"VALUES (%s, %s, %s) "
			"RETURNING id, act_id, prop_id "
			", (SELECT label FROM t_act WHERE id = act_id) "
			", (SELECT label FROM t_prop WHERE id = prop_id) "
			, cls.mID, favProp.mActID, favProp.mPropID);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MClsActProp::GetUpdateQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsActPropArray*>(this->mParent);
	auto parentCls = dynamic_cast<MClsBase*>(parentArray->GetParent());

	if (parentCls)
	{
		const auto& favProp = GetData();
		const auto& cls = parentCls->GetData();

		query = wxString::Format(
			"UPDATE t_favorite_prop "
			" SET cls_id=%s, act_id=%s, prop_id=%s "
			" WHERE id = %s "
			, cls.mID, favProp.mActID, favProp.mPropID
			, favProp.mID);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MClsActProp::GetDeleteQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsActPropArray*>(this->mParent);
	auto parentCls = dynamic_cast<MClsBase*>(parentArray->GetParent());

	if (parentCls)
	{
		const auto& favProp = GetData();
		//const auto& cls = parentCls->GetData();

		query = wxString::Format("DELETE FROM t_favorite_prop WHERE id = %s ",
			favProp.mID);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MClsActProp::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	table->GetAsString(0, row, data.mID);
	table->GetAsString(1, row, data.mActID);
	table->GetAsString(2, row, data.mPropID);
	table->GetAsString(3, row, data.mActLabel);
	table->GetAsString(4, row, data.mPropLabel);
	SetData(data);
	return true;
};
//-------------------------------------------------------------------------
bool MClsActProp::GetFieldValue(unsigned int col, wxVariant &variant)
{
	const auto& data = this->GetData();
	auto mgr = ResMgr::GetInstance();
	switch (col)
	{
	default:break;
	case 1:	variant = variant << wxDataViewIconText(data.mPropLabel, mgr->m_ico_classprop24);
		break;
	case 2:	variant = data.mActLabel;	break;
	case 3:	variant = data.mID;			break;
	}//switch(col) 
	return true;
}
//-------------------------------------------------------------------------
const std::vector<Field>& MClsActProp::GetFieldVector()const
{
	return gClsActPropFieldVec;
}




//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MClsActPropArray::MClsActPropArray(const char option)
	:IModel(option)
{
}
//-------------------------------------------------------------------------
bool MClsActPropArray::GetSelectChildsQuery(wxString& query)const
{

	auto parentCls = dynamic_cast<MClsBase*>(this->mParent);
	if (parentCls)
	{
		const auto& cls = parentCls->GetData();

		query = wxString::Format(
			"SELECT t_favorite_prop.id, act_id, prop_id, t_act.label, t_prop.label "
			" FROM t_favorite_prop "
			" LEFT JOIN t_prop ON t_prop.id = t_favorite_prop.prop_id "
			" LEFT JOIN t_act  ON t_act.id  = t_favorite_prop.act_id "
			" WHERE cls_id = %s AND user_label=CURRENT_USER "
			, cls.mID);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
std::shared_ptr<IModel> MClsActPropArray::CreateChild()
{
	auto child = new T_Item;
	child->SetData(T_Item::T_Data());
	return std::shared_ptr<IModel>(child);
}

//-------------------------------------------------------------------------
bool MClsActPropArray::LoadChildDataFromDb(std::shared_ptr<IModel>& model,
	std::shared_ptr<whTable>& table, const size_t pos)
{
	auto child = std::dynamic_pointer_cast<MClsActProp>(model);
	if (child)
		return child->LoadThisDataFromDb(table, pos);
	return false;
};

//-------------------------------------------------------------------------
const std::vector<Field>& MClsActPropArray::GetFieldVector()const
{
	return gClsActPropFieldVec;
}




//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MClsAllActPropArray::MClsAllActPropArray(const char option)
	:MClsActPropArray(option)
{
}
//-------------------------------------------------------------------------
bool MClsAllActPropArray::GetSelectChildsQuery(wxString& query)const
{

	auto parentCls = dynamic_cast<MClsBase*>(this->mParent);
	if (parentCls)
	{
		const auto& cls = parentCls->GetData();

		query = wxString::Format(
			"SELECT NULL AS id, t_act.id, t_prop.id, t_act.label, t_prop.label "
			" FROM t_ref_class_act "
			" LEFT JOIN t_ref_act_prop USING(act_id) "
			" RIGHT JOIN t_prop ON t_prop.id = t_ref_act_prop.prop_id "
			" RIGHT JOIN t_act  ON t_act.id = t_ref_act_prop.act_id "
			" WHERE t_ref_class_act.cls_id = %s "
			, cls.mID);
		return true;
	}
	return false;
}