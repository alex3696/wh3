#include "_pch.h"
#include "MClsProp.h"
#include "MObjCatalog.h"

using namespace wh;


const std::vector<Field> gClsPropFieldVec = {
		{ "Имя", FieldType::ftName, true },
		{ "Значение", FieldType::ftText, true },
		{ "Тип", FieldType::ftName, true },
		{ "PropID", FieldType::ftInt, true },
		{ "ID", FieldType::ftInt, true }
};


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MClsProp::MClsProp(const char option)
:TModelData<rec::ClsProp>(option)
{
}

//-------------------------------------------------------------------------
bool MClsProp::GetSelectQuery(wxString& query)const
{
	auto clsProp = GetData();
	query = wxString::Format(
		"SELECT t_prop.id AS prop_id "
		" , t_prop.type AS prop_type "
		" , t_prop.label AS prop_label "
		" , val "
		" , t_cls_prop.id AS id "
		" FROM t_cls_prop "
		" LEFT JOIN t_prop ON t_prop.id = t_cls_prop.prop_id "
		" WHERE t_cls_prop.id = %s "
		, clsProp.mID);
	return true;
}
//-------------------------------------------------------------------------
bool MClsProp::GetInsertQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsPropArray*>(GetParent());
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(parentArray->GetParent());

	if (parentCls)
	{
		const rec::ClsProp& prop = this->GetData();
		const rec::Cls& cls = parentCls->GetData();

		query = wxString::Format(
			"INSERT INTO t_cls_prop (val, cls_id, prop_id) "
			" VALUES(%s, '%s', '%s') "
			" RETURNING id "
			, prop.mVal.IsEmpty() ? "NULL" : wxString::Format("'%s'", prop.mVal)
			, cls.mID
			, prop.mProp.mID);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MClsProp::GetUpdateQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsPropArray*>(this->mParent);
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(parentArray->GetParent());

	if (parentCls)
	{
		const rec::ClsProp& prop = this->GetData();
		const rec::Cls& cls = parentCls->GetData();

		query = wxString::Format(
			"UPDATE	t_class_prop "
			" SET val=%s, cls_id='%s', prop_id='%s' "
			" WHERE id=%s "
			, prop.mVal.IsEmpty() ? L"NULL" : wxString::Format(L"'%s'", prop.mVal)
			, cls.mID
			, prop.mProp.mID
			, prop.mID);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MClsProp::GetDeleteQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsPropArray*>(GetParent());
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(parentArray->GetParent());

	if (parentCls)
	{
		const rec::ClsProp& prop = this->GetData();
		//const rec::Cls& cls = parentCls->GetData();

		query = wxString::Format(
			"DELETE FROM t_cls_prop WHERE id = %s ",
			prop.mID);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MClsProp::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	table->GetAsString(0, row, data.mProp.mID);
	table->GetAsString(1, row, data.mProp.mLabel);
	table->GetAsString(2, row, data.mProp.mType);
	table->GetAsString(3, row, data.mVal);
	table->GetAsString(4, row, data.mID);
	SetData(data);
	return true;
};
//-------------------------------------------------------------------------
bool MClsProp::GetFieldValue(unsigned int col, wxVariant &variant)
{
	const auto& data = this->GetData();
	auto mgr = ResMgr::GetInstance();
	switch (col)
	{
	default:	break;
	case 1:
		variant = variant << wxDataViewIconText(data.mProp.mLabel, mgr->m_ico_classprop24);
		break;
	case 2:	variant = data.mVal;						break;
	case 3: variant = data.mProp.GetTypeString();	break;
	case 4: variant = data.mProp.mID;				break;
	case 5: variant = data.mID;						break;
	}//switch(col) 
	return true;
}
//-------------------------------------------------------------------------
const std::vector<Field>& MClsProp::GetFieldVector()const
{
	return gClsPropFieldVec;
}




//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MClsPropArray::MClsPropArray(const char option)
	:TModelArray<T_Item>(option)
{
}
//-------------------------------------------------------------------------
bool MClsPropArray::GetSelectChildsQuery(wxString& query)const
{
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(GetParent());
	if (parentCls)
	{
		auto cls = parentCls->GetData();

		query = wxString::Format(
			"SELECT t_prop.id AS prop_id "
			" , t_prop.label AS prop_label "
			" , t_prop.type AS prop_type "
			" , val "
			" , t_cls_prop.id AS id "
			" FROM t_cls_prop "
			" LEFT JOIN t_prop ON t_prop.id = t_cls_prop.prop_id "
			" WHERE t_cls_prop.cls_id = %s "
			, cls.mID);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
std::shared_ptr<IModel> MClsPropArray::CreateChild()
{
	auto child = new T_Item;
	child->SetData(T_Item::T_Data());
	return std::shared_ptr<IModel>(child);
}

//-------------------------------------------------------------------------
bool MClsPropArray::LoadChildDataFromDb(std::shared_ptr<IModel>& model,
	std::shared_ptr<whTable>& table, const size_t pos)
{
	auto child = std::dynamic_pointer_cast<MClsProp>(model);
	if (child)
		return child->LoadThisDataFromDb(table, pos);
	return false;
};

//-------------------------------------------------------------------------
const std::vector<Field>& MClsPropArray::GetFieldVector()const
{
	return gClsPropFieldVec;
}

