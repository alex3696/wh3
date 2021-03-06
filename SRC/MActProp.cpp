#include "_pch.h"
#include "MAct.h"

using namespace wh;




const std::vector<Field> gActPropFieldVec = {
		{ "���", FieldType::ftName, true },
		{ "RefID", FieldType::ftLong, true },
		{ "PropID", FieldType::ftLong, true }
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
		"SELECT ref_act_prop.id, prop_title FROM ref_act_prop "
		" LEFT JOIN prop ON prop.id = ref_act_prop.prop_id "
		" WHERE id = %s",
		data.mId.SqlVal());
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

		query = wxString::Format("INSERT INTO ref_act_prop (prop_id, act_id) "
			"VALUES (%s, %s) RETURNING id, prop_id",
			prop.mProp.mId.SqlVal(), act.mID);
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
	
		query = wxString::Format("UPDATE ref_act_prop SET "
								"prop_id='%s', act_id='%s' "
								"WHERE id = %s ",
								prop.mProp.mId.SqlVal(), act.mID,
								prop.mId.SqlVal() );
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

		query = wxString::Format("DELETE FROM ref_act_prop WHERE id = %s ",
								prop.mId.SqlVal() );
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MActProp::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	data.mId = table->GetAsString(0, row);
	data.mProp.mLabel = table->GetAsString(1, row);
	data.mProp.mId = table->GetAsLong(2, row);
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
	case 2:	variant = data.mId.toStr();			break;
	case 3:	variant = data.mProp.mId.toStr();	break;
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
			"SELECT ref_act_prop.id, prop.title, prop.id FROM ref_act_prop "
			" LEFT JOIN prop ON prop.id = ref_act_prop.prop_id "
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