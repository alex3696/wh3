#include "_pch.h"
#include "MAct.h"

using namespace wh;




const std::vector<Field> gActPropFieldVec = {
		{ "Èìÿ", FieldType::ftName, true },
		{ "RefID", FieldType::ftInt, true },
		{ "PropID", FieldType::ftInt, true }
};



//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MActProp::MActProp(const char option)
	:TModelData<rec::ActProp>(option)
{
}

//-------------------------------------------------------------------------
bool MActProp::GetSelectQuery(wxString& query)const
{
	auto data = GetData();
	query = wxString::Format(
		"SELECT t_ref_act_prop.id, prop_label FROM t_ref_act_prop "
		" LEFT JOIN t_prop ON t_prop.id = t_ref_act_prop.prop_id "
		" WHERE id = %s",
		data.mID);
	return true;
}
//-------------------------------------------------------------------------
bool MActProp::GetInsertQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MActPropArray*>(this->mParent);
	auto parentAct = dynamic_cast<MAct*>(parentArray->GetParent() );
	
	if (parentAct)
	{
		auto prop = GetData();
		auto act = parentAct->GetData();

		query = wxString::Format("INSERT INTO t_ref_act_prop (prop_id, act_id) "
			"VALUES (%s, %s) RETURNING id, prop_id",
			prop.mProp.mID, act.mID);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MActProp::GetUpdateQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MActPropArray*>(this->mParent);
	auto parentAct = dynamic_cast<MAct*>(parentArray->GetParent());

	if (parentAct)
	{
		auto prop = GetData();
		auto act = parentAct->GetData();
	
		query = wxString::Format("UPDATE t_ref_act_prop SET "
								"prop_id='%s', act_id='%s' "
								"WHERE id = %s ",
								prop.mProp.mID, act.mID,
								prop.mID);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MActProp::GetDeleteQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MActPropArray*>(this->mParent);
	auto parentAct = dynamic_cast<MAct*>(parentArray->GetParent());

	if (parentAct)
	{
		auto prop = GetData();
		auto act = parentAct->GetData();

		query = wxString::Format("DELETE FROM t_ref_act_prop WHERE id = %s ",
								prop.mID );
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MActProp::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	table->GetAsString(0, row, data.mID);
	table->GetAsString(1, row, data.mProp.mLabel);
	table->GetAsString(2, row, data.mProp.mID);
	SetData(data);
	return true;
};
//-------------------------------------------------------------------------
bool MActProp::GetFieldValue(unsigned int col, wxVariant &variant)
{
	const auto& data = this->GetData();
	auto mgr = ResMgr::GetInstance();
	switch (col)
	{
	default:break;
	case 1:	variant = variant << wxDataViewIconText(data.mProp.mLabel, mgr->m_ico_classprop24);
			break;
	case 2:	variant = data.mID;			break;
	case 3:	variant = data.mProp.mID;	break;
	}//switch(col) 
	return true;
}
//-------------------------------------------------------------------------
const std::vector<Field>& MActProp::GetFieldVector()const
{
	return gActPropFieldVec;
}




//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MActPropArray::MActPropArray(const char option)
	:TModelArray<T_Item>(option)
{
}
//-------------------------------------------------------------------------
bool MActPropArray::GetSelectChildsQuery(wxString& query)const
{
	auto parentAct = dynamic_cast<MAct*>(this->mParent);
	if (parentAct)
	{
		auto act = parentAct->GetData();
		query = wxString::Format(
			"SELECT t_ref_act_prop.id, t_prop.label,t_prop.id FROM t_ref_act_prop "
			" LEFT JOIN t_prop ON t_prop.id = t_ref_act_prop.prop_id "
			" WHERE act_id = %s ", act.mID);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
std::shared_ptr<IModel> MActPropArray::CreateChild()
{
	auto child = new T_Item;
	child->SetData(T_Item::T_Data() );
	return std::shared_ptr<IModel>(child);
}

//-------------------------------------------------------------------------
bool MActPropArray::LoadChildDataFromDb(std::shared_ptr<IModel>& model,
	std::shared_ptr<whTable>& table, const size_t pos)
{
	auto child = std::dynamic_pointer_cast<MActProp>(model);
	if (child)
		return child->LoadThisDataFromDb(table, pos);
	return false;
};

//-------------------------------------------------------------------------
const std::vector<Field>& MActPropArray::GetFieldVector()const
{
	return gActPropFieldVec;
}